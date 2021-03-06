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

#ifndef ENCRYPTEDCLIENT_HPP
#define ENCRYPTEDCLIENT_HPP

#include <memory>
#include <QString>
#include <botan/pk_keys.h>

#include "iencrypted_connection.hpp"
#include "connection_monitor.hpp"

class QTcpSocket;
struct IConnectionManager;
struct IEncryptionPrimitivesProvider;


/**
 * @brief tool for making connections
 */
class EncryptedClient
{
    public:
        EncryptedClient(const IEncryptionPrimitivesProvider* ourKeys, IConnectionManager &);

        void makeConnection(const QString& address, quint16 port);

    private:
        const IEncryptionPrimitivesProvider* m_ourKeys;
        ConnectionMonitor m_connectionMonitor;

        void sendPublicKey(QTcpSocket &);
};

#endif // ENCRYPTEDCLIENT_HPP
