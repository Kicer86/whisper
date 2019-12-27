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

#include "connection_manager.hpp"

#include <botan/x509_key.h>
#include <QDebug>

#include "iuser_manager.hpp"


ConnectionManager::ConnectionManager(const IUserManager& usrMgr)
    : m_userManager(usrMgr)
{
}


ConnectionManager::~ConnectionManager()
{
    for (auto& connection: m_connections)
        connection->close();
}


void ConnectionManager::add(std::unique_ptr<IEncryptedConnection> connection)
{
    qDebug() << "registering new connection";

    const auto& users = m_userManager.listUsers();
    const Botan::Public_Key* public_key = connection->getTheirsPublicKey();
    std::string public_key_encoded = Botan::X509::PEM_encode(*public_key);

    for(const UserId& userId: users)
    {
        const QByteArray pkey = m_userManager.publicKey(userId);

        if (public_key_encoded.c_str() == pkey)
        {
            qDebug() << "matched connection to existing user";
            break;
        }
    }

    m_connections.emplace_back(std::move(connection));
}
