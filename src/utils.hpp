
#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED


namespace utils
{
    // https://stackoverflow.com/a/18940595/1749713
    template<class T>
    struct pointer_comp
    {
        typedef std::true_type is_transparent;
        // helper does some magic in order to reduce the number of
        // pairs of types we need to know how to compare: it turns
        // everything into a pointer, and then uses `std::less<T*>`
        // to do the comparison:
        struct helper {
            T* ptr;
            helper():ptr(nullptr) {}
            helper(helper const &) = default;
            helper(T* p):ptr(p) {}

            template<class U, class...Ts>
            helper( std::shared_ptr<U,Ts...> const& sp ):ptr(sp.get()) {}

            template<class U, class...Ts>
            helper( std::unique_ptr<U, Ts...> const& up ):ptr(up.get()) {}

            // && optional: enforces rvalue use only
            bool operator<( helper o ) const {
                return std::less<T*>()( ptr, o.ptr );
            }
        };
        // without helper, we would need 2^n different overloads, where
        // n is the number of types we want to support (so, 8 with
        // raw pointers, unique pointers, and shared pointers).  That
        // seems silly:
        // && helps enforce rvalue use only
        bool operator()( helper const&& lhs, helper const&& rhs ) const {
            return lhs < rhs;
        }
    };

    template<typename T, typename F>
    constexpr T& binary_cast(F& from)
    {
        static_assert(sizeof(F) == sizeof(T), "type sizes need to be equal");

        T* to = reinterpret_cast<T *>(&from);

        return *to;
    }
}

#endif // UTILS_HPP_INCLUDED
