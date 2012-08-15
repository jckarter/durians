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
#include <utility>

#define _S_CAT(a, b) a##b
#define S_CAT(a, b) _S_CAT(a,b)

#ifdef NDEBUG
#define S_UNREACHABLE assert(false);
#else
#define S_UNREACHABLE ::std::abort();
#endif

#define S_AUTO(...) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }

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
        struct _index_of_type;
    
        template<size_t N, typename T, typename...UU>
        struct _index_of_type<N, T, T, UU...> : std::integral_constant<size_t, N> {};
        
        template<size_t N, typename T, typename U, typename...UU>
        struct _index_of_type<N, T, U, UU...> : _index_of_type<N+1, T, UU...> {};

        template<size_t N, typename...TT>
        struct _type_at;
        template<typename T, typename...TT>
        struct _type_at<0, T, TT...> {
            using type = T;
        };
        template<size_t N, typename T, typename...TT>
        struct _type_at<N, T, TT...> : _type_at<N-1, TT...> {};
    }
    
    template<typename T, typename...UU>
    using index_of_type = internal::_index_of_type<0, T, UU...>;

    template<size_t N, typename...TT>
    using type_at = typename internal::_type_at<N, TT...>::type;
    
    namespace internal {
        template<size_t N, size_t...NN>
        struct _integers : _integers<N-1, N-1, NN...> { };
        
        template<size_t...NN>
        struct _integers<0, NN...> { using values = values<size_t, NN...>; };
    }
    
    template<size_t N>
    using integers = typename internal::_integers<N>::values;
}

#endif
