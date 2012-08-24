//
//  print.hpp
//  durians
//
//  Created by Joe Groff on 8/22/12.
//  Copyright (c) 2012 Durian Software. All rights reserved.
//

#ifndef durians_print_hpp
#define durians_print_hpp

#include <durians/misc.hpp>
#include <durians/packs.hpp>
#include <durians/slice.hpp>
#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <type_traits>

namespace durians {
    template<size_t N> struct format_arg_t {};
    
    template<typename T, typename If = void>
    struct print_traits;
    
    template<>
    struct print_traits<bool>
    {
        static constexpr char format_token[] = "s";
        static char const *format_arg(bool b) { return b ? "true" : "false"; }
    };
    
    template<>
    struct print_traits<char>
    {
        static constexpr char format_token[] = "c";
        static int format_arg(char c) { return c; }
    };
    
    template<>
    struct print_traits<wchar_t>
    {
        static constexpr char format_token[] = "lc";
        static std::wint_t format_arg(wchar_t c) { return c; }
    };
    
    namespace internal
    {
        template<typename T>
        struct signed_int_print_traits
        {
            static_assert(std::is_signed<T>::value && sizeof(T) <= sizeof(int), "");
            static constexpr char format_token[] = "d";
            static constexpr char octal_format_token[] = "o";
            static constexpr char hex_format_token[] = "x";
            static constexpr char HEX_format_token[] = "X";
            static int format_arg(T i) { return i; }
        };
    }
    
    template<>
    struct print_traits<signed char> : internal::signed_int_print_traits<signed char>
    {
    };
    
    template<>
    struct print_traits<short> : internal::signed_int_print_traits<short>
    {
    };
    
    template<>
    struct print_traits<int> : internal::signed_int_print_traits<int>
    {
    };
    
    template<>
    struct print_traits<long>
    {
        static constexpr char format_token[] = "ld";
        static constexpr char octal_format_token[] = "lo";
        static constexpr char hex_format_token[] = "lx";
        static constexpr char HEX_format_token[] = "lX";
        static long format_arg(long i) { return i; }
    };
    
    template<>
    struct print_traits<long long>
    {
        static constexpr char format_token[] = "lld";
        static constexpr char octal_format_token[] = "llo";
        static constexpr char hex_format_token[] = "llx";
        static constexpr char HEX_format_token[] = "llX";
        static long long format_arg(long long i) { return i; }
    };
    
    template<typename T>
    struct print_traits<T,
                        typename std::enable_if<(std::is_same<T, std::intmax_t>::value
                                                 && !std::is_same<std::intmax_t, long long>::value
                                                 && !std::is_same<std::intmax_t, long>::value)>::type>
    {
        static constexpr char format_token[] = "jd";
        static constexpr char octal_format_token[] = "jo";
        static constexpr char hex_format_token[] = "jx";
        static constexpr char HEX_format_token[] = "jX";
        static std::intmax_t format_arg(std::intmax_t i) { return i; }
    };
    
    namespace internal
    {
        template<typename T>
        struct unsigned_int_print_traits
        {
            static_assert(std::is_unsigned<T>::value && sizeof(T) <= sizeof(unsigned), "");
            static constexpr char format_token[] = "u";
            static constexpr char octal_format_token[] = "o";
            static constexpr char hex_format_token[] = "x";
            static constexpr char HEX_format_token[] = "X";
            static unsigned format_arg(T i) { return i; }
        };
    }

    template<>
    struct print_traits<unsigned char> : internal::unsigned_int_print_traits<unsigned char>
    {
    };
    
    template<>
    struct print_traits<unsigned short> : internal::unsigned_int_print_traits<unsigned short>
    {
    };
    
    template<>
    struct print_traits<unsigned> : internal::unsigned_int_print_traits<unsigned>
    {
    };
    
    template<>
    struct print_traits<unsigned long>
    {
        static constexpr char format_token[] = "lu";
        static constexpr char octal_format_token[] = "lo";
        static constexpr char hex_format_token[] = "lx";
        static constexpr char HEX_format_token[] = "lX";
        static long format_arg(long i) { return i; }
    };
    
