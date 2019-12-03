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


EncryptedConnection::EncryptedConnection(QTcpSocket* socket):
    m_socket(socket)
{
    connect(m_socket, &QTcpSocket::stateChanged, this, &EncryptedConnection::socketStateChanged);
    connect(m_socket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error), this, &EncryptedConnection::socketError);
}


void EncryptedConnection::socketStateChanged(QAbstractSocket::SocketState socketState)
{
    std::cout << "client socket state changed to " << socketState << "\n";
}


void EncryptedConnection::socketError(QAbstractSocket::SocketError)
{
    std::cout << "client socket state changed to " << m_socket->errorString().toStdString() << "\n";
}
