
#ifndef IIDENTITY_CHECKER_HPP_INCLUDED
#define IIDENTITY_CHECKER_HPP_INCLUDED


class Identity;

struct IIdentityChecker
{
    virtual ~IIdentityChecker() = default;

    virtual bool isValid(const Identity &) const = 0;
};


#endif // IIDENTITY_CHECKER_HPP_INCLUDED
