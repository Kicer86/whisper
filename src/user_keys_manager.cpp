/*
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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

#include "user_keys_manager.hpp"

UserKeysManager::UserKeysManager(const QString& keys_dir)
    : m_keysDir(keys_dir)
{

}


bool UserKeysManager::privateKeyExists() const
{
    return false;
}


bool UserKeysManager::publicKeyExists() const
{
    return false;
}


void UserKeysManager::generateKeysPair() const
{

}
