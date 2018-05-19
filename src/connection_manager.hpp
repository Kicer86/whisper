/*
 * Utility for connections management
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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

#include <memory>

#include <QObject>


class Server;
class Client;
struct IUserManager;

class ConnectionManager: public QObject
{
    public:
        ConnectionManager(IUserManager *);
        ~ConnectionManager();

    private:
        std::unique_ptr<Server> m_server;
        std::unique_ptr<Client> m_client;
        IUserManager* m_userManager;
};

#endif // CONNECTIONMANAGER_HPP
