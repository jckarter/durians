//
//  enum_meta.cpp
//  shit
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. All rights reserved.
//

#include <shit/enum_meta.hpp>
#undef NDEBUG
#include <cassert>
#include <cstring>

namespace alpha {
#define META_MEMBERS_Foo(x) \
    x(A, 0) \
    x(B, 1) \
    x(C, 2)
    META_ENUM(Foo)
}

namespace beta {
#define META_MEMBERS_Bar(x) \
    x(X, 3) \
    x(Y, 4) \
    x(Z, 5)
    META_ENUM_CLASS(Bar)
}

using namespace std;

void test_enum_member_name()
{
    alpha::Foo a = alpha::A, b = alpha::B, c = alpha::C;
    assert(strcmp(shit::enum_member_name(a), "A") == 0);
    assert(strcmp(shit::enum_member_name(b), "B") == 0);
    assert(strcmp(shit::enum_member_name(c), "C") == 0);
    
    beta::Bar x = beta::Bar::X, y = beta::Bar::Y, z = beta::Bar::Z;
    assert(strcmp(shit::enum_member_name(x), "X") == 0);
    assert(strcmp(shit::enum_member_name(y), "Y") == 0);
    assert(strcmp(shit::enum_member_name(z), "Z") == 0);
}

void test_enum_from_name()
{
    assert(shit::enum_from_name<alpha::Foo>("A") == alpha::A);
    assert(shit::enum_from_name<alpha::Foo>("B") == alpha::B);
    assert(shit::enum_from_name<alpha::Foo>("C") == alpha::C);
    assert(shit::enum_from_name<beta::Bar>("X") == beta::Bar::X);
    assert(shit::enum_from_name<beta::Bar>("Y") == beta::Bar::Y);
    assert(shit::enum_from_name<beta::Bar>("Z") == beta::Bar::Z);
}

void test_enum_meta()
{
    test_enum_member_name();
    test_enum_from_name();
}