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

#include <QTcpSocket>

/**
 * @brief representation of encrypted connection
 */

class EncryptedConnection: public QObject, public IEncryptedConnection
{
        Q_OBJECT

    public:
        EncryptedConnection(const Botan::Public_Key* oursPublicKey, const QString& host, quint16 port);
        EncryptedConnection(const Botan::Public_Key* oursPublicKey, QTcpSocket *);

        const Botan::Public_Key* getTheirsPublicKey() const override;

    private:
        const Botan::Public_Key* m_oursPublicKey;
        std::unique_ptr<Botan::Public_Key> m_theirsPublicKey;
        static constexpr int m_symmetricKeySize = 32;
        std::array<unsigned char, m_symmetricKeySize> m_symmetricKey;
        QTcpSocket* m_socket;

        struct not_enouth_data: std::exception {};
        struct unexpected_data: std::exception {};

        enum State
        {
            WaitForConnectionValidation,
            ValidateIncomingConnection,
            ConnectionEstablished,
        } m_state;

        void connectToSocketSignals();

        void sendPublicKey();
        void sendSymmetricKey();
        void readTheirsPublicKey();

        void socketStateChanged(QAbstractSocket::SocketState socketState);
        void socketError(QAbstractSocket::SocketError);
        void readyRead();

    signals:
        void gotTheirsPublicKey(IEncryptedConnection *);
};


#endif // ENCRYPTED_CONNECTION_HPP_INCLUDED
