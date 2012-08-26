//
//  parse_meta.cpp
//  durians
//
//  Created by Joe Groff on 8/24/12.
//  Copyright (c) 2012 Durian Software. All rights reserved.
//

#include <durians/misc.hpp>
#include <durians/parse_meta.hpp>
#include <durians/packs.hpp>

using namespace durians;
using namespace durians::lex;

static_assert(lit<'a'>::match("a", 0) == 1, "");
static_assert(lit<'a'>::match("b", 0) == -1, "");
static_assert(any::match("a", 0) == 1, "");
static_assert(any::match("b", 0) == 1, "");
static_assert(seq<lit<'a'>, lit<'b'>>::match("ab", 0) == 2, "");
static_assert(seq<lit<'a'>, lit<'b'>>::match("aa", 0) == -1, "");
static_assert(opt<lit<'a'>>::match("a", 0) == 1, "");
static_assert(opt<lit<'a'>>::match("b", 0) == 0, "");
static_assert(opt<lit<'a'>>::match("", 0) == 0, "");
static_assert(opt<seq<lit<'a'>, lit<'b'>>>::match("ab", 0) == 2, "");
static_assert(opt<seq<lit<'a'>, lit<'b'>>>::match("aa", 0) == 0, "");
static_assert(alt<lit<'a'>, lit<'b'>>::match("a", 0) == 1, "");
static_assert(alt<lit<'a'>, lit<'b'>>::match("b", 0) == 1, "");
static_assert(alt<lit<'a'>, lit<'b'>>::match("c", 0) == -1, "");
static_assert(alt<lit<'a'>, lit<'b'>>::match("c", 0) == -1, "");
static_assert(star<lit<'a'>>::match("", 0) == 0, "");
static_assert(star<lit<'a'>>::match("a", 0) == 1, "");
static_assert(star<lit<'a'>>::match("aa", 0) == 2, "");
static_assert(star<lit<'a'>>::match("aaa", 0) == 3, "");
static_assert(star<lit<'a'>>::match("aaaa", 0) == 4, "");
static_assert(star<lit<'a'>>::match("b", 0) == 0, "");
static_assert(star<lit<'a'>>::match("ab", 0) == 1, "");
static_assert(star<lit<'a'>>::match("aab", 0) == 2, "");
static_assert(star<lit<'a'>>::match("aaab", 0) == 3, "");
static_assert(star<lit<'a'>>::match("aaaab", 0) == 4, "");
static_assert(star<seq<lit<'a'>, lit<'b'>>>::match("a", 0) == 0, "");
static_assert(star<seq<lit<'a'>, lit<'b'>>>::match("aa", 0) == 0, "");
static_assert(star<seq<lit<'a'>, lit<'b'>>>::match("ab", 0) == 2, "");
static_assert(star<seq<lit<'a'>, lit<'b'>>>::match("aba", 0) == 2, "");
static_assert(star<seq<lit<'a'>, lit<'b'>>>::match("abaa", 0) == 2, "");
static_assert(star<seq<lit<'a'>, lit<'b'>>>::match("abab", 0) == 4, "");

struct test_rules {
    static constexpr char a[] = "abc";
    static constexpr char b[] = "ab.";
    static constexpr char c[] = "ab*";
    static constexpr char d[] = R"(ab\.)";
    static constexpr char e[] = "[ab]";
    static constexpr char f[] = "A[a-z0-9]Z";
    static constexpr char g[] = R"([\--\]])";
    static constexpr char h[] = "ab|cd";
    static constexpr char i[] = "ab?";
    static constexpr char j[] = "ab+";
    static constexpr char k[] = "(ab)+";
    static constexpr char l[] = "(a(bc)?)+";
    static constexpr char m[] = R"(\(\)\r\n\t\d)";
    static constexpr char n[] = R"([\n-\r])";
};

static_assert(only_same<lexer_rule<test_rules::a>::rule,
                        seq<lit<'a'>, lit<'b'>, lit<'c'>>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::b>::rule,
                        seq<lit<'a'>, lit<'b'>, any>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::c>::rule,
                        seq<lit<'a'>, star<lit<'b'>>>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::d>::rule,
                        seq<lit<'a'>, lit<'b'>, lit<'.'>>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::e>::rule,
                        alt<lit<'b'>, lit<'a'>>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::f>::rule,
                        seq<lit<'A'>, alt<range<'0', '9'>, range<'a', 'z'>>, lit<'Z'>>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::g>::rule,
                        alt<range<'-', ']'>>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::h>::rule,
                        alt<seq<lit<'a'>, lit<'b'>>, seq<lit<'c'>, lit<'d'>>>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::i>::rule,
                        seq<lit<'a'>, opt<lit<'b'>>>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::j>::rule,
                        seq<lit<'a'>, plus<lit<'b'>>>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::k>::rule,
                        plus<seq<lit<'a'>, lit<'b'>>>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::l>::rule,
                        plus<seq<lit<'a'>, opt<seq<lit<'b'>, lit<'c'>>>>>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::m>::rule,
                        seq<lit<'('>, lit<')'>, lit<'\r'>, lit<'\n'>, lit<'\t'>, range<'0','9'>>>::value,
              "");
static_assert(only_same<lexer_rule<test_rules::n>::rule,
                        alt<range<'\n', '\r'>>>::value,
              "");

template<char...C> struct op;
template<char...C> struct number;

struct test_lex
{
    static constexpr char op[] = R"([+\-*])";
    static constexpr char number[] = R"(\d+)";
    static constexpr char ws[] = R"(\s+)";
    
    using lexer = rules<rule_str<number, ::number>,
                        rule_str<ws, discard>,
                        rule_str<op, ::op>
                        >;
};

struct test_lex_strings
{
    static constexpr char a[] = "  123  + 456*- 789  ";
};

static_assert(lexer_rule<test_lex::ws>::rule::match(test_lex_strings::a, 0) == 2, "");
static_assert(lexer_rule<test_lex::number>::rule::match(test_lex_strings::a, 2) == 5, "");
static_assert(lexer_rule<test_lex::ws>::rule::match(test_lex_strings::a, 5) == 7, "");
static_assert(lexer_rule<test_lex::op>::rule::match(test_lex_strings::a, 7) == 8, "");

static_assert(std::is_same<tokenize<test_lex::lexer, test_lex_strings::a>::tokens,
                           types<number<'1','2','3'>, op<'+'>, number<'4','5','6'>, op<'*'>, op<'-'>, number<'7','8','9'>>>::value,
              "");