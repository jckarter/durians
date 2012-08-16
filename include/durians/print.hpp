//
//  print.hpp
//  durians
//
//  Created by Joe Groff on 8/14/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef durians_print_hpp
#define durians_print_hpp

#include <array>
#include <limits>
#include <cstdio>
#include <string>
#include <tuple>
#include <type_traits>
#include <durians/enum_meta.hpp>
#include <durians/misc.hpp>
#include <durians/struct_meta.hpp>

namespace durians {
    // TODO:
    // - need to support print_traits that construct a string to hold printed representation
    // - need to support types that can't be printed using formatting strings (e.g.containers)
    // - genericize print target concept?
    template<char...c>
    struct string_constant {
        static const char value[sizeof...(c)+1];
    };
    
    template<char...c>
    constexpr char string_constant<c...>::value[sizeof...(c)+1] = {c..., '\0'};
        
    template<typename...T>
    struct concat_string_constant;
    template<char...c, char...d, typename...T>
    struct concat_string_constant<string_constant<c...>, string_constant<d...>, T...>
    : concat_string_constant<string_constant<c..., d...>, T...>
    {};
    template<char...c>
    struct concat_string_constant<string_constant<c...>> : string_constant<c...> {
        using type = string_constant<c...>;
    };
    template<>
    struct concat_string_constant<> : string_constant<> {
        using type = string_constant<>;
    };

    template<typename Sep, typename...T>
    struct join_string_constant;
    template<char...sep, char...c, char...d, typename...T>
    struct join_string_constant<string_constant<sep...>, string_constant<c...>, string_constant<d...>, T...>
    : join_string_constant<string_constant<sep...>, string_constant<c..., sep..., d...>, T...>
    {};
    template<char...sep, char...c>
    struct join_string_constant<string_constant<sep...>, string_constant<c...>>
    : string_constant<c...> {
        using type = string_constant<c...>;
    };
    template<char...sep>
    struct join_string_constant<string_constant<sep...>> : string_constant<> {
        using type = string_constant<>;
    };
    
    namespace internal {
        template<char c>
        struct escaped_char { using type = string_constant<c>; };
        
        template<>
        struct escaped_char<'%'> { using type = string_constant<'%', '%'>; };
        
        template<char...c>
        using escape_string_constant = concat_string_constant<typename escaped_char<c>::type...>;
    }
    
    template<typename T, typename If = void> struct print_traits;

    template<size_t> struct format_arg_t {};
    
