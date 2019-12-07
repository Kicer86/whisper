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


EncryptedServer::EncryptedServer(const QSslKey& oursPublicKey, const IIdentityChecker& identityChecker, IConnectionManager& connection_manager)
    : m_oursPublicKey(oursPublicKey)
    , m_identityChecker(identityChecker)
    , m_connectionManager(m_connectionManager)
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

        auto encrypted_connection = std::make_unique<EncryptedConnection>(m_oursPublicKey, socket);
        m_waitingForApproval.push_back(std::move(encrypted_connection));
    }
}


void EncryptedServer::validateTheirsPublicKey(IEncryptedConnection* connection)
{

}
