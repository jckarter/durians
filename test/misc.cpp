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

static_assert(is_type_in<int, char, int>::value,
              "is_type_in should return true if type present");
static_assert(!is_type_in<int, char, float>::value,
              "is_type_in should return false if type not present");

static_assert(index_of_type<int, int, char>::value == 0,
              "type_index should return index into type pack");
static_assert(index_of_type<int, char, int>::value == 1,
              "type_index should return index into type pack");

static_assert(is_any_type<is_integral, int, float, void*>::value,
              "is_any_type should return true if any type fits predicate");
static_assert(is_any_type<is_integral, int, long, void*>::value,
              "is_any_type should return true if any type fits predicate");
static_assert(is_any_type<is_integral, int, long, short>::value,
              "is_any_type should return true if every type fits predicate");
static_assert(!is_any_type<is_integral, float, void*, int()>::value,
              "is_any_type should return false if no type fits predicate");
static_assert(!is_any_type<is_integral>::value,
              "is_any_type should return false for empty type pack");

static_assert(!is_every_type<is_integral, int, float, void*>::value,
              "is_every_type should return true if any type doesn't fit predicate");
static_assert(!is_every_type<is_integral, int, long, void*>::value,
              "is_every_type should return false if any type doesn't fit predicate");
static_assert(is_every_type<is_integral, int, long, short>::value,
              "is_every_type should return true if every type fits predicate");
static_assert(!is_every_type<is_integral, float, void*, int()>::value,
              "is_every_type should return false if no type fits predicate");
static_assert(is_every_type<is_integral>::value,
              "is_every_type should return true for empty type pack");

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
