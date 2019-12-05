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
#include <QTcpServer>

struct IConnectionManager;
struct IIdentityChecker;
struct IEncryptedConnection;

/**
 * @brief Server providing encrypted connections
 */
class EncryptedServer final: public QTcpServer
{
    public:
        EncryptedServer(const IIdentityChecker &, IConnectionManager &);
        ~EncryptedServer();

    private:
        const IIdentityChecker& m_identityChecker;
        IConnectionManager& m_connectionManager;
        std::vector<std::unique_ptr<IEncryptedConnection>> m_waitingForApproval;

        void newConnection();
        void validateTheirsPublicKey(IEncryptedConnection *);
};

#endif // ENCRYPTEDSERVER_HPP
