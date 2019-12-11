/*
 * Server - low level utility for listening
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

#ifndef SERVER_HPP
#define SERVER_HPP

#include <QObject>
#include <botan/pk_keys.h>

#include "encryption/encrypted_server.hpp"
#include "identity_checker.hpp"

struct IEncryptionPrimitivesProvider;


class Server final: public QObject
{
        Q_OBJECT

    public:
        Server(const IEncryptionPrimitivesProvider* ourKeys, IConnectionManager& connection_manager, quint16 port, QObject * = nullptr);
        ~Server();

        void start();

    private:
        IdentityChecker m_identityChecker;
        EncryptedServer m_server;
        quint16 m_port;
};

#endif // SERVER_HPP
