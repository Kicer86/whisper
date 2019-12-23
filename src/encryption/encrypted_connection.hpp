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

struct IEncryptionPrimitivesProvider;

/**
 * @brief representation of encrypted connection
 */

class EncryptedConnection: public QObject, public IEncryptedConnection
{
        Q_OBJECT

    public:
        EncryptedConnection(const IEncryptionPrimitivesProvider* ourKeys, const QString& host, quint16 port);
        EncryptedConnection(const IEncryptionPrimitivesProvider* ourKeys, QTcpSocket *);
        ~EncryptedConnection();

        const Botan::Public_Key* getTheirsPublicKey() const override;

    private:
        const IEncryptionPrimitivesProvider* m_ourKeys;
        std::unique_ptr<Botan::Public_Key> m_theirsPublicKey;
        std::vector<unsigned char> m_symmetricKeyPart;
        std::vector<unsigned char> m_symmetricKey;
        QTcpSocket* m_socket;

        struct not_enouth_data: std::exception {};
        struct unexpected_data: std::exception {};
        struct protocol_error: std::exception {};

        enum State
        {
            WaitForPublicKeyFromHost,
            WaitForSymmetricKeyFromHost,
            ConnectionEstablished,
            Ready,
        } m_state;

        EncryptedConnection(const IEncryptionPrimitivesProvider *, State);

        void connectToSocketSignals();

        void sendPublicKey();
        void sendSymmetricKey();
        void readTheirsPublicKey();
        void readSymmetricKey();
        QByteArray readDataWithSizeHeader();

        void socketStateChanged(QAbstractSocket::SocketState socketState);
        void socketError(QAbstractSocket::SocketError);
        void readyRead();
        void disconnected();
        void closeConnection() override;

    signals:
        void connectionEstablished(IEncryptedConnection *);
        void connectionClosed(IEncryptedConnection *);
};


#endif // ENCRYPTED_CONNECTION_HPP_INCLUDED
