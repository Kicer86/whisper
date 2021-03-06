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

#ifndef USERKEYSMANAGER_HPP
#define USERKEYSMANAGER_HPP

#include <QString>
#include <botan/pk_keys.h>

#include "encryption/iencryption_primitives_provider.hpp"

/**
 * @brief Class for managing user's cryptographic keys
 */
class UserKeysManager: public IEncryptionPrimitivesProvider
{
    public:
        UserKeysManager(const QString& keys_dir);
        ~UserKeysManager();

        bool privateKeyExists() const;
        bool publicKeyExists() const;
        bool generateKeysPair() const;

        std::unique_ptr<Botan::Public_Key> ourPublicKey() const override;
        std::unique_ptr<Botan::Private_Key> ourPrivateKey() const override;
        Botan::RandomNumberGenerator& randomGenerator() const override;

    private:
        const std::unique_ptr<Botan::RandomNumberGenerator> m_randomGenerator;
        const QString m_keysDir;

        QString privateKeyPath() const;
        QString publicKeyPath() const;
};

#endif // USERKEYSMANAGER_HPP