    template<> struct print_traits<char> {
        static constexpr char format_string[] = "%c";
        static constexpr size_t format_arg_count = 1;
        static int format_arg(signed char x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<signed char> {
        static constexpr char format_string[] = "%d";
        static constexpr size_t format_arg_count = 1;
        static int format_arg(signed char x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<short> {
        static constexpr char format_string[] = "%d";
        static constexpr size_t format_arg_count = 1;
        static int format_arg(short x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<int> {
        static constexpr char format_string[] = "%d";
        static constexpr size_t format_arg_count = 1;
        static int format_arg(int x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<long> {
        static constexpr char format_string[] = "%ld";
        static constexpr size_t format_arg_count = 1;
        static long format_arg(long x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<long long> {
        static constexpr char format_string[] = "%lld";
        static constexpr size_t format_arg_count = 1;
        static long long format_arg(long long x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<unsigned char> {
        static constexpr char format_string[] = "%u";
        static constexpr size_t format_arg_count = 1;
        static unsigned format_arg(unsigned char x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<unsigned short> {
        static constexpr char format_string[] = "%u";
        static constexpr size_t format_arg_count = 1;
        static unsigned format_arg(unsigned short x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<unsigned> {
        static constexpr char format_string[] = "%u";
        static constexpr size_t format_arg_count = 1;
        static unsigned format_arg(unsigned x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<unsigned long> {
        static constexpr char format_string[] = "%lu";
        static constexpr size_t format_arg_count = 1;
        static unsigned long format_arg(unsigned long x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<unsigned long long> {
        static constexpr char format_string[] = "%llu";
        static constexpr size_t format_arg_count = 1;
        static unsigned long long format_arg(unsigned long long x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<float> {
        static constexpr char format_string[] = "%g";
        static constexpr size_t format_arg_count = 1;
        static double format_arg(float x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<double> {
        static constexpr char format_string[] = "%g";
        static constexpr size_t format_arg_count = 1;
        static double format_arg(double x, format_arg_t<0>) { return x; }
    };
    template<typename T> struct print_traits<T *> {
        static constexpr char format_string[] = "%p";
        static constexpr size_t format_arg_count = 1;
        static T *format_arg(T *x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<char const *> {
        static constexpr char format_string[] = "%s";
        static constexpr size_t format_arg_count = 1;
        static char const *format_arg(char const *x, format_arg_t<0>) { return x; }
    };
    template<> struct print_traits<char *> : print_traits<char const *> {};
    template<size_t N> struct print_traits<char const [N]> : print_traits<char const *> {};
    template<size_t N> struct print_traits<char[N]> : print_traits<char const *> {};    

    namespace internal {
        template<typename T> struct string_constant_print_traits;
        template<char...c>
        struct string_constant_print_traits<string_constant<c...>> {
            static constexpr char format_string[] = {c..., '\0'};
            static constexpr size_t format_arg_count = 0;
        };
    };

    template<char...c> struct print_traits<string_constant<c...>>
    : internal::string_constant_print_traits<typename internal::escape_string_constant<c...>::type>
    {};
            
    template<typename T>
    struct print_traits<T&> : print_traits<T> {};
    template<typename T>
    struct print_traits<T&&> : print_traits<T> {};
    template<typename T>
    struct print_traits<T const> : print_traits<T> {};
    template<typename T>
    struct print_traits<T volatile> : print_traits<T> {};
        
    template<typename T>
    struct print_traits<T, typename std::enable_if<(sizeof(T::format_string)
                                                    && std::is_convertible<decltype(T::format_arg_count), std::size_t>::value)>::type>
    {
        static constexpr char const (&format_string)[sizeof(T::format_string)] = T::format_string;
        static constexpr size_t format_arg_count = T::format_arg_count;
        
        template<typename U, size_t N>
        static auto format_arg(U &&subject, format_arg_t<N> n)
        -> decltype(subject.format_arg(n))
        {
            return subject.format_arg(n);
        }
    };

    template<> struct print_traits<std::string> {
        static constexpr char format_string[] = "%*s";
        static constexpr size_t format_arg_count = 2;
        static int format_arg(std::string const &s, format_arg_t<0>) { return int(s.size()); }
        static char const *format_arg(std::string const &s, format_arg_t<1>) { return s.c_str(); }
    };
    
    namespace internal {
        inline constexpr size_t sum() { return 0; }
        inline constexpr size_t sum(size_t a) { return a; }
        template<typename T, typename...TT>
        inline constexpr size_t sum(T a, T b, TT...c)
        {
            return sum(a + b, c...);
        }
        
        template<typename T, typename NN = integers<sizeof(print_traits<T>::format_string) - 1>>
        struct format_chars;
        
        template<typename T, size_t...NN>
        struct format_chars<T, values<size_t, NN...>>
        {
            using type = string_constant<print_traits<T>::format_string[NN]...>;
        };

        template<typename...T>
        struct format_string : concat_string_constant<typename format_chars<T>::type...> {};
        
        template<size_t N, typename If, typename...T>
        struct format_arg;
        
        template<size_t N, typename T, typename...TT>
        struct format_arg<N, typename std::enable_if<(N < print_traits<T>::format_arg_count)>::type, T, TT...>
        {
            template<typename U, typename...UU>
            static auto get(U &&arg, UU &&...)
            -> decltype(print_traits<T>::format_arg(std::forward<U>(arg), format_arg_t<N>()))
            { return print_traits<T>::format_arg(std::forward<U>(arg), format_arg_t<N>()); }
        };
        
        template<size_t N, typename T, typename...TT>
        struct format_arg<N, typename std::enable_if<(N >= print_traits<T>::format_arg_count)>::type, T, TT...>
        {
            template<typename U, typename...UU>
            static auto get(U &&, UU &&...args)
            S_AUTO(format_arg<N - print_traits<T>::format_arg_count, void, TT...>::get(std::forward<UU>(args)...))
        };
        
        template<typename...TT>
        struct format_arg_count
        : std::integral_constant<std::size_t, sum(print_traits<TT>::format_arg_count...)>
        {};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
        template<size_t...N, typename...T>
        inline int _print(values<size_t, N...>, T &&...args)
        {
            return std::printf(internal::format_string<T...>::value,
                               format_arg<N, void, T...>::get(std::forward<T>(args)...)...);
        }
        template<size_t...N, typename...T>
        inline int _fprint(values<size_t, N...>, std::FILE *f, T &&...args)
        {
            return std::fprintf(f, internal::format_string<T...>::value,
                                format_arg<N, void, T...>::get(std::forward<T>(args)...)...);
        }
        template<size_t...N, typename...T>
        inline int _snprint(values<size_t, N...>, char *out, size_t size, T &&...args)
        {
            return std::snprintf(out, size, internal::format_string<T...>::value,
                                 format_arg<N, void, T...>::get(std::forward<T>(args)...)...);
        }
        
        template<typename String, typename...TT>
        inline void _strprintf(String &s, char const *format, TT &&...args)
        {
            size_t offset = s.size();
            size_t capacity = s.capacity();
            
            s.resize(capacity);
            for (;;) {
                size_t count = std::snprintf(&s[offset], capacity-offset+1, format,
                                             std::forward<TT>(args)...);
                if (offset+count <= capacity) {
                    s.resize(offset+count);
                    break;
                }
                capacity = offset+count;
                s.resize(capacity);
            }
        }
#pragma clang diagnostic pop

        template<size_t...N, typename String, typename...T>
        inline void _strprint(values<size_t, N...>, String &out, T &&...args)
        {
            _strprintf(out, internal::format_string<T...>::value,
                       format_arg<N, void, T...>::get(std::forward<T>(args)...)...);
        }

        template<typename TT, typename NN, typename CC>
        struct _tuple_print_traits;
        
        template<typename...T, size_t...N, char...c>
        struct _tuple_print_traits<types<T...>, values<size_t, N...>, string_constant<c...>> {
            static constexpr char format_string[] = {'{', c..., '}', '\0'};
            static constexpr size_t format_arg_count = sum(print_traits<T>::format_arg_count...);
            
            template<typename Agg, size_t M>
            static auto format_arg(Agg const &t, format_arg_t<M>)
            -> decltype(internal::format_arg<M, void, T...>::get(get<N>(t)...))
            {
                return internal::format_arg<M, void, T...>::get(get<N>(t)...);
            }
        };

        template<typename T, typename NN = integers<aggregate_size<T>::value>>
        struct tuple_print_traits;
        
        template<typename T, size_t...NN>
        struct tuple_print_traits<T, values<size_t, NN...>>
        : _tuple_print_traits<types<typename aggregate_element<NN, T>::type...>,
                              values<size_t, NN...>,
                              typename join_string_constant<string_constant<',', ' '>,
                                                            typename internal::format_chars<typename aggregate_element<NN, T>::type>::type...>::type>
                              
        {};
    }
    
    template<typename T>
    struct print_traits<T, typename std::enable_if<is_aggregate<T>::value>::type>
    : internal::tuple_print_traits<T> {};
    
    template<typename T>
    struct print_traits<T, typename std::enable_if<std::is_enum<T>::value>::type> {
        static constexpr char format_string[] = "%s";
        static constexpr size_t format_arg_count = 1;
        
        static char const *format_arg(T value, format_arg_t<0>) {
            return enum_member_name(value);
        }
    };
    
    template<typename...T>
    inline void print(T &&...args)
    {
        internal::_print(integers<internal::format_arg_count<T...>::value>(),
                         std::forward<T>(args)...);
    }
    
    template<typename...T>
    inline void fprint(std::FILE *out, T &&...args)
    {
        internal::_fprint(integers<internal::format_arg_count<T...>::value>(),
                          out, std::forward<T>(args)...);
    }
    
    template<typename...T>
    inline size_t snprint(char *out, size_t len, T &&...args)
    {
        return internal::_snprint(integers<internal::format_arg_count<T...>::value>(),
                                  out, len, std::forward<T>(args)...);
    }

    template<typename String, typename...T>
    inline void strprint(String &s, T &&...args)
    {
        internal::_strprint(integers<internal::format_arg_count<T...>::value>(),
                            s, std::forward<T>(args)...);
    }
    
    template<typename String = std::string, typename...T>
    inline String str(T &&...args)
    {
        String ret;
        strprint(ret, std::forward<T>(args)...);
        return ret;
    }

    namespace delim {
        constexpr string_constant<' '> space = {};
        constexpr string_constant<'\n'> nl = {};
        constexpr string_constant<'\t'> tab = {};
        constexpr string_constant<','> comma = {};
    }
    
    template<typename...T>
    inline void println(T &&...args)
    {
        print(std::forward<T>(args)..., delim::nl);
    }

    template<typename...T>
    inline void fprintln(std::FILE *out, T &&...args)
    {
        fprint(out, std::forward<T>(args)..., delim::nl);
    }

    template<typename...T>
    inline void snprintln(char *out, size_t len, T &&...args)
    {
        snprint(out, len, std::forward<T>(args)..., delim::nl);
    }

    template<typename String, typename...T>
    inline void strprintln(String &s, T &&...args)
    {
        strprint(s, std::forward<T>(args)..., delim::nl);
    }

    template<typename String = std::string, typename...T>
    inline String strln(T &&...args)
    {
        return str(std::forward<T>(args)..., delim::nl);
    }
}

#endif
