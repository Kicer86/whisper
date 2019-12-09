
#ifndef IENCRYPTION_PRIMITIVES_PROVIDER_HPP_INCLUDED
#define IENCRYPTION_PRIMITIVES_PROVIDER_HPP_INCLUDED


struct IEncryptionPrimitivesProvider
{
    virtual ~IEncryptionPrimitivesProvider() = default;
    virtual std::unique_ptr<Botan::Public_Key> ourPublicKey() const = 0;
    virtual std::unique_ptr<Botan::Private_Key> ourPrivateKey() const = 0;
    virtual Botan::RandomNumberGenerator& randomGenerator() const = 0;
};

#endif // IENCRYPTION_PRIMITIVES_PROVIDER_HPP_INCLUDED
