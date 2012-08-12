//
//  misc.hpp
//  shit
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef shit_misc_hpp
#define shit_misc_hpp

#define _S_CAT(a, b) a##b
#define S_CAT(a, b) _S_CAT(a,b)

namespace shit {
    template<typename T>
    using type = T;
}

#endif
