//
//  reftype.cpp
//  shit
//
//  Created by Joe Groff on 8/14/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include "reftype_test.hpp"
#undef NDEBUG
#include <cassert>

using namespace shit_test;

void test_const_ref(reftype_test ref)
{
    assert(!ref.saved());
    assert(ref.foo() == 42);
}

void test_mutable_ref(reftype_test::mut ref)
{
    assert(!ref.saved());
    ref.save();
    assert(ref.saved());
    ref.set_foo(47);
    assert(ref.foo() == 47);
    ref.restore();
    assert(!ref.saved());
    assert(ref.foo() == 42);
}

void test_reftype()
{
    auto owner1 = reftype_test::make();
    assert(owner1.foo() == 0);
    owner1.set_foo(42);
    test_const_ref(owner1);
    test_mutable_ref(owner1);
    
    auto owner2 = reftype_test::make(42);
    test_const_ref(owner2);
    test_mutable_ref(owner2);
}
