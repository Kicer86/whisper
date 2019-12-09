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

#include <memory>
#include <botan/pkcs8.h>
#include <botan/rsa.h>
#include <botan/system_rng.h>
#include <botan/auto_rng.h>
#include <botan/x509_key.h>
#include <QDir>
#include <QFile>

// https://stackoverflow.com/a/13635912/1749713  related knowledge

namespace
{
    constexpr int bits = 4096;

    void check_ssl_status(int status)
    {
        if (status != 1)
            throw std::runtime_error("Botan error");
    }

    std::unique_ptr<Botan::RandomNumberGenerator> random_number_generator()
    {
        std::unique_ptr<Botan::RandomNumberGenerator> rng;
        #if defined(BOTAN_HAS_SYSTEM_RNG)
        rng.reset(new Botan::System_RNG);
        #else
        rng.reset(new Botan::AutoSeeded_RNG);
        #endif

        return rng;
    }
}


UserKeysManager::UserKeysManager(const QString& keys_dir)
    : m_randomGenerator(random_number_generator())
    , m_keysDir(keys_dir)
{

}


UserKeysManager::~UserKeysManager()
{
}


bool UserKeysManager::privateKeyExists() const
{
    const bool exists = QFile::exists(privateKeyPath());

    return exists;
}


bool UserKeysManager::publicKeyExists() const
{
    const bool exists = QFile::exists(publicKeyPath());

    return exists;
}


bool UserKeysManager::generateKeysPair() const
{
    bool status = true;

    try
    {
        QDir().mkpath(m_keysDir);

        const QString private_key_path = privateKeyPath();
        const QString public_key_path = publicKeyPath();

        Botan::RandomNumberGenerator& rng = randomGenerator();
        Botan::RSA_PrivateKey key_pair(rng, bits);

        std::string public_key = Botan::X509::PEM_encode(key_pair);
        std::string private_key = Botan::PKCS8::PEM_encode(key_pair);

        QFile public_key_file(publicKeyPath());
        public_key_file.open(QFile::WriteOnly);
        public_key_file.write(public_key.c_str());

        QFile private_key_file(privateKeyPath());
        private_key_file.open(QFile::WriteOnly);
        private_key_file.write(private_key.c_str());
    }
    catch(const std::runtime_error &)
    {
        status = false;
    }

    return status;
}


std::unique_ptr<Botan::Public_Key> UserKeysManager::ourPublicKey() const
{
    std::unique_ptr<Botan::Public_Key> public_key( Botan::X509::load_key(publicKeyPath().toStdString()) );

    return public_key;
}


std::unique_ptr<Botan::Private_Key> UserKeysManager::ourPrivateKey() const
{
    Botan::RandomNumberGenerator& rng = randomGenerator();
    std::unique_ptr<Botan::Private_Key> private_key( Botan::PKCS8::load_key(privateKeyPath().toStdString(), rng) );

    return private_key;
}


Botan::RandomNumberGenerator& UserKeysManager::randomGenerator() const
{
    return *m_randomGenerator.get();
}



QString UserKeysManager::publicKeyPath() const
{
    return QString("%1/%2").arg(m_keysDir).arg("rsa-public.pem");
}


QString UserKeysManager::privateKeyPath() const
{
    return QString("%1/%2").arg(m_keysDir).arg("rsa-private.pem");
}