    template<>
    struct print_traits<unsigned long long>
    {
        static constexpr char format_token[] = "llu";
        static constexpr char octal_format_token[] = "llo";
        static constexpr char hex_format_token[] = "llx";
        static constexpr char HEX_format_token[] = "llX";
        static long long format_arg(long long i) { return i; }
    };
    
    template<typename T>
    struct print_traits<T,
                        typename std::enable_if<(std::is_same<T, std::uintmax_t>::value
                                                 && !std::is_same<std::uintmax_t, long long>::value
                                                 && !std::is_same<std::uintmax_t, long>::value)>::type>
    {
        static constexpr char format_token[] = "ju";
        static constexpr char octal_format_token[] = "jo";
        static constexpr char hex_format_token[] = "jx";
        static constexpr char HEX_format_token[] = "jX";
        static std::intmax_t format_arg(std::uintmax_t i) { return i; }
    };
    
    namespace internal {
        template<typename T>
        struct float_print_traits {
            static constexpr char format_token[] = "f";
            static constexpr char hex_format_token[] = "a";
            static constexpr char HEX_format_token[] = "A";
            static double format_arg(T f) { return f; }
        };
    }
    
    template<>
    struct print_traits<float> : internal::float_print_traits<float>
    {
    };

    template<>
    struct print_traits<double> : internal::float_print_traits<double>
    {
    };
    
    template<>
    struct print_traits<long double>
    {
        static constexpr char format_token[] = "Lf";
        static constexpr char hex_format_token[] = "La";
        static constexpr char HEX_format_token[] = "LA";
        static long double format_arg(long double f) { return f; }
    };
    
    template<>
    struct print_traits<char const *>
    {
        static constexpr char format_token[] = "s";
        static char const *format_arg(char const *s) { return s; }
    };
    
    template<>
    struct print_traits<char *> : print_traits<char const *>
    {
    };
    
    template<>
    struct print_traits<wchar_t const *>
    {
        static constexpr char format_token[] = "ls";
        static wchar_t const *format_arg(wchar_t const *s) { return s; }
    };
    
    template<>
    struct print_traits<wchar_t *> : print_traits<wchar_t const *>
    {
    };
    
    template<typename T>
    struct print_traits<T*>
    {
        static constexpr char format_token[] = "p";
        static T *format_arg(T *p) { return p; }
    };
    
    template<typename T>
    struct print_traits<T, decltype(static_cast<void>(lvalue<T const>().print(stdout)))>
    {
        template<typename Dest>
        static void print(Dest &&dest, T const &thing)
        {
            thing.print(std::forward<Dest>(dest));
        }
    };
    
    namespace internal {
        template<typename T>
        struct base_print_traits : print_traits<typename std::decay<T>::type> {};        
        
        template<typename T, typename If = void>
        struct _is_print_atom : std::false_type {};
        template<typename T>
        struct _is_print_atom<T, decltype(static_cast<void>(static_cast<char const *>(print_traits<typename std::decay<T>::type>::format_token)))>
        : std::true_type {};

        template<typename T, typename If = void>
        struct _has_print_method : std::false_type {};
        template<typename T>
        struct _has_print_method<T, decltype(static_cast<void>(base_print_traits<T>::print(stdout, rvalue<typename std::remove_reference<T>::type>())))>
        : std::true_type {};
    }
        
    template<typename T>
    using is_print_atom = internal::_is_print_atom<T>;
    template<typename T>
    using has_print_method = internal::_has_print_method<T>;

    template<typename T>
    struct octal_t
    {
        T &&what;
    };

    template<typename T>
    struct hex_t
    {
        T &&what;
    };

    template<typename T>
    struct HEX_t
    {
        T &&what;
    };
    
    template<typename T>
    octal_t<T> octal(T &&what) { return octal_t<T>{std::forward<T>(what)}; }
    template<typename T>
    hex_t<T> hex(T &&what) { return hex_t<T>{std::forward<T>(what)}; }
    template<typename T>
    HEX_t<T> HEX(T &&what) { return HEX_t<T>{std::forward<T>(what)}; }
    
    template<typename T>
    struct print_traits<octal_t<T>, typename std::enable_if<is_print_atom<T>::value>::type>
    {
        static constexpr auto &format_token = print_traits<T>::octal_format_token;
        static auto format_arg(octal_t<T> const &o)
        -> decltype(print_traits<T>::format_arg(o.what))
        {
            return print_traits<T>::format_arg(o.what);
        }
    };
    
