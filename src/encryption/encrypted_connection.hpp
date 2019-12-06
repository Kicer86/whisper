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


#ifndef ENCRYPTED_CONNECTION_HPP_INCLUDED
#define ENCRYPTED_CONNECTION_HPP_INCLUDED

#include "iencrypted_connection.hpp"

#include <memory>
#include <QTcpSocket>

/**
 * @brief representation of encrypted connection
 */

class EncryptedConnection: public QObject, public IEncryptedConnection
{
        Q_OBJECT

    public:
        EncryptedConnection(const QSslKey& oursPublicKey, const QString& host, quint16 port);
        EncryptedConnection(const QSslKey& oursPublicKey, QTcpSocket *);

        QSslKey getTheirsPublicKey() const override;

    private:
        QSslKey m_oursPublicKey;
        QSslKey m_theirsPublicKey;
        std::unique_ptr<QTcpSocket> m_socket;

        enum State
        {
            WaitForTheirsPublicKey,
            ConnectionEstablished,
        } m_state;

        void connectToSocketSignals();
        void sendPublicKey();

        void socketStateChanged(QAbstractSocket::SocketState socketState);
        void socketError(QAbstractSocket::SocketError);
        void readyRead();

    signals:
        void gotTheirsPublicKey(IEncryptedConnection *);
};


#endif // ENCRYPTED_CONNECTION_HPP_INCLUDED
