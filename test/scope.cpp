//
//  scope.cpp
//  shit
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <shit/scope.hpp>
#undef NDEBUG
#include <cassert>

static void test_scope_success()
{
    bool exit = false, failure = false, success = false;
    try {
        SCOPE_EXIT(exit = true;);
        SCOPE_FAILURE(failure = true;);
        SCOPE_SUCCESS(success = true;);
    } catch (...) {}
    assert(exit);
    assert(!failure);
    assert(success);
}

static void test_scope_failure()
{
    bool exit = false, failure = false, success = false;
    try {
        SCOPE_EXIT(exit = true;);
        SCOPE_FAILURE(failure = true;);
        SCOPE_SUCCESS(success = true;);
        
        throw 0;
    } catch (...) {}
    assert(exit);
    assert(failure);
    assert(!success);
}

void test_scope()
{
    test_scope_success();
    test_scope_failure();
}