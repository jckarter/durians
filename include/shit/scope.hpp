//
//  scope.hpp
//  shit
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef shit_scope_hpp
#define shit_scope_hpp

#include "misc.hpp"
#include <exception>

namespace shit {
    template<typename F>
    struct scope_exit {
        F f;
        
        ~scope_exit() { f(); }
    };
    
    template<typename F>
    scope_exit<F> make_scope_exit(F &&f) { return {f}; }
    
#define SCOPE_EXIT(block) auto S_CAT(Q_scope_, __LINE__) = \
    ::shit::make_scope_exit([&]{ block })
#define SCOPE_FAILURE(block) auto S_CAT(Q_scope_, __LINE__) = \
    ::shit::make_scope_exit([&]{ if (::std::uncaught_exception()) { block } })
#define SCOPE_SUCCESS(block) auto S_CAT(Q_scope_, __LINE__) = \
    ::shit::make_scope_exit([&]{ if (!::std::uncaught_exception()) { block } })
}

#endif
