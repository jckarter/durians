//
//  misc.cpp
//  durians
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <durians/misc.hpp>
#include <cstdlib>
#include <memory>
#undef NDEBUG
#include <cassert>

using namespace std;
using namespace durians;

static_assert(is_empty<static_function<void(void*), free>>::value,
              "static_function should be empty");

static_assert(std::is_same<if_<true, int, float>, int>::value, "if_<true>");
static_assert(std::is_same<if_<false, int, float>, float>::value, "if_<false>");
static_assert(std::is_same<switch_<void, case_<true, int>, case_<false, float>>, int>::value,
              "switch_<>");
static_assert(std::is_same<switch_<void, case_<true, int>, case_<true, float>>, int>::value,
              "switch_<>");
static_assert(std::is_same<switch_<void, case_<false, int>, case_<true, float>>, float>::value,
              "switch_<>");
static_assert(std::is_same<switch_<void, case_<false, int>, case_<false, float>>, void>::value,
              "switch_<>");

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
