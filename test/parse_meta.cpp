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

constexpr size_t _match_size(char const *begin, char const *end) {
    return end ? size_t(end - begin) : ~size_t(0);
}
template<typename Rule>
constexpr size_t match_size(char const *s) { return _match_size(s, Rule::match(s)); }

static_assert(match_size<lit<'a'>>("a") == 1, "");
static_assert(match_size<lit<'a'>>("b") == -1, "");
static_assert(match_size<any>("a") == 1, "");
static_assert(match_size<any>("b") == 1, "");
static_assert(match_size<seq<lit<'a'>, lit<'b'>>>("ab") == 2, "");
static_assert(match_size<seq<lit<'a'>, lit<'b'>>>("aa") == -1, "");
static_assert(match_size<opt<lit<'a'>>>("a") == 1, "");
static_assert(match_size<opt<lit<'a'>>>("b") == 0, "");
static_assert(match_size<opt<lit<'a'>>>("") == 0, "");
static_assert(match_size<opt<seq<lit<'a'>, lit<'b'>>>>("ab") == 2, "");
static_assert(match_size<opt<seq<lit<'a'>, lit<'b'>>>>("aa") == 0, "");
static_assert(match_size<alt<lit<'a'>, lit<'b'>>>("a") == 1, "");
static_assert(match_size<alt<lit<'a'>, lit<'b'>>>("b") == 1, "");
static_assert(match_size<alt<lit<'a'>, lit<'b'>>>("c") == -1, "");
static_assert(match_size<alt<lit<'a'>, lit<'b'>>>("c") == -1, "");
static_assert(match_size<star<lit<'a'>>>("") == 0, "");
static_assert(match_size<star<lit<'a'>>>("a") == 1, "");
static_assert(match_size<star<lit<'a'>>>("aa") == 2, "");
static_assert(match_size<star<lit<'a'>>>("aaa") == 3, "");
static_assert(match_size<star<lit<'a'>>>("aaaa") == 4, "");
static_assert(match_size<star<lit<'a'>>>("b") == 0, "");
static_assert(match_size<star<lit<'a'>>>("ab") == 1, "");
static_assert(match_size<star<lit<'a'>>>("aab") == 2, "");
static_assert(match_size<star<lit<'a'>>>("aaab") == 3, "");
static_assert(match_size<star<lit<'a'>>>("aaaab") == 4, "");
static_assert(match_size<star<seq<lit<'a'>, lit<'b'>>>>("a") == 0, "");
static_assert(match_size<star<seq<lit<'a'>, lit<'b'>>>>("aa") == 0, "");
static_assert(match_size<star<seq<lit<'a'>, lit<'b'>>>>("ab") == 2, "");
static_assert(match_size<star<seq<lit<'a'>, lit<'b'>>>>("aba") == 2, "");
static_assert(match_size<star<seq<lit<'a'>, lit<'b'>>>>("abaa") == 2, "");
static_assert(match_size<star<seq<lit<'a'>, lit<'b'>>>>("abab") == 4, "");

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
