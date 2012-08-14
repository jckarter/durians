//
//  print.hpp
//  shit
//
//  Created by Joe Groff on 8/14/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef shit_print_hpp
#define shit_print_hpp

#include <cstdio>
#include <functional>
#include <type_traits>
#include <shit/misc.hpp>

namespace shit {
    template<char...c>
    struct string_constant {
        static const char value[sizeof...(c)+1];
    };
    
    template<char...c>
    const char string_constant<c...>::value[sizeof...(c)+1] = {c..., '\0'};
    
    template<char...c>
    struct chars {};
    
    template<typename...T>
    struct concat_chars;
    template<char...c, char...d, typename...T>
    struct concat_chars<chars<c...>, chars<d...>, T...>
    : concat_chars<chars<c..., d...>, T...>
    {};
    template<char...c>
    struct concat_chars<chars<c...>> : string_constant<c...> {};
    template<>
    struct concat_chars<> : string_constant<> {};
    
    template<typename FormatChars, typename...FormatArgs>
    struct basic_print_traits {
        using format_chars = FormatChars;
        static constexpr std::size_t format_arg_count = sizeof...(FormatArgs);
        
        template<std::size_t N> struct format_arg;
    };
    
    template<typename FC, typename...FA>
    template<std::size_t N>
    struct basic_print_traits<FC, FA...>::format_arg {
        template<typename T>
        static auto get(T &&x)
        -> decltype(type_at<N, FA...>()(std::forward<T>(x)))
        {
            return type_at<N, FA...>()(std::forward<T>(x));
        }
    };
    
    inline constexpr size_t sum() { return 0; }
    inline constexpr size_t sum(size_t a) { return a; }
    template<typename T, typename...TT>
    inline constexpr size_t sum(T a, T b, TT...c)
    {
        return sum(a + b, c...);
    }
    
    struct identity {
        template<typename T>
        T operator()(T &&x) { return x; }
    };
    
    template<typename T, typename If = void>
    struct print_traits;

    template<>
    struct print_traits<char>
    : basic_print_traits<chars<'%', 'c'>, identity> {};
    
    template<>
    struct print_traits<short>
    : basic_print_traits<chars<'%', 'd'>, identity> {};
    
    template<>
    struct print_traits<int>
    : basic_print_traits<chars<'%', 'd'>, identity> {};
    
    template<>
    struct print_traits<long>
    : basic_print_traits<chars<'%', 'l', 'd'>, identity> {};
    
    template<>
    struct print_traits<long long>
    : basic_print_traits<chars<'%', 'l', 'l', 'd'>, identity> {};

    template<typename T>
    struct print_traits<T *>
    : basic_print_traits<chars<'%', 'p'>, identity> {};
    
    template<>
    struct print_traits<char const *>
    : basic_print_traits<chars<'%', 's'>, identity> {};
    
    template<>
    struct print_traits<char *>
    : basic_print_traits<chars<'%', 's'>, identity> {};
    
    template<char...c>
    struct print_traits<chars<c...>>
    : basic_print_traits<chars<c...>> {};
    
    template<typename T>
    struct print_traits<T&> : print_traits<T> {};
    template<typename T>
    struct print_traits<T&&> : print_traits<T> {};
    template<typename T>
    struct print_traits<T const> : print_traits<T> {};
    template<typename T>
    struct print_traits<T volatile> : print_traits<T> {};
    
    template<typename...T>
    using format_string = concat_chars<typename print_traits<T>::format_chars...>;
    
    template<size_t N, typename If, typename...T>
    struct format_arg;
    
    template<size_t N, typename T, typename...TT>
    struct format_arg<N, typename std::enable_if<(N < print_traits<T>::format_arg_count)>::type, T, TT...>
    {
        template<typename U, typename...UU>
        static auto get(U &&arg, UU &&...)
        -> decltype(print_traits<T>::template format_arg<N>::get(std::forward<U>(arg)))
        {
            return print_traits<T>::template format_arg<N>::get(std::forward<U>(arg));
        }
    };
    
    template<size_t N, typename T, typename...TT>
    struct format_arg<N, typename std::enable_if<(N >= print_traits<T>::format_arg_count)>::type, T, TT...>
    {
        template<typename U, typename...UU>
        static auto get(U &&, UU &&...args)
        -> decltype(format_arg<N - print_traits<T>::format_arg_count, void, TT...>::get(std::forward<UU>(args)...))
        {
            return format_arg<N - print_traits<T>::format_arg_count, void, TT...>::get(std::forward<UU>(args)...);
        }
    };
    
    template<size_t...N, typename...T>
    inline void _print(values<size_t, N...>, T &&...args)
    {
        ::std::printf(format_string<T...>::value, format_arg<N, void, T...>::get(std::forward<T>(args)...)...);
    }
    
    template<typename...T>
    inline void print(T &&...args)
    {
        _print(integers<sum(print_traits<T>::format_arg_count...)>(), std::forward<T>(args)...);
    }
    
    namespace delim {
        constexpr chars<' '> space = {};
        constexpr chars<'\n'> nl = {};
        constexpr chars<'\t'> tab = {};
        constexpr chars<','> comma = {};
    }
    
    template<typename...T>
    inline void println(T &&...args)
    {
        print(std::forward<T>(args)..., delim::nl);
    }
    
    template<size_t N>
    struct string_constant2
    {
        char value[N];
    };
    
    template<size_t N, size_t M>
    constexpr string_constant2<N+M-1> concat(char const (&a)[N], char const (&b)[M])
    {
        return concat(a, b, integers<N-1>(), integers<M-1>());
    }
    
    template<size_t N, size_t M, size_t...NN, size_t...MM>
    constexpr string_constant2<N+M-1> concat(char const (&a)[N], char const (&b)[M],
                                       values<size_t, NN...>, values<size_t, MM...>)
    {
        return {{a[NN]..., b[MM]..., '\0'}};
    }
    
    static_assert(concat("abc", "def").value[0] == 'a', "");
    static_assert(concat("abc", "def").value[1] == 'b', "");
    static_assert(concat("abc", "def").value[2] == 'c', "");
    static_assert(concat("abc", "def").value[3] == 'd', "");
    static_assert(concat("abc", "def").value[4] == 'e', "");
    static_assert(concat("abc", "def").value[5] == 'f', "");
    static_assert(concat("abc", "def").value[6] == '\0', "");
}

#endif
