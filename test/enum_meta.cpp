//
//  enum_meta.cpp
//  durians
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <durians/enum_meta.hpp>
#undef NDEBUG
#include <cassert>
#include <cstring>

namespace alpha {
#define META_MEMBERS_Foo(x) \
    x(A, 0) \
    x(B, 1) \
    x(C, 2)
    META_ENUM(Foo)
    
    enum Boo { AA, BB, CC };
}

namespace beta {
#define META_MEMBERS_Bar(x) \
    x(X, 3) \
    x(Y, 4) \
    x(Z, 5)
    META_ENUM_CLASS(Bar)
    
    enum class Far { X, Y, Z };
}

using namespace std;

static_assert(durians::is_meta_enum<alpha::Foo>::value,
              "is_meta_enum is true for META_ENUM");
static_assert(durians::is_meta_enum<beta::Bar>::value,
              "is_meta_enum is true for META_ENUM_CLASS");
static_assert(!durians::is_meta_enum<alpha::Boo>::value,
              "is_meta_enum is false for plain enum");
static_assert(!durians::is_meta_enum<beta::Far>::value,
              "is_meta_enum is false for plain enum class");
static_assert(!durians::is_meta_enum<int>::value,
              "is_meta_enum is false for non-enum");
static_assert(!durians::is_meta_enum<std::false_type>::value,
              "is_meta_enum is false for non-enum");

void test_enum_member_name()
{
    alpha::Foo a = alpha::A, b = alpha::B, c = alpha::C;
    assert(strcmp(durians::enum_member_name(a), "A") == 0);
    assert(strcmp(durians::enum_member_name(b), "B") == 0);
    assert(strcmp(durians::enum_member_name(c), "C") == 0);
    
    beta::Bar x = beta::Bar::X, y = beta::Bar::Y, z = beta::Bar::Z;
    assert(strcmp(durians::enum_member_name(x), "X") == 0);
    assert(strcmp(durians::enum_member_name(y), "Y") == 0);
    assert(strcmp(durians::enum_member_name(z), "Z") == 0);
}

void test_enum_from_name()
{
    assert(durians::enum_from_name<alpha::Foo>("A") == alpha::A);
    assert(durians::enum_from_name<alpha::Foo>("B") == alpha::B);
    assert(durians::enum_from_name<alpha::Foo>("C") == alpha::C);
    assert(durians::enum_from_name<beta::Bar>("X") == beta::Bar::X);
    assert(durians::enum_from_name<beta::Bar>("Y") == beta::Bar::Y);
    assert(durians::enum_from_name<beta::Bar>("Z") == beta::Bar::Z);
}

void test_enum_meta()
{
    test_enum_member_name();
    test_enum_from_name();
}