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
            static constexpr char const *match(char const *c) { return *c == C ? c + 1 : nullptr; }
        };
        
        template<char Lo, char Hi>
        struct range
        {
            static constexpr char const *match(char const *c) {
                return *c >= Lo && *c <= Hi ? c + 1 : nullptr;
            }
        };
        
        struct any
        {
            static constexpr char const *match(char const *c) { return c + 1; }
        };
        
        template<typename...Rule>
        struct seq;
        
        template<typename Rule, typename...Rules>
        struct seq<Rule, Rules...>
        {
            static constexpr char const *next(char const *step) {
                    return step ? seq<Rules...>::match(step) : nullptr;
            }
            
            static constexpr char const *match(char const *c) {
                return next(Rule::match(c));
            }
        };
        
        template<>
        struct seq<>
        {
            static constexpr char const *match(char const *c) { return c; }
        };
        
        template<typename Rule>
        struct opt
        {
            static constexpr char const *next(char const *c, char const *step) {
                return step ? step : c;
            }
            static constexpr char const *match(char const *c) { return next(c, Rule::match(c)); }
        };
        
        template<typename...Rule>
        struct alt;
        
        template<typename Rule, typename...Rules>
        struct alt<Rule, Rules...>
        {
            static constexpr char const *next(char const *c, char const *step) {
                return step ? step : alt<Rules...>::match(c);
            }
            static constexpr char const *match(char const *c) { return next(c, Rule::match(c)); }
        };
        
        template<>
        struct alt<>
        {
            static constexpr char const *match(char const *c) { return nullptr; }
        };
        
        constexpr char const *star_next(char const *(*match)(char const*),
                                        char const *c, char const *step)
        {
            return step ? match(step) : c;
        }

        template<typename Rule>
        struct star
        {
            static constexpr char const *match(char const *c) {
                return star_next(match, c, Rule::match(c));
            }
        };

        template<typename Rule>
        struct plus
        {
            static constexpr char const *next(char const *c, char const *step) {
                return step ? star<Rule>::match(step) : nullptr;
            }
            static constexpr char const *match(char const *c) { return next(c, Rule::match(c)); }
        };
        
        namespace internal {
            template<typename Seq, typename...Rules>
            struct make_rule;
            
            template<typename Seq>
            struct make_rule<Seq>
            {
                using rule = Seq;
            };
            
            template<typename Rule>
            struct make_rule<seq<>, Rule>
            {
                using rule = Rule;
            };
            
            template<typename Rule, typename...Rules, typename...Seq>
            struct make_rule<seq<Seq...>, Rule, Rules...>
            : make_rule<seq<Rule, Seq...>, Rules...>
            {};
            
            template<typename Rules, typename Parent>
            struct lexer_parent {};
            
            template<char C>
            struct underscore_rule
            {
                using rule = lit<C>;
            };
            
            template<>
            struct underscore_rule<'d'>
            {
                using rule = range<'0', '9'>;
            };

            template<>
            struct underscore_rule<'w'>
            {
                using rule = alt<range<'A', 'Z'>,
                                 range<'a', 'z'>,
                                 range<'0', '9'>,
                                 lit<'_'>>;
            };
            
            template<>
            struct underscore_rule<'n'>
            {
                using rule = lit<'\n'>;
            };
            
            template<>
            struct underscore_rule<'t'>
            {
                using rule = lit<'\t'>;
            };
            
            template<>
            struct underscore_rule<'r'>
            {
                using rule = lit<'\r'>;
            };
            
            template<char From, char To, typename ToRule = typename underscore_rule<To>::rule>
            struct underscore_range
            {
                static_assert(From != From, "- must be followed by a character");
            };
            
            template<char From, char UnderscoreTo, char To>
            struct underscore_range<From, UnderscoreTo, lit<To>>
            {
                using rule = range<From, To>;
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
        : internal::make_rule<seq<>, Rules...>
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
                     types<typename internal::make_rule<seq<>, Rules...>::rule,
                           ParentRules...>,
                     Grandparent>
        {};
        
        template<char const *S, size_t N, typename...Rules, typename Parent>
        struct lexer_rule<S, N, values<char, '.'>, types<Rules...>, Parent>
        : lexer_rule<S, N, values<char>, types<any, Rules...>, Parent>
        {};

        template<char const *S, size_t N, char C, typename...Rules, typename Parent>
        struct lexer_rule<S, N, values<char, '\\', C>, types<Rules...>, Parent>
        : lexer_rule<S, N, values<char>,
                     types<typename internal::underscore_rule<C>::rule, Rules...>,
                     Parent>
        {};

        template<char const *S, size_t N, typename Rule, typename...Rules, typename Parent>
        struct lexer_rule<S, N, values<char, '*'>, types<Rule, Rules...>, Parent>
        : lexer_rule<S, N, values<char>, types<star<Rule>, Rules...>, Parent>
        {};
        
        template<char const *S, size_t N, typename Rule, typename...Rules, typename Parent>
        struct lexer_rule<S, N, values<char, '+'>, types<Rule, Rules...>, Parent>
        : lexer_rule<S, N, values<char>, types<plus<Rule>, Rules...>, Parent>
        {};

        template<char const *S, size_t N, typename Rule, typename...Rules, typename Parent>
        struct lexer_rule<S, N, values<char, '?'>, types<Rule, Rules...>, Parent>
        : lexer_rule<S, N, values<char>, types<opt<Rule>, Rules...>, Parent>
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
        : lexer_rule<S, N, values<char>, types<lit<C>, Rules...>, Parent>
        {};
        
        namespace internal {
            template<char const *S, size_t N, typename CC, typename Alt, typename Parent>
            struct lexer_char_class;
            
            template<char const *S, size_t N, typename Alt, typename Parent>
            struct lexer_char_class<S, N, values<char>, Alt, Parent>
            : lexer_char_class<S, N+1, values<char, S[N]>, Alt, Parent>
            {};
            
            template<char const *S, size_t N, typename...Alts, typename Parent>
            struct lexer_char_class<S, N, values<char, '\\'>, alt<Alts...>, Parent>
            : lexer_char_class<S, N+1, values<char, '\\', S[N]>, alt<Alts...>, Parent>
            {};

            template<char const *S, size_t N, char C, typename...Alts, typename Parent>
            struct lexer_char_class<S, N, values<char, '-'>, alt<lit<C>, Alts...>, Parent>
            : lexer_char_class<S, N+1, values<char, '-', S[N]>, alt<lit<C>, Alts...>, Parent>
            {};
            
            template<char const *S, size_t N, typename...Alts, typename Parent>
            struct lexer_char_class<S, N, values<char, '-'>, alt<Alts...>, Parent>
            {
                static_assert(N != N, "- must be preceded by a character");
            };
            
            template<char const *S, size_t N, char C, typename...Alts, typename Parent>
            struct lexer_char_class<S, N, values<char, '-', '\\'>, alt<lit<C>, Alts...>, Parent>
            : lexer_char_class<S, N+1, values<char, '-', '\\', S[N]>, alt<lit<C>, Alts...>, Parent>
            {};
            
            template<char const *S, size_t N, char From, typename...Alts, typename Parent>
            struct lexer_char_class<S, N, values<char, '-', ']'>, alt<lit<From>, Alts...>, Parent>
            {
                static_assert(N != N, "- must be followed by a character");
            };
            
            template<char const *S, size_t N, typename...Alts, typename Parent>
            struct lexer_char_class<S, N, values<char, 0>, alt<Alts...>, Parent>
            {
                static_assert(N != N, "[ must be closed by a matching ]");
            };

            template<char const *S, size_t N, typename Alt, typename Parent>
            struct lexer_char_class<S, N, values<char, '\\', 0>, Alt, Parent>
            {
                static_assert(N != N, "[ must be closed by a matching ]");
            };
            
            template<char const *S, size_t N, char From, typename...Alts, typename Parent>
            struct lexer_char_class<S, N, values<char, '-', 0>, alt<lit<From>, Alts...>, Parent>
            {
                static_assert(N != N, "[ must be closed by a matching ]");
            };
            
            template<char const *S, size_t N, char From, typename...Alt, typename Parent>
            struct lexer_char_class<S, N, values<char, '-', '\\', 0>, alt<lit<From>, Alt...>, Parent>
            {
                static_assert(N != N, "[ must be closed by a matching ]");
            };
            
            template<char const *S, size_t N, char C, typename...Alts, typename Parent>
            struct lexer_char_class<S, N, values<char, C>, alt<Alts...>, Parent>
            : lexer_char_class<S, N, values<char>, alt<lit<C>, Alts...>, Parent>
            {};
            
            template<char const *S, size_t N, char C, typename...Alts, typename Parent>
            struct lexer_char_class<S, N, values<char, '\\', C>, alt<Alts...>, Parent>
            : lexer_char_class<S, N, values<char>,
                               alt<typename internal::underscore_rule<C>::rule, Alts...>,
                               Parent>
            {};

            template<char const *S, size_t N, char From, char To, typename...Alts, typename Parent>
            struct lexer_char_class<S, N, values<char, '-', To>, alt<lit<From>, Alts...>, Parent>
            : lexer_char_class<S, N, values<char>, alt<range<From, To>, Alts...>, Parent>
            {
                static_assert(From <= To,
                              "start of character class range must be less than or equal to end");
            };
            
            template<char const *S, size_t N, char From, char To, typename...Alts, typename Parent>
            struct lexer_char_class<S, N, values<char, '-', '\\', To>, alt<lit<From>, Alts...>, Parent>
            : lexer_char_class<S, N, values<char>,
                               alt<typename internal::underscore_range<From, To>::rule, Alts...>,
                               Parent>
            {
                static_assert(From <= To,
                              "start of character class range must be less than or equal to end");
            };
            
            template<char const *S, size_t N, typename...Alt, typename...Rules, typename Parent>
            struct lexer_char_class<S, N, values<char, ']'>, alt<Alt...>,
                                    lexer_parent<types<Rules...>, Parent>>
            : lexer_rule<S, N, values<char>, types<alt<Alt...>, Rules...>, Parent>
            {};
        }

        template<char const *S, size_t N, typename...Rules, typename Parent>
        struct lexer_rule<S, N, values<char, '['>, types<Rules...>, Parent>
        : internal::lexer_char_class<S, N, values<char>, alt<>,
                                     internal::lexer_parent<types<Rules...>, Parent>>
        {};

        template<char const *S, size_t N, typename...Rules, typename Parent>
        struct lexer_rule<S, N, values<char, '|'>, types<Rules...>, Parent>
        {
            using rule = alt<typename internal::make_rule<seq<>, Rules...>::rule,
                             typename lexer_rule<S, N>::rule>;
        };
    }
}

#endif
