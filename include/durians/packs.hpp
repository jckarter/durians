//
//  packs.hpp
//  durians
//
//  Created by Joe Groff on 8/16/12.
//  Copyright (c) 2012 Durian Software. All rights reserved.
//

#ifndef durians_packs_hpp
#define durians_packs_hpp

#include <type_traits>

namespace durians {
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
    
    template<typename T>
    struct end_generate_values { using type = T; };
    struct end_generate_types {};
    
    namespace internal {
        template<typename Generator, typename Generator::type...vv>
        struct _generate_values : _generate_values<typename Generator::next,
                                                   vv...,
                                                   Generator::value>
        {};
        
        template<typename T, T...vv>
        struct _generate_values<end_generate_values<T>, vv...>
        {
            using values = values<T, vv...>;
        };
        
        template<typename Generator, typename...TT>
        struct _generate_types : _generate_types<typename Generator::next,
                                                 TT...,
                                                 typename Generator::type>
        {};
        
        template<typename...TT>
        struct _generate_types<end_generate_types, TT...>
        {
            using types = types<TT...>;
        };
    }
    
    template<typename T>
    using generate_values = typename internal::_generate_values<T>::values;
    template<typename T>
    using generate_types = typename internal::_generate_types<T>::types;
}

#endif
