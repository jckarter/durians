//
//  print.cpp
//  durians
//
//  Created by Joe Groff on 8/14/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <durians/enum_meta.hpp>
#include <durians/struct_meta.hpp>
#include <durians/print.hpp>
#undef NDEBUG
#include <cassert>

using namespace durians;
using namespace std;

struct thing {
    int x;
    double y;

    static constexpr char format_string[] = "{x=%d, y=%g}";
    static constexpr size_t format_arg_count = 2;
    
    int format_arg(format_arg_t<0>) const { return x; }
    double format_arg(format_arg_t<1>) const { return y; }
};

struct bluh {
    static constexpr char format_string[] = "bluh";
    static constexpr size_t format_arg_count = 0;
};

#define META_MEMBERS_meta_enum(x) \
    x(foo, 0) \
    x(bar, 1) \
    x(bas, 2)
META_ENUM_CLASS(meta_enum)

#define META_FIELDS_meta_struct(x) \
    x(foo, int) \
    x(bar, double) \
    x(bas, char)
META_STRUCT(meta_struct)

void test_print()
{
    using durians::delim::space;

    string s = strln(s, 1, space, short(2), space, 3l, space, 4.5);
    
    assert(s == "1 2 3 4.5\n");

    strprint(s);
    assert(s == "1 2 3 4.5\n");
    
    strprintln(s,
               thing{44, 55.5}, space, bluh{});
    
    assert(s == "1 2 3 4.5\n"
                "{x=44, y=55.5} bluh\n");
    
    char const *cstring = "cstring";
    
    strprintln(s,
               "literal", space, cstring, space, string("string"));
    
    assert(s == "1 2 3 4.5\n"
                "{x=44, y=55.5} bluh\n"
                "literal cstring string\n");
    
    strprintln(s,
               tuple<int, double>{42, 63.5}, space,
               array<int, 4>{{1, 2, 3, 4}}, space,
               make_pair(52, 73.5));
    
    assert(s == "1 2 3 4.5\n"
                "{x=44, y=55.5} bluh\n"
                "literal cstring string\n"
                "{42, 63.5} {1, 2, 3, 4} {52, 73.5}\n");
    
    strprintln(s);

    assert(s == "1 2 3 4.5\n"
                "{x=44, y=55.5} bluh\n"
                "literal cstring string\n"
                "{42, 63.5} {1, 2, 3, 4} {52, 73.5}\n"
                "\n");
    
    strprintln(s,
               meta_enum::foo, space, meta_enum::bar, space, meta_enum::bas);
    
    assert(s == "1 2 3 4.5\n"
                "{x=44, y=55.5} bluh\n"
                "literal cstring string\n"
                "{42, 63.5} {1, 2, 3, 4} {52, 73.5}\n"
                "\n"
                "foo bar bas\n");

    strprintln(s, 'x', space, int8_t(0), space, uint8_t(0));

    assert(s == "1 2 3 4.5\n"
                "{x=44, y=55.5} bluh\n"
                "literal cstring string\n"
                "{42, 63.5} {1, 2, 3, 4} {52, 73.5}\n"
                "\n"
                "foo bar bas\n"
                "x 0 0\n");

    strprintln(s,
               meta_struct{7, 6.5, '4'});
    
    assert(s == "1 2 3 4.5\n"
                "{x=44, y=55.5} bluh\n"
                "literal cstring string\n"
                "{42, 63.5} {1, 2, 3, 4} {52, 73.5}\n"
                "\n"
                "foo bar bas\n"
                "x 0 0\n"
                "{7, 6.5, 4}\n");
}