    template<typename T>
    struct print_traits<hex_t<T>, typename std::enable_if<is_print_atom<T>::value>::type>
    {
        static constexpr auto &format_token = print_traits<T>::hex_format_token;
        static auto format_arg(hex_t<T> const &o)
        -> decltype(print_traits<T>::format_arg(o.what))
        {
            return print_traits<T>::format_arg(o.what);
        }
    };
    
    template<typename T>
    struct print_traits<HEX_t<T>, typename std::enable_if<is_print_atom<T>::value>::type>
    {
        static constexpr auto &format_token = print_traits<T>::HEX_format_token;
        static auto format_arg(HEX_t<T> const &o)
        -> decltype(print_traits<T>::format_arg(o.what))
        {
            return print_traits<T>::format_arg(o.what);
        }
    };
    
    namespace internal {
        template<typename T, typename NN = integers<sizeof(T::value) - 1>>
        struct string_literal;
        
        template<char...C>
        struct _string_literal
        {
            static const char value[sizeof...(C)+1];
        };

        template<char...C>
        char const _string_literal<C...>::value[sizeof...(C)+1] = {C..., 0};
        
        template<typename T, size_t...NN>
        struct string_literal<T, values<size_t, NN...>>
        : _string_literal<T::value[NN]...>
        {
        };
        
        template<char...C>
        struct string_constant {
            static constexpr char value[] = {C..., 0};
        };
        
        template<typename T, typename If = void>
        struct _is_string_constant : std::false_type {};
        template<typename T>
        struct _is_string_constant<T, typename std::enable_if<(std::is_empty<T>::value
                                                               && std::is_convertible<decltype(T::value), char const*>::value)>::type>
        : std::true_type {};
        
        template<typename T>
        using is_string_constant = _is_string_constant<typename std::decay<T>::type>;
        
        template<typename A, typename B,
                 typename NN = integers<sizeof(A::value)-1>,
                 typename MM = integers<sizeof(B::value)-1>>
        struct concat;
        template<typename A, typename B, size_t...NN, size_t...MM>
        struct concat<A, B, values<size_t, NN...>, values<size_t, MM...>>
        {
            static constexpr char value[] = {A::value[NN]..., B::value[MM]..., 0};
        };
        
        template<typename A, size_t N, typename If, char...C>
        struct _escape : _escape<A, N+1, void, C..., A::value[N]> {};
        template<typename A, size_t N, char...C>
        struct _escape<A, N, typename std::enable_if<A::value[N] == '%'>::type, C...>
        : _escape<A, N+1, void, C..., '%', '%'>
        {
        };
        
        template<typename A, size_t N, char...C>
        struct _escape<A, N, typename std::enable_if<A::value[N] == 0>::type, C...>
        : string_constant<C...>
        {
        };
        
        template<typename A>
        using escape = _escape<A, 0, void>;
        
        template<size_t N, char...C>
        struct stringize_int
        : stringize_int<N/10, C..., '0' + N%10>
        {};
        
        template<char...C>
        struct stringize_int<0, C...>
        {
            static constexpr char value[] = {C..., 0};
        };

        // FIXME support width, precision, other printf flags
        template<size_t Width, size_t Prec, typename T,
                 typename NN = integers<sizeof(internal::base_print_traits<T>::format_token)-1>>
        struct format_spec;
        
        template<typename T, size_t...NN>
        struct format_spec<0, 0, T, values<size_t, NN...>>
        {
            static constexpr char value[] = {'%', internal::base_print_traits<T>::format_token[NN]..., 0};
        };
        
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
        template<typename...AA>
        inline int printf_to(FILE *f, char const *fmt, AA &&...args) noexcept
        {
            return std::fprintf(f, fmt, args...);
        }
        
        template<typename...AA>
        inline int printf_to(mutable_slice<char> s, char const *fmt, AA &&...args) noexcept
        {
            return std::snprintf(s.data(), s.size(), fmt, args...);
        }
        
