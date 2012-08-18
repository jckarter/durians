//
//  timing.hpp
//  durians
//
//  Created by Joe Groff on 8/18/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef durians_timing_hpp
#define durians_timing_hpp

#include <chrono>
#include <cstdio>
#include <durians/scope.hpp>

namespace durians {
    template<typename Duration, typename F>
    inline auto time(Duration &duration, F &&f)
    -> decltype(f())
    {
        using std::chrono::high_resolution_clock;
        using std::chrono::duration_cast;
        auto start = high_resolution_clock::now();
        SCOPE_EXIT(duration = duration_cast<Duration>(high_resolution_clock::now() - start));

        return f();
    }
    
    template<typename F>
    inline auto log_time(char const *name, F &&f)
    -> decltype(f())
    {
        std::chrono::nanoseconds timing;
        SCOPE_SUCCESS(std::fprintf(stderr, "%s succeeded in %zu nanoseconds\n",
                                   name, size_t(timing.count())));
        SCOPE_FAILURE(std::fprintf(stderr, "%s failed in %zu nanoseconds\n",
                                   name, size_t(timing.count())));
        return time(timing, f);
    }
}

#endif
