
#ifndef IKEYS_PROVIDER_HPP_INCLUDED
#define IKEYS_PROVIDER_HPP_INCLUDED


struct IKeysProvider
{
    virtual ~IKeysProvider() = default;
    virtual std::unique_ptr<Botan::Public_Key> ourPublicKey() const = 0;
    virtual std::unique_ptr<Botan::Private_Key> ourPrivateKey() const = 0;
    virtual Botan::RandomNumberGenerator& randomGenerator() const = 0;
};


#endif // IKEYS_PROVIDER_HPP_INCLUDED
