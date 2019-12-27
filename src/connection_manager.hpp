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

#ifndef CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP

#include <map>

#include "encryption/iconnection_manager.hpp"
#include "encryption/iencrypted_connection.hpp"
#include "utils.hpp"

class IUserManager;

/**
 * @brief class for managing connections
 */
class ConnectionManager: public QObject, public IConnectionManager
{
        Q_OBJECT

    public:
        ConnectionManager(const IUserManager &);
        ~ConnectionManager();

        void add(std::unique_ptr<IEncryptedConnection>) override;

    private:
        typedef std::vector<QMetaObject::Connection> Connections;
        std::map<std::unique_ptr<IEncryptedConnection>, Connections, utils::pointer_comp<IEncryptedConnection>> m_connections;

        const IUserManager& m_userManager;

        void connectionClosed(IEncryptedConnection *);
        void dropConnection(IEncryptedConnection *);
};

#endif // CONNECTIONMANAGER_HPP