        template<typename...AA>
        inline int printf_to(std::string &s, char const *format, AA &&...args) noexcept
        {
            size_t offset = s.size();
            size_t capacity = s.capacity();
            size_t count;
            
            s.resize(capacity);
            for (;;) {
                count = std::snprintf(&s[offset], capacity-offset+1, format,
                                      std::forward<AA>(args)...);
                if (offset+count <= capacity) {
                    s.resize(offset+count);
                    break;
                }
                capacity = offset+count;
                s.resize(capacity);
            }
            return int(count);
        }
#pragma clang diagnostic pop

        // all parameters handled
        template<size_t N, typename Dest, typename...TT, typename Fmt, typename...AA>
        inline void _print_to(typename std::enable_if<N == sizeof...(TT)>::type *,
                              Dest &&dest,
                              std::tuple<TT...> const &args,
                              Fmt,
                              AA &&...format_args)
        {
            printf_to(std::forward<Dest>(dest), string_literal<Fmt>::value,
                      std::forward<AA>(format_args)...);
        }

        // handle print atom
        template<size_t N, typename Dest, typename...TT, typename Fmt, typename...AA>
        inline void _print_to(typename std::enable_if<is_type_at<is_print_atom, N, TT...>::value>::type *,
                              Dest &&dest,
                              std::tuple<TT...> const &args,
                              Fmt,
                              AA &&...format_args)
        {
            
            _print_to<N+1>(nullptr, std::forward<Dest>(dest), args,
                           concat<Fmt, format_spec<0, 0, type_at<N, TT...>>>(),
                           std::forward<AA>(format_args)...,
                           base_print_traits<type_at<N, TT...>>::format_arg(std::get<N>(args)));
        }

        // handle string constant (empty type with static constexpr char value[] member)
        template<size_t N, typename Dest, typename...TT, typename Fmt, typename...AA>
        inline void _print_to(typename std::enable_if<is_type_at<is_string_constant, N, TT...>::value>::type *,
                              Dest &&dest,
                              std::tuple<TT...> const &args,
                              Fmt,
                              AA &&...format_args)
        {
            
            _print_to<N+1>(nullptr, std::forward<Dest>(dest), args,
                           concat<Fmt, base_print_traits<type_at<N, TT...>>>(),
                           std::forward<AA>(format_args)...);
        }
        
        // handle type with print() method
        template<size_t N, typename Dest, typename...TT, typename Fmt, typename...AA>
        inline void _print_to(typename std::enable_if<is_type_at<has_print_method, N, TT...>::value>::type *,
                              Dest &&dest,
                              std::tuple<TT...> const &args,
                              Fmt,
                              AA &&...format_args)
        {
            printf_to(std::forward<Dest>(dest), string_literal<Fmt>::value,
                      std::forward<AA>(format_args)...);

            internal::base_print_traits<type_at<N, TT...>>::print(std::forward<Dest>(dest), std::get<N>(args));
            
            _print_to<N+1>(nullptr, std::forward<Dest>(dest), args,
                           internal::string_constant<>());
        }
    }

    template<typename T>
    struct print_traits<T, typename std::enable_if<internal::is_string_constant<T>::value>::type>
    {
        static constexpr auto &value = internal::escape<T>::value;
    };

    namespace delim {
        constexpr internal::string_constant<'\n'> nl = {};
        constexpr internal::string_constant<' '> space = {};
        constexpr internal::string_constant<'\t'> tab = {};
    }
    
    template<typename Dest, typename...TT>
    inline void print_to(Dest &&dest, TT &&...args)
    {
        internal::_print_to<0>(nullptr, std::forward<Dest>(dest),
                               std::tuple<TT &&...>{std::forward<TT>(args)...},
                               internal::string_constant<>());
    }
    
    template<typename Dest, typename...TT>
    inline void println_to(Dest &&dest, TT &&...args)
    {
        print_to(std::forward<Dest>(dest), std::forward<TT>(args)..., delim::nl);
    }
    
    template<typename Dest, typename...TT>
    inline void print(Dest &&dest, TT &&...args)
    {
        print_to(stdout, std::forward<TT>(args)...);
    }
    
    template<typename Dest, typename...TT>
    inline void println(Dest &&dest, TT &&...args)
    {
        print_to(stdout, std::forward<TT>(args)..., delim::nl);
    }
    
    template<typename...AA>
    inline std::string str(AA &&...args)
    {
        std::string s;
        print_to(s, std::forward<AA>(args)...);
        return s;
    }
}

#endif
