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

#ifndef ENCRYPTEDSERVER_HPP
#define ENCRYPTEDSERVER_HPP

#include <memory>
#include <set>
#include <QTcpServer>
#include <QSslKey>
#include <botan/pk_keys.h>

#include "connection_monitor.hpp"

struct IConnectionManager;
struct IIdentityChecker;
struct IEncryptedConnection;
struct IEncryptionPrimitivesProvider;

/**
 * @brief Server providing encrypted connections
 */
class EncryptedServer final: public QTcpServer
{
    public:
        EncryptedServer(const IEncryptionPrimitivesProvider* ourKeys, const IIdentityChecker &, IConnectionManager &);
        ~EncryptedServer();

    private:
        const IEncryptionPrimitivesProvider* m_ourKeys;
        const IIdentityChecker& m_identityChecker;
        ConnectionMonitor m_connectionMonitor;

        void newConnection();
};

#endif // ENCRYPTEDSERVER_HPP
