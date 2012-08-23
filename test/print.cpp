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

struct some_struct_with_print_method {
    template<typename Dest>
    void print(Dest &&dest) const
    {
        print_to(std::forward<Dest>(dest), "hello world");
    }
};

static_assert(is_print_atom<int>::value, "is_print_atom<int>");
static_assert(is_print_atom<char const *>::value, "is_print_atom<int>");
static_assert(is_print_atom<char *>::value, "is_print_atom<int>");
static_assert(is_print_atom<int &>::value, "is_print_atom<int &>");
static_assert(!is_print_atom<some_struct>::value, "!is_print_atom<some_struct>");
static_assert(!is_print_atom<some_struct_with_print_method>::value,
              "!is_print_atom<some_struct_with_print_method>");
static_assert(!has_print_method<int>::value,
              "!has_print_method<int>");
static_assert(has_print_method<some_struct_with_print_method>::value,
              "has_print_method<some_struct_with_print_method>");
static_assert(has_print_method<some_struct_with_print_method &>::value,
              "has_print_method<some_struct_with_print_method &>");
static_assert(has_print_method<some_struct_with_print_method &&>::value,
              "has_print_method<some_struct_with_print_method &&>");
static_assert(has_print_method<some_struct_with_print_method const>::value,
              "has_print_method<some_struct_with_print_method const>");

void test_print()
{
    string s1 = str(L'0', '1', 2, "345", octal(067), L"89", hex(0xabc), HEX(0xDEF));
    assert(s1 == "0123456789abcDEF");
    
    string s2 = str(0, '[', some_struct_with_print_method{}, ']', 0);
    assert(s2 == "0[hello world]0");
}
