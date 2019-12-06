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

EncryptedConnection::EncryptedConnection(const QSslKey& oursPublicKey, const QString& host, quint16 port)
    : m_oursPublicKey(oursPublicKey)
    , m_socket(std::make_unique<QTcpSocket>())
    , m_state(WaitForTheirsPublicKey)
{
    connectToSocketSignals();

    m_socket->connectToHost(host, port);

    sendPublicKey();
}



EncryptedConnection::EncryptedConnection(const QSslKey& oursPublicKey, QTcpSocket* socket)
    : m_oursPublicKey(oursPublicKey)
    , m_socket(socket)
    , m_state(WaitForTheirsPublicKey)
{
    connectToSocketSignals();
}


QSslKey EncryptedConnection::getTheirsPublicKey() const
{
    return m_theirsPublicKey;
}


void EncryptedConnection::connectToSocketSignals()
{
    QTcpSocket* s = m_socket.get();

    connect(s, &QTcpSocket::stateChanged, this, &EncryptedConnection::socketStateChanged);
    connect(s, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error), this, &EncryptedConnection::socketError);
    connect(s, &QTcpSocket::readyRead, this, &EncryptedConnection::readyRead);
}


void EncryptedConnection::sendPublicKey()
{
    const QByteArray keyByteArray = m_oursPublicKey.toPem();
    const int keyByteArraySize = keyByteArray.size();
    assert(keyByteArraySize < 65536);

    union
    {
        quint16 publicKeySize;
        char publicKeySizeRaw[2];
    } publicKeyUnion;

    publicKeyUnion.publicKeySize = keyByteArraySize;

    m_socket->write(publicKeyUnion.publicKeySizeRaw, 2);
    m_socket->write(keyByteArray);
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
    /// @todo: extract
    switch (m_state)
    {
        case WaitForTheirsPublicKey:
        {
            if (m_socket->bytesAvailable() < 2)
                break;

            union
            {
                char signedBytes[2];
                quint16 size;
            } sizeTranslator;

            m_socket->getChar(&sizeTranslator.signedBytes[0]);
            m_socket->getChar(&sizeTranslator.signedBytes[1]);

            const int size = sizeTranslator.size;

            if (m_socket->bytesAvailable() < size)
            {
                m_socket->ungetChar(sizeTranslator.signedBytes[1]);
                m_socket->ungetChar(sizeTranslator.signedBytes[0]);
            }
            else
            {
                assert(m_theirsPublicKey.isNull());
                const QByteArray theirsPublicKey = m_socket->read(size);

                m_theirsPublicKey = QSslKey(theirsPublicKey, QSsl::Rsa, QSsl::Pem, QSsl::PublicKey);

                m_state = ConnectionEstablished;
            }

            break;
        }

        case ConnectionEstablished:
        {
            break;
        }
    }
}
