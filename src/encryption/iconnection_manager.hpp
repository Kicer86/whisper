
#ifndef ICONNECTION_MANAGER_HPP_INCLUDED
#define ICONNECTION_MANAGER_HPP_INCLUDED

#include <memory>

struct IEncryptedConnection;


struct IConnectionManager
{
    virtual ~IConnectionManager() = default;

    virtual void add(std::unique_ptr<IEncryptedConnection>) = 0;
};


#endif // ICONNECTION_MANAGER_HPP_INCLUDED
