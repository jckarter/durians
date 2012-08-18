//
//  static_string.cpp
//  durians
//
//  Created by Joe Groff on 8/16/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <cstring>
#include <durians/static_string.hpp>

#undef NDEBUG
#include <cassert>

using namespace durians;

using s = static_string<>;

static_assert(s("asdf").size() == 4, "single string");
static_assert((s("asdf") + "hjkl").size() == 8, "catted string");
static_assert((s("asdf") + "hjkl").slice(2, 4).size() == 4, "sliced string");

static_assert(s("asdf")[0] == 'a', "[]");
static_assert(s("asdf")[3] == 'f', "[]");
static_assert((s("asdf") + "hjkl")[0] == 'a', "[]");
static_assert((s("asdf") + "hjkl")[3] == 'f', "[]");
static_assert((s("asdf") + "hjkl")[4] == 'h', "[]");
static_assert((s("asdf") + "hjkl")[7] == 'l', "[]");
static_assert((s("asdf") + "hjkl").slice(2,4)[0] == 'd', "[]");
static_assert((s("asdf") + "hjkl").slice(2,4)[3] == 'j', "[]");

static_assert(s("asdf") == s("asdf"), "==");
static_assert(s("asdf") == s("as") + "df", "==");
static_assert(s("asdf") != s("asdg"), "!=");
static_assert(s("asdf") < s("asdg"), "!=");
static_assert(s("asd") < s("asdg"), "!=");
static_assert(s("asdf") > s("asde"), "!=");
static_assert(s("asdf") > s("asd"), "!=");

static_assert(s("asdf").find('a') == 0, "find");
static_assert(s("asdf").find('f') == 3, "find");
static_assert(s("asdf").find('g') == s::npos, "find");
static_assert((s("asdf") + "hjkl").find('a') == 0, "find");
static_assert((s("asdf") + "hjkl").find('f') == 3, "find");
static_assert((s("asdf") + "hjkl").find('h') == 4, "find");
static_assert((s("asdf") + "hjkl").find('l') == 7, "find");

struct hatfields {
    static constexpr auto value = s("hatfields");
};

struct mccoys {
        static constexpr auto value = s("mccoys");
};

struct hatfields_mccoys {
    static constexpr auto value = hatfields::value + " vs " + mccoys::value;
};

void test_static_string()
{
    assert(strcmp(static_string_literal<hatfields>::value, "hatfields") == 0);
    assert(strcmp(static_string_literal<mccoys>::value, "mccoys") == 0);
    assert(strcmp(static_string_literal<hatfields_mccoys>::value, "hatfields vs mccoys") == 0);
}