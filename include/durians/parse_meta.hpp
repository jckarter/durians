//
//  parse_meta.hpp
//  durians
//
//  Created by Joe Groff on 8/24/12.
//  Copyright (c) 2012 Durian Software. All rights reserved.
//

#ifndef durians_parse_meta_hpp
#define durians_parse_meta_hpp

#include <cstddef>
#include <type_traits>
#include <durians/packs.hpp>

namespace durians {
    namespace lex {
        template<char C>
        struct lit
        {
            static constexpr size_t match(char const *c, size_t n) {
                return c[n] == C ? n + 1 : -1;
            }
        };
        
        template<char Lo, char Hi>
        struct range
        {
            static constexpr size_t match(char const *c, size_t n) {
                return c[n] >= Lo && c[n] <= Hi ? n + 1 : -1;
            }
        };
        
        struct any
        {
            static constexpr size_t match(char const *c, size_t n) { return n + 1; }
        };
        
        template<typename...Rule>
        struct seq;
        
        template<typename Rule, typename...Rules>
        struct seq<Rule, Rules...>
        {
            static constexpr size_t next(char const *c, size_t n, size_t step) {
                return step != -1 ? seq<Rules...>::match(c, step) : -1;
            }
            
            static constexpr size_t match(char const *c, size_t n) {
                return next(c, n, Rule::match(c, n));
            }
        };
        
        template<>
        struct seq<>
        {
            static constexpr size_t match(char const *c, size_t n) { return n; }
        };
        
        template<typename Rule>
        struct opt
        {
            static constexpr size_t next(char const *c, size_t n, size_t step) {
                return step != -1 ? step : n;
            }
            static constexpr size_t match(char const *c, size_t n) {
                return next(c, n, Rule::match(c, n));
            }
        };
        
        template<typename...Rule>
        struct alt;
        
        template<typename Rule, typename...Rules>
        struct alt<Rule, Rules...>
        {
            static constexpr size_t next(char const *c, size_t n, size_t step) {
                return step != -1 ? step : alt<Rules...>::match(c, n);
            }
            static constexpr size_t match(char const *c, size_t n) {
                return next(c, n, Rule::match(c, n));
            }
        };
        
        template<>
        struct alt<>
        {
            static constexpr size_t match(char const *c, size_t n) { return -1; }
        };
        
        constexpr size_t star_next(size_t (*match)(char const*, size_t),
                                   char const *c, size_t n, size_t step)
        {
            return step != -1 ? match(c, step) : n;
        }

        template<typename Rule>
        struct star
        {
            static constexpr size_t match(char const *c, size_t n) {
                return star_next(match, c, n, Rule::match(c, n));
            }
        };

        template<typename Rule>
        struct plus
        {
            static constexpr size_t next(char const *c, size_t n, size_t step) {
                return step != -1 ? star<Rule>::match(c, step) : -1;
            }
            static constexpr size_t match(char const *c, size_t n) {
                return next(c, n, Rule::match(c, n));
            }
        };
    }

    namespace internal {
        template<typename Seq, typename...Rules>
        struct make_rule;
        
        template<typename Seq>
        struct make_rule<Seq>
        {
            using rule = Seq;
        };
        
        template<typename Rule>
        struct make_rule<lex::seq<>, Rule>
        {
            using rule = Rule;
        };
        
        template<typename Rule, typename...Rules, typename...Seq>
        struct make_rule<lex::seq<Seq...>, Rule, Rules...>
        : make_rule<lex::seq<Rule, Seq...>, Rules...>
        {};
        
        template<typename Rules, typename Parent>
        struct lexer_parent {};
        
        template<char C>
        struct underscore_rule
        {
            using rule = lex::lit<C>;
        };
        
        template<>
        struct underscore_rule<'d'>
        {
            using rule = lex::range<'0', '9'>;
        };

        template<>
        struct underscore_rule<'w'>
        {
            using rule = lex::alt<lex::range<'A', 'Z'>,
                                  lex::range<'a', 'z'>,
                                  lex::range<'0', '9'>,
                                  lex::lit<'_'>>;
        };
        
        template<>
        struct underscore_rule<'s'>
        {
            using rule = lex::alt<lex::lit<' '>, lex::lit<'\t'>,
                                  lex::lit<'\r'>, lex::lit<'\n'>>;
        };
        
        template<>
        struct underscore_rule<'n'>
        {
            using rule = lex::lit<'\n'>;
        };
        
        template<>
        struct underscore_rule<'t'>
        {
            using rule = lex::lit<'\t'>;
        };
        
