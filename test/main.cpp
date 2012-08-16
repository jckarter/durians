//
//  test.cpp
//  durians
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <cstdio>

#define RUN_TEST(x) extern void x(); x();

int main()
{
    RUN_TEST(test_misc);
    RUN_TEST(test_scope);
    RUN_TEST(test_struct_meta);
    RUN_TEST(test_enum_meta);
    RUN_TEST(test_maybe);
    RUN_TEST(test_slice);
    RUN_TEST(test_reftype);
    RUN_TEST(test_io);
    RUN_TEST(test_print);
    RUN_TEST(test_packs);
    std::puts("ok");
}