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

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QAbstractSocket>
#include <QObject>
#include <QSslError>


class Client: public QObject
{
    public:
        void connectoTo(const QString& host, quint16 port);

    private:
        void socketEncrypted();
        void socketStateChanged(QAbstractSocket::SocketState socketState);
        void socketSslErrors(const QList<QSslError> &errors);
};

#endif // CLIENT_HPP
