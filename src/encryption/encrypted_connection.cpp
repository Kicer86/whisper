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

#include <iostream>
#include <botan/data_src.h>
#include <botan/pubkey.h>
#include <botan/x509_key.h>
#include <botan/system_rng.h>
#include <botan/auto_rng.h>

#include "iencryption_primitives_provider.hpp"
#include "utils.hpp"


EncryptedConnection::EncryptedConnection(const IKeysProvider* ourKeys, const QString& host, quint16 port)
    : m_ourKeys(ourKeys)
    , m_socket(new QTcpSocket(this))
    , m_state(WaitForConnectionValidation)
    , m_symmetricKey(32)
{
    connectToSocketSignals();

    m_socket->connectToHost(host, port);

    sendPublicKey();
}



EncryptedConnection::EncryptedConnection(const IKeysProvider* ourKeys, QTcpSocket* socket)
    : m_ourKeys(ourKeys)
    , m_socket(socket)
    , m_state(ValidateIncomingConnection)
    , m_symmetricKey(32)
{
    socket->setParent(this);
    connectToSocketSignals();
}


const Botan::Public_Key* EncryptedConnection::getTheirsPublicKey() const
{
    return m_theirsPublicKey.get();
}


void EncryptedConnection::connectToSocketSignals()
{
    connect(m_socket, &QTcpSocket::stateChanged, this, &EncryptedConnection::socketStateChanged);
    connect(m_socket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error), this, &EncryptedConnection::socketError);
    connect(m_socket, &QTcpSocket::readyRead, this, &EncryptedConnection::readyRead);
}


void EncryptedConnection::sendPublicKey()
{
    auto ourPublicKey = m_ourKeys->ourPublicKey();
    const QByteArray keyByteArray = Botan::X509::PEM_encode(*ourPublicKey.get()).c_str();
    const int keyByteArraySize = keyByteArray.size();
    assert(keyByteArraySize < 65536);

    /// @todo Use some nice tool for sending data instead of sending raw bytes.
    ///       It would be nice to aboif little/big endian problems
    const quint16 public_key_size = keyByteArraySize;
    const char* public_key_size_chars = utils::binary_cast<const char[2]>(public_key_size);

    m_socket->write(public_key_size_chars, 2);
    m_socket->write(keyByteArray);
}


void EncryptedConnection::sendSymmetricKey()
{
    std::unique_ptr<Botan::RandomNumberGenerator> rng;
    #if defined(BOTAN_HAS_SYSTEM_RNG)
    rng.reset(new Botan::System_RNG);
    #else
    rng.reset(new Botan::AutoSeeded_RNG);
    #endif

    for(int i = 0; i < m_symmetricKeySize; i++)
        m_symmetricKey[i] = rng->next_byte();

    Botan::PK_Encryptor_EME enc(*m_theirsPublicKey, *rng.get(), "EME1(SHA-256)");
    std::vector<uint8_t> encrypted_key = enc.encrypt(m_symmetricKey,*rng.get());

    const quint16 encrypted_message_size = encrypted_key.size();
    const char* encrypted_message_size_chars = utils::binary_cast<const char[2]>(encrypted_message_size);

    m_socket->write(encrypted_message_size_chars, 2);
    m_socket->write(reinterpret_cast<const char *>(encrypted_key.data()), encrypted_key.size());
}


void EncryptedConnection::readTheirsPublicKey()
{
    if (m_socket->bytesAvailable() < 2)
        throw not_enouth_data{};

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
    {
        assert(m_theirsPublicKey.get() == nullptr);
        QByteArray theirsPublicKey = m_socket->read(size);

        Botan::DataSource_Memory publicKeyDS(theirsPublicKey.toStdString());

        m_theirsPublicKey.reset(Botan::X509::load_key(publicKeyDS));
    }
}


void EncryptedConnection::readSymmetricKey()
{
    if (m_socket->bytesAvailable() < 2)
        throw not_enouth_data{};

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
    {
        std::unique_ptr<Botan::RandomNumberGenerator> rng;
        #if defined(BOTAN_HAS_SYSTEM_RNG)
        rng.reset(new Botan::System_RNG);
        #else
        rng.reset(new Botan::AutoSeeded_RNG);
        #endif

        QByteArray encrypted_symmetric_key = m_socket->read(size);

        std::unique_ptr<Botan::Private_Key> private_key = m_ourKeys->ourPrivateKey();
        Botan::PK_Decryptor_EME dec(*private_key.get(), *rng.get(), "EME1(SHA-256)");

        const uint8_t* encrypted_symmetric_key_bytes = reinterpret_cast<const uint8_t*>(encrypted_symmetric_key.constData());
        auto decrypted_symmetric_key = dec.decrypt(encrypted_symmetric_key_bytes, encrypted_symmetric_key.size());

        m_symmetricKey.clear();
        std::copy(decrypted_symmetric_key.cbegin(),
                  decrypted_symmetric_key.cend(),
                  std::back_inserter(m_symmetricKey));
    }
}


void EncryptedConnection::socketStateChanged(QAbstractSocket::SocketState socketState)
{
    std::cout << "client socket state changed to: " << socketState << "\n";
}


void EncryptedConnection::socketError(QAbstractSocket::SocketError)
{
    std::cout << "client socket error: " << m_socket->errorString().toStdString() << "\n";
}


void EncryptedConnection::readyRead()
{
    try
    {
        while (m_socket->bytesAvailable())
            switch (m_state)
            {
                // server side only
                case ValidateIncomingConnection:
                {
                    readTheirsPublicKey();

                    /// @todo: validate connection
                    sendPublicKey();
                    sendSymmetricKey();
                    m_state = ConnectionEstablished;

                    std::cout << "client accepted\n";
                    break;
                }

                // client side only
                case WaitForConnectionValidation:
                {
                    readTheirsPublicKey();

                    /// @todo: validate connection
                    m_state = WaitForSymmetricKey;

                    std::cout << "accepted by server\n";
                    break;
                }

                case WaitForSymmetricKey:
                {
                    readSymmetricKey();

                    m_state = ConnectionEstablished;
                    break;
                }

                case ConnectionEstablished:
                {
                    throw unexpected_data{};
                    break;
                }
            }
    }
    catch(const not_enouth_data &) {}       /// this is not failure, wait for more
    catch(const unexpected_data &) {}       /// some error in protocol, @todo kill connection
}
