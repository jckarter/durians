//
//  reftype_test.h
//  shit
//
//  Created by Joe Groff on 8/14/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef __shit__reftype_test__
#define __shit__reftype_test__

#include <shit/reftype.hpp>

namespace shit_test {
    struct reftype_test : shit::reftype<reftype_test> {
        S_DECLARE_REFTYPE()
        
        S_DECLARE_MAKE()
        S_DECLARE_MAKE(int)

        S_DECLARE_READER(foo, int)
        
        bool saved();
    };
    
    struct reftype_test::mut : reftype_test {
        S_DECLARE_WRITER(foo, int)
        
        void save();
        void restore();
    };
    
}

#endif /* defined(__shit__reftype_test__) */
