
#ifndef IENCRYPTED_CONNECTION_HPP_INCLUDED
#define IENCRYPTED_CONNECTION_HPP_INCLUDED

#include <QIODevice>


struct IEncryptedConnection: QIODevice
{
    virtual ~IEncryptedConnection() = default;
};


#endif // IENCRYPTED_CONNECTION_HPP_INCLUDED
