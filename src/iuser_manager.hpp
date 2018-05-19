
#include <QString>
#include <QVector>

typedef int UserId;

struct IUserManager
{
    virtual ~IUserManager() = default;

    QVector<UserId> listUsers() const;
    QString name(const UserId &) const;
    QString address(const UserId &) const;
};
