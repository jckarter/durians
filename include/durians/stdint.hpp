//
//  stdint.hpp
//  durians
//
//  Created by Joe Groff on 8/30/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef durians_stdint_hpp
#define durians_stdint_hpp

#include <cstdint>
#include <cstddef>

namespace durians {
    namespace internal {
        template<std::size_t Bits>
        struct int_types;
        
        template<>
        struct int_types<8>
        {
            using signed_t = std::int8_t;
            using unsigned_t = std::uint8_t;
            using signed_least_t = std::int_least8_t;
            using unsigned_least_t = std::uint_least8_t;
            using signed_fast_t = std::int_fast8_t;
            using unsigned_fast_t = std::uint_fast8_t;
        };

        template<>
        struct int_types<16>
        {
            using signed_t = std::int16_t;
            using unsigned_t = std::uint16_t;
            using signed_least_t = std::int_least16_t;
            using unsigned_least_t = std::uint_least16_t;
            using signed_fast_t = std::int_fast16_t;
            using unsigned_fast_t = std::uint_fast16_t;
        };
        
        template<>
        struct int_types<32>
        {
            using signed_t = std::int32_t;
            using unsigned_t = std::uint32_t;
            using signed_least_t = std::int_least32_t;
            using unsigned_least_t = std::uint_least32_t;
            using signed_fast_t = std::int_fast32_t;
            using unsigned_fast_t = std::uint_fast32_t;
        };
        
        template<>
        struct int_types<64>
        {
            using signed_t = std::int64_t;
            using unsigned_t = std::uint64_t;
            using signed_least_t = std::int_least64_t;
            using unsigned_least_t = std::uint_least64_t;
            using signed_fast_t = std::int_fast64_t;
            using unsigned_fast_t = std::uint_fast64_t;
        };
    }
    
    template<size_t Bits>
    using signed_t = typename internal::int_types<Bits>::signed_t;
    
    template<size_t Bits>
    using unsigned_t = typename internal::int_types<Bits>::unsigned_t;
    
    template<size_t Bits>
    using signed_least_t = typename internal::int_types<Bits>::signed_least_t;
    
    template<size_t Bits>
    using unsigned_least_t = typename internal::int_types<Bits>::unsigned_least_t;
    
    template<size_t Bits>
    using signed_fast_t = typename internal::int_types<Bits>::signed_fast_t;
    
    template<size_t Bits>
    using unsigned_fast_t = typename internal::int_types<Bits>::unsigned_fast_t;
}

#endif
