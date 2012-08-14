//
//  print.cpp
//  shit
//
//  Created by Joe Groff on 8/14/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <shit/print.hpp>

using namespace shit;
using namespace std;

struct thing {
    int x, y;
};

struct get_x {
    int operator()(thing const &x) const { return x.x; }
};

struct get_y {
    int operator()(thing const &x) const { return x.y; }
};

namespace shit {
    template<>
    struct print_traits<thing> : basic_print_traits<
        chars<'{', 'x', '=', '%', 'd', ',', ' ', 'y', '=', '%', 'd', '}'>,
        get_x,
        get_y> {};
}

void test_print()
{
    using shit::delim::space;
    thing t{44, 55};
    println(1, space, short(2), space, 3l, space, t);
}