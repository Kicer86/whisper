
#ifndef IENCRYPTED_CONNECTION_HPP_INCLUDED
#define IENCRYPTED_CONNECTION_HPP_INCLUDED

#include <QIODevice>
#include <botan/pk_keys.h>


class IEncryptedConnection: public QIODevice
{
    Q_OBJECT

public:
    virtual ~IEncryptedConnection() = default;

    virtual const Botan::Public_Key* getTheirsPublicKey() const = 0;

signals:
    void protocolCriticalError();
    void connectionClosed();
};


#endif // IENCRYPTED_CONNECTION_HPP_INCLUDED
