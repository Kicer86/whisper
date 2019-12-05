
#ifndef IENCRYPTED_CONNECTION_HPP_INCLUDED
#define IENCRYPTED_CONNECTION_HPP_INCLUDED

#include <QSslKey>


struct IEncryptedConnection
{
    virtual ~IEncryptedConnection() = default;

    virtual QSslKey getTheirsPublicKey() const = 0;
};


#endif // IENCRYPTED_CONNECTION_HPP_INCLUDED
