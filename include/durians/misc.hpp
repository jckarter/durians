//
//  misc.hpp
//  durians
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef durians_misc_hpp
#define durians_misc_hpp

#include <type_traits>
#include <utility>

#define _S_CAT(a, b) a##b
#define S_CAT(a, b) _S_CAT(a,b)

#ifdef NDEBUG
#define S_UNREACHABLE assert(false)
#else
#define S_UNREACHABLE ::std::abort()
#endif

#define S_AUTO(...) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }

namespace durians {
    template<typename T>
    using type = T;
    
    template<typename T, T f>
    struct static_function;
    
    template<typename R, typename...AA, R f(AA...)>
    struct static_function<R(AA...), f>
    {
        R operator()(AA...args) const
        {
            return f(static_cast<AA>(args)...);
        }
    };

#define S_STATIC_FUNCTION(f) ::durians::static_function<decltype(f), f>

    template<typename A, typename B>
    struct only_same;
    template<typename A>
    struct only_same<A, A> : std::true_type {};
    
    // NOTE: these functions are intentionally undefined
    template<typename T>
    T &lvalue();
    template<typename T>
    T &&xvalue();
    template<typename T>
    T rvalue();
}

#endif
