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

#ifndef CONNECTIONMONITOR_HPP
#define CONNECTIONMONITOR_HPP

#include <memory>
#include <map>
#include <QObject>

#include "../utils.hpp"

class EncryptedConnection;
class IConnectionManager;
class IEncryptedConnection;

/**
 * @brief class for monitoring not yet established connections
 */
class ConnectionMonitor: public QObject
{
    public:
        ConnectionMonitor(IConnectionManager &);
        ~ConnectionMonitor();

        void watch(std::unique_ptr<EncryptedConnection>);

    private:
        typedef std::vector<QMetaObject::Connection> Connections;
        std::map<std::unique_ptr<EncryptedConnection>, Connections, utils::pointer_comp<EncryptedConnection>> m_waitingForApproval;
        IConnectionManager& m_connectionManager;

        void connectionEstablished(EncryptedConnection *);
        void connectionClosed(EncryptedConnection *);
};

#endif // CONNECTIONMONITOR_HPP
