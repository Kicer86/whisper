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
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/bn.h>
#include <QDir>

// https://stackoverflow.com/a/13635912/1749713  related knowledge

namespace
{
    constexpr int bits = 4096;

    using BN_ptr = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;
    using RSA_ptr = std::unique_ptr<RSA, decltype(&::RSA_free)>;
    using EVP_KEY_ptr = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
    using BIO_FILE_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;

    void check_ssl_status(int status)
    {
        if (status != 1)
            throw std::runtime_error("OpenSSL error");
    }
}


UserKeysManager::UserKeysManager(const QString& keys_dir)
    : m_keysDir(keys_dir)
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

        BN_ptr bn(BN_new(), ::BN_free);
        check_ssl_status( BN_set_word(bn.get(), RSA_F4) );

        RSA_ptr rsa(RSA_new(), ::RSA_free);
        check_ssl_status( RSA_generate_key_ex(rsa.get(), bits, bn.get(), nullptr) );

        BIO_FILE_ptr pem_pub(BIO_new_file(public_key_path.toStdString().c_str(), "w"), ::BIO_free);
        check_ssl_status( PEM_write_bio_RSA_PUBKEY(pem_pub.get(), rsa.get()) );

        BIO_FILE_ptr pem_prv(BIO_new_file(private_key_path.toStdString().c_str(), "w"), ::BIO_free);
        check_ssl_status( PEM_write_bio_RSAPrivateKey(pem_prv.get(), rsa.get(), NULL, NULL, 0, NULL, NULL) );
    }
    catch(const std::runtime_error &)
    {
        status = false;
    }

    return status;
}


QSslKey UserKeysManager::ourPublicKey() const
{
    QFile publicKeyFile(publicKeyPath());
    publicKeyFile.open(QFile::ReadOnly);

    QSslKey key(&publicKeyFile, QSsl::Rsa, QSsl::Pem, QSsl::PublicKey);
    assert(key.isNull() == false);

    return key;
}


QString UserKeysManager::publicKeyPath() const
{
    return QString("%1/%2").arg(m_keysDir).arg("rsa-public.pem");
}


QString UserKeysManager::privateKeyPath() const
{
    return QString("%1/%2").arg(m_keysDir).arg("rsa-private.pem");
}

