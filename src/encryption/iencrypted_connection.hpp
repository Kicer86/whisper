
#ifndef IENCRYPTED_CONNECTION_HPP_INCLUDED
#define IENCRYPTED_CONNECTION_HPP_INCLUDED

#include <QSslKey>
#include <botan/pk_keys.h>


struct IEncryptedConnection
{
    virtual ~IEncryptedConnection() = default;

    virtual const Botan::Public_Key* getTheirsPublicKey() const = 0;
    virtual void closeConnection() = 0;
};


#endif // IENCRYPTED_CONNECTION_HPP_INCLUDED