        template<>
        struct underscore_rule<'r'>
        {
            using rule = lex::lit<'\r'>;
        };
        
        template<char From, char To, typename ToRule = typename underscore_rule<To>::rule>
        struct underscore_range
        {
            static_assert(From != From, "- must be followed by a character");
        };
        
        template<char From, char UnderscoreTo, char To>
        struct underscore_range<From, UnderscoreTo, lex::lit<To>>
        {
            using rule = lex::range<From, To>;
        };
    
    }

    template<char const *S, size_t N = 0,
             typename CC = values<char>,
             typename Rules = types<>,
             typename Parent = void>
    struct lexer_rule;
    
    template<char const *S, size_t N, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char>, types<Rules...>, Parent>
    : lexer_rule<S, N+1, values<char, S[N]>, types<Rules...>, Parent>
    {};
    
    template<char const *S, size_t N, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char, '\\'>, types<Rules...>, Parent>
    : lexer_rule<S, N+1, values<char, '\\', S[N]>, types<Rules...>, Parent>
    {};
    
    template<char const *S, size_t N, typename...Rules>
    struct lexer_rule<S, N, values<char, 0>, types<Rules...>, void>
    : internal::make_rule<lex::seq<>, Rules...>
    {
    };

    template<char const *S, size_t N, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char, '('>, types<Rules...>, Parent>
    : lexer_rule<S, N, values<char>, types<>,
                 internal::lexer_parent<types<Rules...>, Parent>>
    {
    };
    
    template<char const *S, size_t N, typename...Rules>
    struct lexer_rule<S, N, values<char, ')'>, types<Rules...>, void>
    {
        static_assert(N != N, ") must be preceded by a matching (");
    };
    
    template<char const *S, size_t N, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char, 0>, types<Rules...>, Parent>
    {
        static_assert(N != N, "( must be closed by a matching )");
    };
    
    template<char const *S, size_t N, typename...Rules,
             typename...ParentRules, typename Grandparent>
    struct lexer_rule<S, N, values<char, ')'>, types<Rules...>,
                      internal::lexer_parent<types<ParentRules...>, Grandparent>>
    : lexer_rule<S, N, values<char>,
                 types<typename internal::make_rule<lex::seq<>, Rules...>::rule,
                       ParentRules...>,
                 Grandparent>
    {};
    
    template<char const *S, size_t N, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char, '.'>, types<Rules...>, Parent>
    : lexer_rule<S, N, values<char>, types<lex::any, Rules...>, Parent>
    {};

    template<char const *S, size_t N, char C, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char, '\\', C>, types<Rules...>, Parent>
    : lexer_rule<S, N, values<char>,
                 types<typename internal::underscore_rule<C>::rule, Rules...>,
                 Parent>
    {};

    template<char const *S, size_t N, typename Rule, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char, '*'>, types<Rule, Rules...>, Parent>
    : lexer_rule<S, N, values<char>, types<lex::star<Rule>, Rules...>, Parent>
    {};
    
    template<char const *S, size_t N, typename Rule, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char, '+'>, types<Rule, Rules...>, Parent>
    : lexer_rule<S, N, values<char>, types<lex::plus<Rule>, Rules...>, Parent>
    {};

    template<char const *S, size_t N, typename Rule, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char, '?'>, types<Rule, Rules...>, Parent>
    : lexer_rule<S, N, values<char>, types<lex::opt<Rule>, Rules...>, Parent>
    {};
    
    template<char const *S, size_t N, typename Parent>
    struct lexer_rule<S, N, values<char, '*'>, types<>, Parent>
    {
        static_assert(N != N, "* must be preceded by an atom");
    };

    template<char const *S, size_t N, typename Parent>
    struct lexer_rule<S, N, values<char, '+'>, types<>, Parent>
    {
        static_assert(N != N, "+ must be preceded by an atom");
    };

    template<char const *S, size_t N, typename Parent>
    struct lexer_rule<S, N, values<char, '?'>, types<>, Parent>
    {
        static_assert(N != N, "? must be preceded by an atom");
    };
    
    template<char const *S, size_t N, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char, '\\', 0>, types<Rules...>, Parent>
    {
        static_assert(N != N, "\\ must be followed by a character");
    };

    template<char const *S, size_t N, char C, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char, C>, types<Rules...>, Parent>
    : lexer_rule<S, N, values<char>, types<lex::lit<C>, Rules...>, Parent>
    {};
    
