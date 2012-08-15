//
//  struct_meta.cpp
//  durians
//
//  Created by Joe Groff on 7/23/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <durians/struct_meta.hpp>
#include <array>
#undef NDEBUG
#include <cassert>
#include <vector>

namespace durians_test {
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
        
        static_assert(durians::aggregate_size<Foo>::value == 3, "tuple_size of meta struct");
        static_assert(std::is_same<durians::aggregate_element<0, Foo>::type, int>::value, "tuple_element of meta struct");
        static_assert(std::is_same<durians::aggregate_element<1, Foo>::type, float>::value, "tuple_element of meta struct");
        static_assert(std::is_same<durians::aggregate_element<2, Foo>::type, array<int,4>>::value, "tuple_element of meta struct");
        
        vector<tuple<char const *, int, int, type_token>> info;
            
        durians::each_field_metadata<Foo, type_token_traits>([&](char const * name,
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
        const Bar cbar{3, 4.0f, 5};
            
        durians::each_field(bar, fill());
        assert(bar.foo == 0);
        assert(bar.bar == 1.0f);
        assert(bar.bas == 2);
        
        assert(durians::get<0>(bar) == 0);
        assert(durians::get<1>(bar) == 1.0f);
        assert(durians::get<2>(bar) == 2);
        assert(durians::get<0>(cbar) == 3);
        assert(durians::get<1>(cbar) == 4.0f);
        assert(durians::get<2>(cbar) == 5);
        assert(durians::get<0>(Bar{7, 8.0f, 9}) == 7);
        assert(durians::get<1>(Bar{7, 8.0f, 9}) == 8.0f);
        assert(durians::get<2>(Bar{7, 8.0f, 9}) == 9);
    }
    
}

void test_struct_meta()
{
    using namespace durians_test;
    test_each_field_metadata();
    test_each_field();
}