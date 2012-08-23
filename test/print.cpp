//
//  print.cpp
//  durians
//
//  Created by Joe Groff on 8/22/12.
//  Copyright (c) 2012 Durian Software. All rights reserved.
//

#include <durians/print.hpp>
#include <string>

using namespace durians;
using namespace std;

struct some_struct {};

static_assert(is_print_atom<int>::value, "is_print_atom<int>");
static_assert(is_print_atom<char const *>::value, "is_print_atom<int>");
static_assert(is_print_atom<char *>::value, "is_print_atom<int>");
static_assert(!is_print_atom<some_struct>::value, "!is_print_atom<some_struct>");

void test_print()
{
    string s = str('1', 2, "345", octal(067), hex(0xabc), HEX(0xDEF));
    assert(s == "1234567abcDEF");
}
