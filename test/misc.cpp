//
//  misc.cpp
//  shit
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <shit/misc.hpp>
#include <cstdlib>
#include <memory>
#undef NDEBUG
#include <cassert>

using namespace std;
using namespace shit;

static_assert(is_empty<static_function<void(void*), free>>::value,
              "static_function should be empty");

static bool did_free = false;
namespace {
    void free2(void *x)
    {
        free(x);
        did_free = true;
    }
}

static void test_static_function_as_deleter()
{
    did_free = false;
    {
        unique_ptr<char, static_function<void(void*), free2>> buf(strdup("asdf"));
        assert(sizeof(buf) == sizeof(char*));
    }
    assert(did_free);
}

void test_misc()
{
    test_static_function_as_deleter();
}
