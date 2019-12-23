/*
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "encrypted_connection.hpp"

#include <botan/data_src.h>
#include <botan/pubkey.h>
#include <botan/x509_key.h>
#include <botan/system_rng.h>
#include <botan/auto_rng.h>

#include "iencryption_primitives_provider.hpp"
#include "../utils.hpp"

/// @todo: instead of passing or creating TcpSocket here, a QIODevice should be passed.
//         this would make UT possible.


namespace
{
    constexpr int SymmetricKeySize = 32;
}


EncryptedConnection::EncryptedConnection(const IEncryptionPrimitivesProvider* ourKeys, const QString& host, quint16 port)
    : EncryptedConnection(ourKeys, WaitForPublicKeyFromHost)
{
    m_socket = new QTcpSocket(this);
    connectToSocketSignals();

    m_socket->connectToHost(host, port);

    connect(m_socket, &QTcpSocket::connected, [this]()
    {
        sendPublicKey();
    });
}



EncryptedConnection::EncryptedConnection(const IEncryptionPrimitivesProvider* ourKeys, QTcpSocket* socket)
    : EncryptedConnection(ourKeys, WaitForPublicKeyFromHost)
{
    m_socket = socket;
    m_socket->setParent(this);
    connectToSocketSignals();

    sendPublicKey();
}


EncryptedConnection::~EncryptedConnection()
{
    assert(m_socket->state() == QAbstractSocket::UnconnectedState);  // are we killing alive connection?!

    if (m_socket->state() != QAbstractSocket::UnconnectedState)
    {
        m_socket->disconnect(this);    // stop listening, we are getting destroyed
        closeConnection();
    }
}


const Botan::Public_Key* EncryptedConnection::getTheirsPublicKey() const
{
    return m_theirsPublicKey.get();
}


EncryptedConnection::EncryptedConnection(const IEncryptionPrimitivesProvider* ourKeys, State state)
    : m_ourKeys(ourKeys)
    , m_symmetricKeyPart(SymmetricKeySize)
    , m_socket(nullptr)
    , m_state(state)
{
    // prepare part of symmetric key
    Botan::RandomNumberGenerator& rng = m_ourKeys->randomGenerator();

    for(int i = 0; i < SymmetricKeySize; i++)
        m_symmetricKeyPart[i] = rng.next_byte();
}


void EncryptedConnection::connectToSocketSignals()
{
    connect(m_socket, &QTcpSocket::stateChanged, this, &EncryptedConnection::socketStateChanged);
    connect(m_socket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error), this, &EncryptedConnection::socketError);
    connect(m_socket, &QTcpSocket::readyRead, this, &EncryptedConnection::readyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &EncryptedConnection::disconnected);
}


void EncryptedConnection::sendPublicKey()
{
    auto ourPublicKey = m_ourKeys->ourPublicKey();
    const QByteArray keyByteArray = Botan::X509::PEM_encode(*ourPublicKey.get()).c_str();
    const int keyByteArraySize = keyByteArray.size();
    assert(keyByteArraySize < 65536);

    /// @todo Use some nice tool for sending data instead of sending raw bytes.
    ///       It would be nice to aboif little/big endian problems
    const quint16 public_key_size = static_cast<quint16>(keyByteArraySize);
    const char* public_key_size_chars = utils::binary_cast<const char[2]>(public_key_size);

    m_socket->write(public_key_size_chars, 2);
    m_socket->write(keyByteArray);
}


void EncryptedConnection::sendSymmetricKey()
{
    // send our part of symmetric key
    Botan::RandomNumberGenerator& rng = m_ourKeys->randomGenerator();
    Botan::PK_Encryptor_EME enc(*m_theirsPublicKey, rng, "EME1(SHA-256)");
    std::vector<uint8_t> encrypted_key = enc.encrypt(m_symmetricKeyPart, rng);

    const quint16 encrypted_message_size = static_cast<quint16>(encrypted_key.size());
    const char* encrypted_message_size_chars = utils::binary_cast<const char[2]>(encrypted_message_size);

    m_socket->write(encrypted_message_size_chars, 2);
    m_socket->write(reinterpret_cast<const char *>(encrypted_key.data()), encrypted_key.size());
}


void EncryptedConnection::readTheirsPublicKey()
{
    assert(m_theirsPublicKey.get() == nullptr);
    const QByteArray theirsPublicKey = readDataWithSizeHeader();
    Botan::DataSource_Memory publicKeyDS(theirsPublicKey.toStdString());

    m_theirsPublicKey.reset(Botan::X509::load_key(publicKeyDS));
}


void EncryptedConnection::readSymmetricKey()
{
    const QByteArray encrypted_symmetric_key = readDataWithSizeHeader();

    Botan::RandomNumberGenerator& rng = m_ourKeys->randomGenerator();
    std::unique_ptr<Botan::Private_Key> private_key = m_ourKeys->ourPrivateKey();
    Botan::PK_Decryptor_EME dec(*private_key.get(), rng, "EME1(SHA-256)");

    const uint8_t* encrypted_symmetric_key_bytes = reinterpret_cast<const uint8_t*>(encrypted_symmetric_key.constData());
    auto decrypted_symmetric_key = dec.decrypt(encrypted_symmetric_key_bytes, encrypted_symmetric_key.size());

    m_symmetricKey.clear();
    std::copy(decrypted_symmetric_key.cbegin(),
                decrypted_symmetric_key.cend(),
                std::back_inserter(m_symmetricKey));

    if (m_symmetricKey.size() != m_symmetricKeyPart.size())
        throw protocol_error{};

    // xor received key with out part
    for(unsigned int i = 0; i < m_symmetricKey.size(); i++)
        m_symmetricKey[i] ^= m_symmetricKeyPart[i];
}


QByteArray EncryptedConnection::readDataWithSizeHeader()
{
    if (m_socket->bytesAvailable() < 2)
        throw not_enouth_data{};

    QByteArray result;

    char size_table[2];

    m_socket->getChar(&size_table[0]);
    m_socket->getChar(&size_table[1]);

    /// @todo Use some nice tool for sending data instead of sending raw bytes.
    ///       It would be nice to aboif little/big endian problems
    const int size = utils::binary_cast<quint16>(size_table);

    if (m_socket->bytesAvailable() < size)
    {
        // undo any reads - wait for more
        m_socket->ungetChar(size_table[1]);
        m_socket->ungetChar(size_table[0]);

        throw not_enouth_data{};
    }
    else
        result = m_socket->read(size);

    return result;
}


void EncryptedConnection::socketStateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << "client socket state changed to: " << socketState;
}


void EncryptedConnection::socketError(QAbstractSocket::SocketError error)
{
    switch(error)
    {
        case QAbstractSocket::HostNotFoundError:
        case QAbstractSocket::ConnectionRefusedError:
        case QAbstractSocket::RemoteHostClosedError:
            closeConnection();
            break;

        default:
            qDebug() << "client socket error" << error;
            closeConnection();
            break;
    }
}


void EncryptedConnection::readyRead()
{
    try
    {
        while (m_socket->bytesAvailable())
            switch (m_state)
            {
                case WaitForPublicKeyFromHost:
                {
                    readTheirsPublicKey();
                    sendSymmetricKey();

                    m_state = WaitForSymmetricKeyFromHost;

                    qDebug() << "got public key";
                    qDebug() << "sending symetric key";
                    break;
                }

                case WaitForSymmetricKeyFromHost:
                {
                    readSymmetricKey();

                    qDebug() << "got symmetric key";

                    m_state = ConnectionEstablished;
                    break;
                }

                case ConnectionEstablished:
                {
                    throw unexpected_data{};
                    break;
                }

                case Ready:
                {
                    break;
                }
            }

        if (m_state == ConnectionEstablished)
        {
            emit connectionEstablished(this);
            m_state = Ready;
        }
    }
    catch(const not_enouth_data &) {}       /// this is not failure, wait for more
    catch(const unexpected_data &)          /// error in protocol - unexpected data
    {
        qCritical() << "Unexpected data in protocol";
        closeConnection();
    }
    catch(const protocol_error &)           /// something unexpected happend, @todo kill connection
    {
        qCritical() << "Broken protocol";
        closeConnection();
    }
}


void EncryptedConnection::disconnected()
{
    qDebug() << "socket disconnected";

    emit connectionClosed(this);
}


void EncryptedConnection::closeConnection()
{
    qDebug() << "closing connection gracefully";
    m_socket->disconnectFromHost();

    m_socket->state() == QAbstractSocket::UnconnectedState || m_socket->waitForDisconnected(1000);
}
