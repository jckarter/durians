//
//  StructMetaTest.cpp
//  Megacanvas
//
//  Created by Joe Groff on 7/23/12.
//  Copyright (c) 2012 Durian Software. All rights reserved.
//

#include <shit/struct_meta.hpp>
#include <array>
#undef NDEBUG
#include <cassert>
#include <vector>

namespace shit { namespace test {
    enum type_token { Int, Float, ArrayInt4 };
    
    template<typename> struct type_token_traits;
    template<> struct type_token_traits<int> {
        static type_token value() { return Int; }
    };
    template<> struct type_token_traits<float> {
        static type_token value() { return Float; }
    };
    template<> struct type_token_traits<std::array<int,4>> {
        static type_token value() { return ArrayInt4; }
    };
    
    static void test_each_field_metadata()
    {
        using namespace std;
#define META_FIELDS_Foo(x) \
    x(foo, int) \
    x(bar, float) \
    x(bas, array<int, 4>)
        META_STRUCT(Foo)
        vector<tuple<char const *, int, int, type_token>> info;
            
        each_field_metadata<Foo, type_token_traits>([&](char const * name,
                                                      int size,
                                                      int offset,
                                                      type_token token) {
            info.push_back(make_tuple(name, size, offset, token));
        });
            
        assert(info.size() == 3);
        assert(info[0] == make_tuple("foo", int(sizeof(int)), int(offsetof(Foo, foo)), Int));
        assert(info[1] == make_tuple("bar", int(sizeof(float)), int(offsetof(Foo, bar)), Float));
        assert(info[2] == make_tuple("bas", int(sizeof(array<int,4>)), int(offsetof(Foo, bas)), ArrayInt4));
    }
    
    static void test_each_field()
    {
#define META_FIELDS_Bar(x) \
    x(foo, int) \
    x(bar, float) \
    x(bas, unsigned char)
        META_STRUCT(Bar)
            
        struct fill {
            int x = 0;
            template<typename T>
            void operator()(char const *name, T &y) {
                y = x++;
            }
        };
            
        Bar bar;
            
        each_field(bar, fill());
        assert(bar.foo == 0);
        assert(bar.bar == 1.0f);
        assert(bar.bas == 2);
    }
}}

void test_struct_meta()
{
    using namespace shit::test;
    test_each_field_metadata();
    test_each_field();
}