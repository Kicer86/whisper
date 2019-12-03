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

#include "encrypted_client.hpp"

#include <QTcpSocket>

#include "encrypted_connection.hpp"


EncryptedClient::EncryptedClient()
{
}


std::unique_ptr<IEncryptedConnection> EncryptedClient::makeConnection(const QString& address, quint16 port)
{
    QTcpSocket* socket = new QTcpSocket;
    socket->connectToHost(address, port);

    auto encryptedConnection = std::make_unique<EncryptedConnection>(socket);

    return encryptedConnection;
}
