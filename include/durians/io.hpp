//
//  io.hpp
//  durians
//
//  Created by Joe Groff on 8/16/12.
//  Copyright (c) 2012 Durian Software. All rights reserved.
//

#ifndef durians_io_hpp
#define durians_io_hpp

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <memory>
#include <durians/misc.hpp>
#include <durians/slice.hpp>

namespace durians {
    inline bool error_return(unsigned long x) { return x == -1; }
    inline bool error_return(void *x) { return x == nullptr; }
    
    template<typename R, typename...A, typename...B>
    inline R eintr_safe(R (*func)(A...), B &&...args)
    {
        static_assert(sizeof...(A) == sizeof...(B), "eintr_safe passed wrong number of arguments");
        R ret;
        do {
            ret = func(static_cast<A>(args)...);
        } while (error_return(ret) && errno == EINTR);
        return ret;
    }
    
    inline std::FILE *open_file(char const *name, char const *mode)
    {
        return eintr_safe(fopen, name, mode);
    }
    
    inline void close_file(std::FILE *f)
    {
        if (f) {
            int err = eintr_safe(fclose, f);
            assert(err == 0);
        }
    }
    
    using unique_file = std::unique_ptr<std::FILE, static_function<void(std::FILE*), close_file>>;
    using shared_file = std::shared_ptr<std::FILE>;
    
    inline shared_file open_shared_file(char const *name, char const *mode)
    {
        return shared_file(open_file(name, mode),
                           static_function<void(std::FILE*), close_file>());
    }
    
    inline unique_file open_unique_file(char const *name, char const *mode)
    {
        return unique_file(open_file(name, mode));
    }
    
    template<typename T>
    inline size_t read_file(std::FILE *f, mutable_slice<T> dest)
    {
        return eintr_safe(fread, dest.data(), sizeof(T), dest.size(), f);
    }

    template<typename T>
    inline size_t write_file(std::FILE *f, slice<T> src)
    {
        return eintr_safe(fwrite, src.data(), sizeof(T), src.size(), f);
    }
    
    inline size_t write_file(std::FILE *f, string_slice src)
    {
        return eintr_safe(fwrite, src.data(), 1, src.size(), f);
    }    
}

#endif
