//
//  misc.hpp
//  shit
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef shit_misc_hpp
#define shit_misc_hpp

#include <type_traits>

#define _S_CAT(a, b) a##b
#define S_CAT(a, b) _S_CAT(a,b)

#ifdef NDEBUG
#define S_UNREACHABLE assert(false);
#else
#define S_UNREACHABLE ::std::abort();
#endif

namespace shit {
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
}

#endif