    namespace internal {
        template<char const *S, size_t N, typename CC, typename Alt, typename Parent>
        struct lexer_char_class;
        
        template<char const *S, size_t N, typename Alt, typename Parent>
        struct lexer_char_class<S, N, values<char>, Alt, Parent>
        : lexer_char_class<S, N+1, values<char, S[N]>, Alt, Parent>
        {};
        
        template<char const *S, size_t N, typename...Alts, typename Parent>
        struct lexer_char_class<S, N, values<char, '\\'>, lex::alt<Alts...>, Parent>
        : lexer_char_class<S, N+1, values<char, '\\', S[N]>, lex::alt<Alts...>, Parent>
        {};

        template<char const *S, size_t N, char C, typename...Alts, typename Parent>
        struct lexer_char_class<S, N, values<char, '-'>, lex::alt<lex::lit<C>, Alts...>, Parent>
        : lexer_char_class<S, N+1, values<char, '-', S[N]>, lex::alt<lex::lit<C>, Alts...>, Parent>
        {};
        
        template<char const *S, size_t N, typename...Alts, typename Parent>
        struct lexer_char_class<S, N, values<char, '-'>, lex::alt<Alts...>, Parent>
        {
            static_assert(N != N, "- must be preceded by a character");
        };
        
        template<char const *S, size_t N, char C, typename...Alts, typename Parent>
        struct lexer_char_class<S, N, values<char, '-', '\\'>, lex::alt<lex::lit<C>, Alts...>, Parent>
        : lexer_char_class<S, N+1, values<char, '-', '\\', S[N]>, lex::alt<lex::lit<C>, Alts...>, Parent>
        {};
        
        template<char const *S, size_t N, char From, typename...Alts, typename Parent>
        struct lexer_char_class<S, N, values<char, '-', ']'>, lex::alt<lex::lit<From>, Alts...>, Parent>
        {
            static_assert(N != N, "- must be followed by a character");
        };
        
        template<char const *S, size_t N, typename...Alts, typename Parent>
        struct lexer_char_class<S, N, values<char, 0>, lex::alt<Alts...>, Parent>
        {
            static_assert(N != N, "[ must be closed by a matching ]");
        };

        template<char const *S, size_t N, typename Alt, typename Parent>
        struct lexer_char_class<S, N, values<char, '\\', 0>, Alt, Parent>
        {
            static_assert(N != N, "[ must be closed by a matching ]");
        };
        
        template<char const *S, size_t N, char From, typename...Alts, typename Parent>
        struct lexer_char_class<S, N, values<char, '-', 0>, lex::alt<lex::lit<From>, Alts...>, Parent>
        {
            static_assert(N != N, "[ must be closed by a matching ]");
        };
        
        template<char const *S, size_t N, char From, typename...Alt, typename Parent>
        struct lexer_char_class<S, N, values<char, '-', '\\', 0>, lex::alt<lex::lit<From>, Alt...>, Parent>
        {
            static_assert(N != N, "[ must be closed by a matching ]");
        };
        
        template<char const *S, size_t N, char C, typename...Alts, typename Parent>
        struct lexer_char_class<S, N, values<char, C>, lex::alt<Alts...>, Parent>
        : lexer_char_class<S, N, values<char>, lex::alt<lex::lit<C>, Alts...>, Parent>
        {};
        
        template<char const *S, size_t N, char C, typename...Alts, typename Parent>
        struct lexer_char_class<S, N, values<char, '\\', C>, lex::alt<Alts...>, Parent>
        : lexer_char_class<S, N, values<char>,
                           lex::alt<typename internal::underscore_rule<C>::rule, Alts...>,
                           Parent>
        {};

        template<char const *S, size_t N, char From, char To, typename...Alts, typename Parent>
        struct lexer_char_class<S, N, values<char, '-', To>, lex::alt<lex::lit<From>, Alts...>, Parent>
        : lexer_char_class<S, N, values<char>, lex::alt<lex::range<From, To>, Alts...>, Parent>
        {
            static_assert(From <= To,
                          "start of character class range must be less than or equal to end");
        };
        
        template<char const *S, size_t N, char From, char To, typename...Alts, typename Parent>
        struct lexer_char_class<S, N, values<char, '-', '\\', To>, lex::alt<lex::lit<From>, Alts...>, Parent>
        : lexer_char_class<S, N, values<char>,
                           lex::alt<typename internal::underscore_range<From, To>::rule, Alts...>,
                           Parent>
        {
            static_assert(From <= To,
                          "start of character class range must be less than or equal to end");
        };
        
