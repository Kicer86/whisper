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

#include "ssl_server.hpp"

#include <iostream>
#include <memory>
#include <QSslSocket>


void SslServer::incomingConnection(qintptr socketDescriptor)
{
    std::unique_ptr<QSslSocket> serverSocket = std::make_unique<QSslSocket>();

    if (serverSocket->setSocketDescriptor(socketDescriptor))
    {
        QSslSocket* sslSocket = serverSocket.release();

        addPendingConnection(sslSocket);
        connect(sslSocket, &QSslSocket::encrypted, this, &SslServer::socketEncrypted);
        connect(sslSocket, &QSslSocket::stateChanged, this, &SslServer::socketStateChanged);
        connect(sslSocket, qOverload<const QList<QSslError> &>(&QSslSocket::sslErrors), this, &SslServer::socketSslErrors);
        connect(sslSocket, qOverload<QAbstractSocket::SocketError>(&QSslSocket::error), this, &SslServer::socketError);
        sslSocket->startServerEncryption();
    }
}


void SslServer::socketEncrypted()
{
    std::cout << "client socket encrypted\n";
}


void SslServer::socketStateChanged(QAbstractSocket::SocketState socketState)
{
    std::cout << "client socket state changed to " << socketState << "\n";
}


void SslServer::socketSslErrors(const QList<QSslError>& errors)
{
    for(const QSslError& error: errors)
        std::cout << "client socket error: " << error.errorString().toStdString() << "\n";
}


void SslServer::socketError(QAbstractSocket::SocketError error)
{
    std::cout << "client socket error " << error << "\n";
}
