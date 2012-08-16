//
//  packs.cpp
//  durians
//
//  Created by Joe Groff on 8/16/12.
//  Copyright (c) 2012 Durian Software. All rights reserved.
//

#include <durians/packs.hpp>
#include <type_traits>

using namespace durians;
using namespace std;

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

void test_packs()
{
    
}