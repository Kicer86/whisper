/*
 * Class for managing users
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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

#include "user_manager.hpp"

#include <QStringList>
#include <QVariant>
#include <OpenLibrary/utils_qt/iconfiguration.hpp>


namespace
{
    const QString users_config_node("users");
}


UserManager::User::User(const QString& n, const QString& h, quint16 hp, const QByteArray& p)
    : name(n)
    , host(h)
    , pkey(p)
    , port(hp)
{
}


UserManager::UserManager(IConfiguration& config)
    : m_config(config)
{
    load();
}


UserManager::~UserManager()
{

}


QVector<UserId> UserManager::listUsers() const
{
    QVector<UserId> result;

    for(auto it = m_users.cbegin(); it != m_users.cend(); ++it)
        result.append(it->first);

    return result;
}


QString UserManager::name(const UserId& id) const
{
    auto it = m_users.find(id);
    return it == m_users.cend()? QString(): it->second.name;
}


std::pair<QString, quint16> UserManager::address(const UserId& id) const
{
    auto it = m_users.find(id);
    return it == m_users.cend()? std::pair<QString, quint16>(): std::make_pair(it->second.host, it->second.port);
}


QByteArray UserManager::publicKey(const UserId& id) const
{
    auto it = m_users.find(id);
    return it == m_users.cend()? QByteArray(): it->second.pkey;
}


void UserManager::load()
{
    const QStringList users = m_config.getSubEntries(users_config_node);

    for (const QString& user: users)
    {
        const QString userName = m_config.getEntry(QString("%1::%2::name").arg(users_config_node).arg(user)).toString();
        const QByteArray publicKey = m_config.getEntry(QString("%1::%2::pkey").arg(users_config_node).arg(user)).toByteArray();
        const QString host = m_config.getEntry(QString("%1::%2::host").arg(users_config_node).arg(user)).toString();
        const int port = m_config.getEntry(QString("%1::%2::port").arg(users_config_node).arg(user)).toInt();

        m_users.emplace(m_userNextId++, User(userName, host, static_cast<quint16>(port), publicKey));
    }
}
