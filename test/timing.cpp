//
//  timing.cpp
//  durians
//
//  Created by Joe Groff on 8/18/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <durians/timing.hpp>
#include <thread>
#undef NDEBUG
#include <cassert>

using namespace durians;
using namespace std;

static int test_int;
static auto test_lambda = []() -> int & { return test_int; };
static std::chrono::nanoseconds test_nanos;
static_assert(is_same<int&, decltype(time(test_nanos, test_lambda))>::value, "");

void test_timing()
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-stack-address"
    log_time("test_timing", [&]{
        auto timing = chrono::nanoseconds::zero();
        int x = 12;
        
        int val = time(timing, [&]() -> int {
            for (size_t i = 0; i < 10; ++i)
                this_thread::yield();
            return x;
        });
        assert(timing != chrono::nanoseconds::zero());
        assert(val == 12);
        
        timing = chrono::nanoseconds::zero();
        
        int &ref = time(timing, [&]() -> int& {
            for (size_t i = 0; i < 10; ++i)
                this_thread::yield();
            return x;
        });
        
        assert(timing != chrono::nanoseconds::zero());
        assert(&ref == &x);
    });
#pragma clang pop
}
