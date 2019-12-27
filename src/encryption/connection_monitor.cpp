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

#include "connection_monitor.hpp"

#include "encrypted_connection.hpp"
#include "iconnection_manager.hpp"


ConnectionMonitor::ConnectionMonitor(IConnectionManager& manager)
    : m_connectionManager(manager)
{
}


ConnectionMonitor::~ConnectionMonitor()
{
    for (auto& connection: m_waitingForApproval)
        connection.first->close();
}


void ConnectionMonitor::watch(std::unique_ptr<EncryptedConnection> connection)
{
    EncryptedConnection* connectionPtr = connection.get();

    auto establishmentConnection = connect(connectionPtr, &EncryptedConnection::connectionEstablished,
                                           std::bind(&ConnectionMonitor::connectionEstablished, this, connectionPtr));

    auto closeConnection = connect(connectionPtr, &EncryptedConnection::connectionClosed,
                                   std::bind(&ConnectionMonitor::connectionClosed, this, connectionPtr));

    m_waitingForApproval.emplace(std::move(connection), Connections{ establishmentConnection, closeConnection } );
}


void ConnectionMonitor::connectionEstablished(EncryptedConnection* connection)
{
    auto connection_ptr = extractConnection(connection);

    if (connection_ptr.get() == nullptr)       // weird - we did not expect this to happen
    {
        qCritical() << "unexpected establishment";

        connection->close();
    }
    else
        m_connectionManager.add(std::move(connection_ptr));
}


void ConnectionMonitor::connectionClosed(EncryptedConnection* connection)
{
    auto it = m_waitingForApproval.find(connection);

    if (it != m_waitingForApproval.end())       // connection closed before being established
        m_waitingForApproval.erase(it);
}


std::unique_ptr<EncryptedConnection> ConnectionMonitor::extractConnection(EncryptedConnection* connection)
{
    std::unique_ptr<EncryptedConnection> result;

    auto it = m_waitingForApproval.find(connection);

    if (it != m_waitingForApproval.end())
    {
        for (auto& connection_obj: it->second)
                QObject::disconnect(connection_obj);

        auto connection_node = m_waitingForApproval.extract(it);
        result = std::move(connection_node.key());
    }

    return result;
}
