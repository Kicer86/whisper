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


EncryptedConnection::EncryptedConnection(QTcpSocket* socket)
    : m_socket(socket)
    , m_state(WaitForTheirsPublicKey)
{
    connect(m_socket, &QTcpSocket::stateChanged, this, &EncryptedConnection::socketStateChanged);
    connect(m_socket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error), this, &EncryptedConnection::socketError);
    connect(m_socket, &QTcpSocket::readyRead, this, &EncryptedConnection::readyRead);
}


QSslKey EncryptedConnection::getTheirsPublicKey() const
{
    return m_theirsPublicKey;
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
