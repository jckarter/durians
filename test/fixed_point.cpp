//
//  fixed_point.cpp
//  durians
//
//  Created by Joe Groff on 8/30/12.
//  Copyright (c) 2012 Durian Software. All rights reserved.
//

#include <durians/fixed_point.hpp>
#undef NDEBUG
#include <cassert>
#include <type_traits>

using namespace durians;
using fixed_16_16 = fixed_bin<32, 16>;
using fixed_32_32 = fixed_bin<64, 32>;

using fixed_cents = fixed_dec<64, 2>;

fixed_32_32 mul(fixed_32_32 a, fixed_32_32 b)
{
    return a * b;
}

template<typename T, T V>
struct barf;

namespace {
    template<typename Fixed>
    void test_fixed_type()
    {
        static_assert(std::is_trivial<Fixed>::value,
                      "fixed types should be trivial");
        static_assert(std::is_standard_layout<Fixed>::value,
                      "fixed types should be standard-layout");
        static_assert(sizeof(Fixed) == sizeof(typename Fixed::value_type),
                      "fixed type should have same size as underlying type");

        assert(Fixed(2) + Fixed(2) == Fixed(4));
        assert(Fixed(2) * Fixed(2) == Fixed(4));
        assert(Fixed(4) / Fixed(2) == Fixed(2));
        assert(Fixed(3) / Fixed(2) == Fixed(1.5));
        assert(Fixed(3) % Fixed(2) == Fixed(1));
        assert(Fixed(3) % Fixed(1.5) == Fixed(0));
        assert(Fixed(3) % Fixed(1.5) == Fixed(0));
        assert(Fixed(3.25) % Fixed(1.5) == Fixed(0.25));

        assert(Fixed(2).recip() == Fixed(0.5));
        assert(Fixed(-2).recip() == Fixed(-0.5));

        assert(Fixed(1.5).trunc() == 1);
        assert(Fixed(-1.5).trunc() == -1);
        assert(double(Fixed(1.5)) == 1.5);
        assert(double(Fixed(-1.5)) == -1.5);
        
        static_assert(Fixed(2) + Fixed(2) == Fixed(4), "");
        static_assert(Fixed(2) * Fixed(2) == Fixed(4), "");
        static_assert(Fixed(4) / Fixed(2) == Fixed(2), "");
        static_assert(Fixed(3) / Fixed(2) == Fixed(1, Fixed::denominator/2), "");
        static_assert(Fixed(3) % Fixed(2) == Fixed(1), "");
        
        static_assert(Fixed(2).recip() == Fixed(0, Fixed::denominator/2), "");
        static_assert(Fixed(-2).recip() == Fixed(0, -Fixed::denominator/2), "");
        
        static_assert(Fixed(1, Fixed::denominator/2).trunc() == 1, "");
        static_assert(Fixed(-1, -Fixed::denominator/2).trunc() == -1, "");

        // FIXME xcode 4.4 does not yet support user-defined literal syntax
        static_assert(Fixed::template literal<'0'>() == Fixed(0), "");
        static_assert(Fixed::template literal<'1'>() == Fixed(1), "");
        static_assert(Fixed::template literal<'1', '0'>() == Fixed(10), "");
        static_assert(Fixed::template literal<'1', '0', '0'>() == Fixed(100), "");
        static_assert(Fixed::template literal<'1', '2', '3'>() == Fixed(123), "");
        static_assert(Fixed::template literal<'1', '2', '3', '.'>() == Fixed(123), "");
        static_assert(Fixed::template literal<'1', '2', '3', '.', '0'>() == Fixed(123), "");
        static_assert(Fixed::template literal<'1', '2', '3', '.', '5'>() == Fixed(123, Fixed::denominator/2), "");
        static_assert(Fixed::template literal<'1', '2', '3', '.', '2', '5'>() == Fixed(123, Fixed::denominator/4), "");
    }
}

void test_fixed_point()
{
    test_fixed_type<fixed_16_16>();
    test_fixed_type<fixed_32_32>();
    test_fixed_type<fixed_cents>();
}
