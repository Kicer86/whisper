/*
 * Client - low level utility for establishing connections with others.
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

#include "client.hpp"

#include <iostream>
#include <memory>

#include <QSslSocket>


void Client::connectoTo(const QString& host, quint16 port)
{
    QSslSocket* clientSocket = new QSslSocket;

    clientSocket->connectToHostEncrypted(host, port);

    connect(clientSocket, &QSslSocket::encrypted, this, &Client::socketEncrypted);
    connect(clientSocket, &QSslSocket::stateChanged, this, &Client::socketStateChanged);
    connect(clientSocket, qOverload<const QList<QSslError> &>(&QSslSocket::sslErrors), this, &Client::socketSslErrors);
}


void Client::socketEncrypted()
{
    std::cout << "client socket encrypted\n";
}


void Client::socketStateChanged(QAbstractSocket::SocketState socketState)
{
    std::cout << "client socket state changed to " << socketState << "\n";
}


void Client::socketSslErrors(const QList<QSslError>& errors)
{
    for(const QSslError& error: errors)
        std::cout << "client socket error: " << error.errorString().toStdString() << "\n";
}
