//
//  reftype_test.h
//  durians
//
//  Created by Joe Groff on 8/14/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef __durians__reftype_test__
#define __durians__reftype_test__

#include <durians/reftype.hpp>

namespace durians_test {
    struct reftype_test : durians::reftype<reftype_test> {
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

#endif /* defined(__durians__reftype_test__) */