        template<char const *S, size_t N, typename...Alt, typename...Rules, typename Parent>
        struct lexer_char_class<S, N, values<char, ']'>, lex::alt<Alt...>,
                                lexer_parent<types<Rules...>, Parent>>
        : lexer_rule<S, N, values<char>, types<lex::alt<Alt...>, Rules...>, Parent>
        {};
    }

    template<char const *S, size_t N, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char, '['>, types<Rules...>, Parent>
    : internal::lexer_char_class<S, N, values<char>, lex::alt<>,
                                 internal::lexer_parent<types<Rules...>, Parent>>
    {};

    template<char const *S, size_t N, typename...Rules, typename Parent>
    struct lexer_rule<S, N, values<char, '|'>, types<Rules...>, Parent>
    {
        using rule = lex::alt<typename internal::make_rule<lex::seq<>, Rules...>::rule,
                              typename lexer_rule<S, N>::rule>;
    };
    
    template<typename...Rules>
    struct rules;
    
    template<char...C>
    struct discard;
    
    template<typename Rule, template<char...> class Token>
    struct rule {};
    template<char const *S, template<char...> class Token>
    using rule_str = rule<typename lexer_rule<S>::rule, Token>;

    namespace internal {
        template<template<char...> class Token, char const *S, size_t From, size_t To, char...C>
        struct make_token
        : make_token<Token, S, From+1, To, C..., S[From]>
        {};

        template<template<char...> class Token, char const *S, size_t End, char...C>
        struct make_token<Token, S, End, End, C...>
        {
            using type = Token<C...>;
        };
    }
    
    template<typename AllRules, char const *S,
             size_t N = 0,
             typename Rules = AllRules, typename Tokens = types<>, typename If = void>
    struct tokenize;
    
    template<typename AllRules, typename Rule, template<char...> class Token, typename...Rules,
             char const *S, size_t N, typename Tokens>
    struct tokenize<AllRules, S, N,
                    rules<rule<Rule, Token>, Rules...>,
                    Tokens,
                    typename std::enable_if<S[N] != 0 && Rule::match(S, N) == -1>::type>
    : tokenize<AllRules, S, N, rules<Rules...>, Tokens>
    {
    };
    
    template<typename AllRules, typename...Rules,
             char const *S, size_t N, typename Tokens>
    struct tokenize<AllRules, S, N,
                    rules<Rules...>,
                    Tokens,
                    typename std::enable_if<S[N] == 0>::type>
    {
        using tokens = Tokens;
        static constexpr size_t end = N;
    };
    
    template<typename AllRules,
             char const *S, size_t N, typename Tokens>
    struct tokenize<AllRules, S, N,
                    rules<>,
                    Tokens>
    {
        using tokens = Tokens;
        static constexpr size_t end = N;
    };

    template<typename AllRules, typename Rule, template<char...> class Token, typename...Rules,
             char const *S, size_t N, typename...Tokens>
    struct tokenize<AllRules, S, N,
                    rules<rule<Rule, Token>, Rules...>,
                    types<Tokens...>,
                    typename std::enable_if<Rule::match(S, N) != -1>::type>
    : tokenize<AllRules, S, Rule::match(S, N), AllRules,
               types<Tokens..., typename internal::make_token<Token, S, N, Rule::match(S, N)>::type>>
    {
    };

    template<typename AllRules, typename Rule, typename...Rules,
             char const *S, size_t N, typename...Tokens>
    struct tokenize<AllRules, S, N,
                    rules<rule<Rule, discard>, Rules...>,
                    types<Tokens...>,
                    typename std::enable_if<Rule::match(S, N) != -1>::type>
    : tokenize<AllRules, S, Rule::match(S, N), AllRules,
               types<Tokens...>>
    {
    };
    
    namespace parse {
        template<template<char...> class TokenType>
        struct token {
            template<typename...Tokens> struct match {};
        };
        
        struct any {
            template<typename...Tokens> struct match {};
        };
        
        template<typename...Rules>
        struct seq {
            template<typename...Tokens> struct match {};
        };
        
        template<typename...Rules>
        struct alt {
            template<typename...Tokens> struct match {};
        };
        
        template<typename Rule>
        struct opt {
            template<typename...Tokens> struct match {};
        };
        
        template<typename Rule>
        struct star {
            template<typename...Tokens> struct match {};
        };
        
        template<typename Rule>
        struct plus {
            template<typename...Tokens> struct match {};
        };
    }
}

#endif
