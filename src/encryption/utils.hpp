
#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

namespace utils
{
    template<typename T, typename F>
    constexpr T& binary_cast(F& from)
    {
        static_assert(sizeof(F) == sizeof(T), "type sizes need to be equal");

        T* to = reinterpret_cast<T *>(&from);

        return *to;
    }

}


#endif // UTILS_HPP_INCLUDED
