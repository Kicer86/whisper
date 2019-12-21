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

#ifndef USERMANAGER_HPP
#define USERMANAGER_HPP

#include "iuser_manager.hpp"

struct IConfiguration;

class UserManager final: public IUserManager
{
    public:
        UserManager(IConfiguration &);
        ~UserManager();

        QVector<UserId> listUsers() const override;
        QString name(const UserId &) const override;
        QString address(const UserId &) const override;
        QByteArray publicKey(const UserId &) const override;

    private:
        IConfiguration& m_config;

        void load();
};

#endif // USERMANAGER_HPP
