
#include <QString>
#include <QVector>

typedef int UserId;

struct IUserManager
{
    virtual ~IUserManager() = default;

    virtual QVector<UserId> listUsers() const = 0;
    virtual QString name(const UserId &) const = 0;
    virtual std::pair<QString, quint16> address(const UserId &) const = 0;
    virtual QByteArray publicKey(const UserId &) const = 0;
};
