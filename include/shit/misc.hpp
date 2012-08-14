//
//  misc.hpp
//  shit
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef shit_misc_hpp
#define shit_misc_hpp

#include <type_traits>

#define _S_CAT(a, b) a##b
#define S_CAT(a, b) _S_CAT(a,b)

#ifdef NDEBUG
#define S_UNREACHABLE assert(false);
#else
#define S_UNREACHABLE ::std::abort();
#endif

namespace shit {
    template<typename T>
    using type = T;
    
    template<typename T, T f>
    struct static_function;
    
    template<typename R, typename...AA, R f(AA...)>
    struct static_function<R(AA...), f>
    {
        R operator()(AA...args) const
        {
            return f(static_cast<AA>(args)...);
        }
    };
    
    template<typename...TT>
    struct types {};
    
    template<typename T, T...NN>
    struct values {};
    
    template<typename T, typename...UU>
    struct is_type_in;
    template<typename T, typename U, typename...UU>
    struct is_type_in<T, U, UU...> : is_type_in<T, UU...> {};
    template<typename T, typename...UU>
    struct is_type_in<T, T, UU...> : std::integral_constant<bool, true> {};
    template<typename T>
    struct is_type_in<T> : std::integral_constant<bool, false> {};

    template<template<typename> class Trait, typename...TT>
    struct is_any_type;
    template<template<typename> class Trait>
    struct is_any_type<Trait> : std::integral_constant<bool, false> {};
    template<template<typename> class Trait, typename T, typename...TT>
    struct is_any_type<Trait, T, TT...>
    : std::integral_constant<bool, Trait<T>::value || is_any_type<Trait, TT...>::value> {};

    template<template<typename> class Trait, typename...TT>
    struct is_every_type;
    template<template<typename> class Trait>
    struct is_every_type<Trait> : std::integral_constant<bool, true> {};
    template<template<typename> class Trait, typename T, typename...TT>
    struct is_every_type<Trait, T, TT...>
    : std::integral_constant<bool, Trait<T>::value && is_every_type<Trait, TT...>::value> {};

    namespace internal {
        template<size_t N, typename T, typename...UU>
        struct type_index;
    
        template<size_t N, typename T, typename...UU>
        struct type_index<N, T, T, UU...> : std::integral_constant<size_t, N> {};
        
        template<size_t N, typename T, typename U, typename...UU>
        struct type_index<N, T, U, UU...> : type_index<N+1, T, UU...> {};
    }
    
    template<typename T, typename...UU>
    using type_index = internal::type_index<0, T, UU...>;
    
    namespace internal {
        template<size_t N, size_t...NN>
        struct integers : integers<N-1, N-1, NN...> { };
        
        template<size_t...NN>
        struct integers<0, NN...> { using values = values<size_t, NN...>; };
    }
    
    template<size_t N>
    using integers = typename internal::integers<N>::values;
}

#endif
