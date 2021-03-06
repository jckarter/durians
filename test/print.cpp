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

#define META_MEMBERS_Foo(x) \
    x(A, 0) \
    x(B, 1) \
    x(C, 2)
META_ENUM_CLASS(Foo)

enum class Bar { X, Y, Z };
enum class Bas : long long { XX, YY, ZZ };

void test_print()
{
    string s1 = str(L'0', '1', 2, "345", octal(067), L"89", hex(0xabc), HEX(0xDEF), delim::nl);
    assert(s1 == "0123456789abcDEF\n");
    
    string s2 = str(0, '[', some_struct_with_print_method{}, ']', 0);
    assert(s2 == "0[hello world]0");

    string s3 = str(internal::string_constant<'%'>());
    assert(s3 == "%");
    
    string s4 = str(Foo::A, Foo::B, Foo::C);
    assert(s4 == "ABC");

    string s5 = str(Bar::X, Bar::Y, Bar::Z);
    assert(s5 == "012");

    string s6 = str(Bas::XX, Bas::YY, Bas::ZZ);
    assert(s6 == "012");
}
