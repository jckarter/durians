//
//  slice.cpp
//  durians
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <durians/slice.hpp>
#undef NDEBUG
#include <cassert>

using namespace std;
using namespace durians;

static void test_make_slice()
{
    int marr[4] = {1,2,3,4};
    auto mslice = make_slice(marr);
    static_assert(is_same<decltype(mslice), mutable_slice<int>>::value,
                  "make_slice of mutable array should give mutable_slice");
    assert(mslice.size() == 4);

    int const carr[4] = {1,2,3,4};
    auto cslice = make_slice(carr);
    static_assert(is_same<decltype(cslice), slice<int>>::value,
                  "make_slice of const array should give slice");
    assert(cslice.size() == 4);
    
    vector<int> mvec = {1,2,3};
    auto mvslice = make_slice(mvec);
    static_assert(is_same<decltype(mvslice), mutable_slice<int>>::value,
                  "make_slice of mutable vector should give mutable_slice");
    assert(mvslice.size() == 3);

    vector<int> const cvec = {1,2,3};
    auto cvslice = make_slice(cvec);
    static_assert(is_same<decltype(cvslice), slice<int>>::value,
                  "make_slice of const vector should give slice");
    assert(cvslice.size() == 3);
    
    int *mb = marr+1, *me = marr+3;
    int const *ce = marr+3;
    auto mpslice = make_slice(mb, me);
    auto cpslice = make_slice(mb, ce);
    static_assert(is_same<decltype(mpslice), mutable_slice<int>>::value,
                  "make_slice of mutable pointers should give mutable_slice");
    static_assert(is_same<decltype(cpslice), slice<int>>::value,
                  "make_slice of const pointers should give slice");
    assert(mpslice.size() == 2);
    assert(cpslice.size() == 2);
}

static void test_slice_const_conversion()
{
    int a[4] = {1,2,3,4};
    mutable_slice<int> ms = a;
    slice<int> cs = ms;
    assert(cs.size() == 4);
    assert(cs.data() == ms.data());
    
    int b[5] = {5,6,7,8,9};
    mutable_slice<int> ms2 = b;
    cs = ms2;
    assert(cs.size() == 5);
    assert(cs.data() == ms2.data());
}

static void test_string_slice()
{
    assert(string_slice("asdf") == "asdf");
    char const *cstr = "hjkl";
    assert(string_slice(cstr) == "hjkl");
    assert(string_slice(&cstr[1], 2) == "jk");
    string str = "aoeu";
    assert(string_slice(str) == "aoeu");
}

static void test_string_slice_compare()
{
    assert(string_slice() == string_slice());
    assert(string_slice() == "");
    assert(string_slice("") == string_slice());
    
    assert(string_slice() <= string_slice());
    assert(string_slice() <= "");
    assert(string_slice("") <= string_slice());
    assert(string_slice() <= string_slice("x"));
    assert(string_slice("w") <= string_slice("x"));
    assert(string_slice("x") <= string_slice("x"));
    assert(string_slice("x") <= string_slice("xx"));

    assert(string_slice() < string_slice("x"));
    assert(string_slice("w") < string_slice("x"));
    assert(string_slice("x") < string_slice("xx"));
    
    assert(string_slice("asdf") == "asdf");
}

void test_slice()
{
    test_make_slice();
    test_string_slice();
    test_string_slice_compare();
    test_slice_const_conversion();
}