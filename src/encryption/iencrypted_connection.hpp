
#ifndef IENCRYPTED_CONNECTION_HPP_INCLUDED
#define IENCRYPTED_CONNECTION_HPP_INCLUDED

#include <QIODevice>
#include <botan/pk_keys.h>


struct IEncryptedConnection: QIODevice
{
    virtual ~IEncryptedConnection() = default;

    virtual const Botan::Public_Key* getTheirsPublicKey() const = 0;
};


#endif // IENCRYPTED_CONNECTION_HPP_INCLUDED
