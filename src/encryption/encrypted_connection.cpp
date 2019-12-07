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
#include <openssl/rand.h>

#include "utils.hpp"


EncryptedConnection::EncryptedConnection(const QSslKey& oursPublicKey, const QString& host, quint16 port)
    : m_oursPublicKey(oursPublicKey)
    , m_socket(new QTcpSocket(this))
    , m_state(WaitForConnectionValidation)
{
    connectToSocketSignals();

    m_socket->connectToHost(host, port);

    sendPublicKey();
}



EncryptedConnection::EncryptedConnection(const QSslKey& oursPublicKey, QTcpSocket* socket)
    : m_oursPublicKey(oursPublicKey)
    , m_socket(socket)
    , m_state(ValidateIncomingConnection)
{
    socket->setParent(this);
    connectToSocketSignals();
}


QSslKey EncryptedConnection::getTheirsPublicKey() const
{
    return m_theirsPublicKey;
}


void EncryptedConnection::connectToSocketSignals()
{
    connect(m_socket, &QTcpSocket::stateChanged, this, &EncryptedConnection::socketStateChanged);
    connect(m_socket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error), this, &EncryptedConnection::socketError);
    connect(m_socket, &QTcpSocket::readyRead, this, &EncryptedConnection::readyRead);
}


void EncryptedConnection::sendPublicKey()
{
    const QByteArray keyByteArray = m_oursPublicKey.toPem();
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
    RAND_bytes(m_symmetricKey.data(), m_symmetricKeySize);

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
        assert(m_theirsPublicKey.isNull());
        const QByteArray theirsPublicKey = m_socket->read(size);

        m_theirsPublicKey = QSslKey(theirsPublicKey, QSsl::Rsa, QSsl::Pem, QSsl::PublicKey);
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
                    m_state = ConnectionEstablished;

                    std::cout << "client accepted\n";
                    break;
                }

                // client side only
                case WaitForConnectionValidation:
                {
                    readTheirsPublicKey();

                    /// @todo: validate connection
                    m_state = ConnectionEstablished;

                    std::cout << "accepted by server\n";
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
