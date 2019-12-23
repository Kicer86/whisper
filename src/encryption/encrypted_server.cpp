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

#include "encrypted_server.hpp"

#include <memory>
#include <QTcpSocket>

#include "encrypted_connection.hpp"
#include "iconnection_manager.hpp"


EncryptedServer::EncryptedServer(const IEncryptionPrimitivesProvider* ourKeys, const IIdentityChecker& identityChecker, IConnectionManager& connection_manager)
    : m_ourKeys(ourKeys)
    , m_identityChecker(identityChecker)
    , m_connectionManager(connection_manager)
{
    connect(this, &QTcpServer::newConnection, this, &EncryptedServer::newConnection);
}


EncryptedServer::~EncryptedServer()
{

}


void EncryptedServer::newConnection()
{
    while(hasPendingConnections())
    {
        QTcpSocket* socket = nextPendingConnection();

        auto encrypted_connection = std::make_unique<EncryptedConnection>(m_ourKeys, socket);
        m_waitingForApproval.insert(std::move(encrypted_connection));

        connect(encrypted_connection.get(), &EncryptedConnection::connectionEstablished,
                this, &EncryptedServer::connectionEstablished);

        connect(encrypted_connection.get(), &EncryptedConnection::connectionClosed,
                this, &EncryptedServer::connectionClosed);
    }
}


void EncryptedServer::validateTheirsPublicKey(IEncryptedConnection *)
{

}


void EncryptedServer::connectionEstablished(IEncryptedConnection* connection)
{

}


void EncryptedServer::connectionClosed(IEncryptedConnection* connection)
{
    m_waitingForApproval.find(connection);
}
