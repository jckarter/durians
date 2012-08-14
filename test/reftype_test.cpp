//
//  reftype_test.cpp
//  shit
//
//  Created by Joe Groff on 8/14/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include "reftype_test.hpp"
#include <cstdio>
#include <vector>

namespace shit_test {
    using namespace shit;
    using namespace std;
    
    struct reftype_test::impl : reftype_impl<reftype_test> {
        int foo;
        vector<int> stack;
        
        impl() : foo(0) {}
        impl(int foo) : foo(foo), stack() {}
        
        bool saved() const { return !stack.empty(); }
        void save() { stack.push_back(foo); }
        void restore() { if (saved()) { foo = stack.back(); stack.pop_back(); } }
    };
    
    S_DEFINE_REFTYPE(reftype_test)
    
    S_DEFINE_MAKE(reftype_test, (), ())
    S_DEFINE_MAKE(reftype_test, (int x), (x))
    
    S_DEFINE_SIMPLE_READER(reftype_test, foo, int)
    S_DEFINE_SIMPLE_WRITER(reftype_test, foo, int)
    
    S_DEFINE_IMPL_FORWARD(reftype_test::mut, void, save, (), ())
    S_DEFINE_IMPL_FORWARD(reftype_test::mut, void, restore, (), ())
    S_DEFINE_IMPL_FORWARD(reftype_test, bool, saved, (), ())
}