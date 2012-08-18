//
//  io.cpp
//  durians
//
//  Created by Joe Groff on 8/16/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <durians/io.hpp>
#undef NDEBUG
#include <cassert>

using namespace std;
using namespace durians;

static void test_unique_file()
{
    unique_file f = open_unique_file("/dev/null", "r");
}

static void test_shared_file()
{
    shared_file f = open_shared_file("/dev/null", "r");
}

static void test_read_write()
{
    char const *data_out = "hello world";
    char data_in[11];
    
    {
        unique_file out = open_unique_file("testfile.txt", "w");
        write_file(out.get(), data_out);
    }
    
    {
        unique_file in = open_unique_file("testfile.txt", "r");
        read_file(in.get(), make_slice(data_in));
    }
    
    assert(equal(begin(data_in), end(data_in), data_out));
}

void test_io()
{
    test_unique_file();
    test_shared_file();
    test_read_write();
}
