//
//  fixed_point.hpp
//  durians
//
//  Created by Joe Groff on 8/30/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef durians_fixed_point_hpp
#define durians_fixed_point_hpp

#include <durians/stdint.hpp>
#include <cmath>
#include <type_traits>

namespace durians {
    namespace internal {
        // FIXME binary exponentiation
        constexpr size_t int_pow(size_t b, size_t e) { return e == 0 ? 1 : b * int_pow(b, e - 1); }
        constexpr int16_t wide_mul(int8_t a, int8_t b) { return int16_t(a) * int16_t(b); }
        constexpr int32_t wide_mul(int16_t a, int16_t b) { return int32_t(a) * int32_t(b); }
        constexpr int64_t wide_mul(int32_t a, int32_t b) { return int64_t(a) * int64_t(b); }
        
        // FIXME what if int128 is unsupported?
        constexpr __int128_t wide_mul(int64_t a, int64_t b) { return __int128_t(a) * __int128_t(b); }
        
        template<typename Int>
        constexpr Int mul_fixed(Int a, Int b, ptrdiff_t denom) { return Int(wide_mul(a, b) / denom); }
        template<typename Int>
        constexpr Int div_fixed(Int a, Int b, ptrdiff_t denom) { return Int(wide_mul(a, Int(denom)) / b); }

        template<size_t Integral, size_t Size, size_t Denominator, char...C>
        struct dec_fixed_literal_value;
        
        template<size_t Integral, size_t Size, size_t Denominator, char...C>
        struct dec_fixed_literal_value;
        
        template<size_t Integral, size_t Num, size_t Denom, size_t Exp,
                 size_t Size, size_t Denominator, char...C>
        struct hex_fixed_literal_value;
        
        template<size_t Size, size_t Denominator, char...C>
        struct fixed_literal_value
        : dec_fixed_literal_value<0, Size, Denominator, C...>
        {
        };
        
        template<size_t Size, size_t Denominator, char...C>
        struct fixed_literal_value<Size, Denominator, '-', C...>
        {
            static constexpr signed_t<Size> value
            = -fixed_literal_value<Size, Denominator, C...>::value;
        };
        
        template<size_t Size, size_t Denominator>
        struct fixed_literal_value<Size, Denominator, '0'>
        {
            static constexpr signed_t<Size> value = 0;
        };
        
        template<size_t Size, size_t Denominator, char...C>
        struct fixed_literal_value<Size, Denominator, '0', C...>
        {
            static_assert(Size != Size, "octal fixed literals not yet supported");
        };
        
        template<size_t Size, size_t Denominator, char...C>
        struct fixed_literal_value<Size, Denominator, '0', 'x', C...>
        : hex_fixed_literal_value<0, 0, 0, 0, Size, Denominator, C...>
        {
        };
        
        template<size_t Integral, size_t Num, size_t Denom, size_t Exp, size_t ExpBase,
                 size_t Size, size_t Denominator>
        struct final_fixed_literal_value
        {
            // FIXME exponent
            static constexpr signed_t<Size> value
            = Integral * Denominator + Num * Denominator / Denom ;
        };
        
        template<size_t Integral, size_t Size, size_t Denominator>
        struct dec_fixed_literal_value<Integral, Size, Denominator>
        : final_fixed_literal_value<Integral, 0, 1, 0, 10, Size, Denominator>
        {
        };

        template<size_t Integral, size_t Size, size_t Denominator, char C, char...CC>
        struct dec_fixed_literal_value<Integral, Size, Denominator, C, CC...>
        : dec_fixed_literal_value<Integral*10 + (C - '0'), Size, Denominator, CC...>
        {
            static_assert(C >= '0' && C <= '9', "decimal fixed literal must consist only of decimal digits");
        };
        
        template<size_t Integral, size_t Num, size_t Denom,
                 size_t Size, size_t Denominator, char...CC>
        struct dec_fixed_literal_mantissa;
        
        template<size_t Integral, size_t Size, size_t Denominator, char...CC>
        struct dec_fixed_literal_value<Integral, Size, Denominator, '.', CC...>
        : dec_fixed_literal_mantissa<Integral, 0, 1, Size, Denominator, CC...>
        {
        };
        
        template<size_t Integral, size_t Num, size_t Denom,
                 size_t Size, size_t Denominator>
        struct dec_fixed_literal_mantissa<Integral, Num, Denom, Size, Denominator>
        : final_fixed_literal_value<Integral, Num, Denom, 0, 10, Size, Denominator>
        {
        };
        
        template<size_t Integral, size_t Num, size_t Denom,
                 size_t Size, size_t Denominator, char C, char...CC>
        struct dec_fixed_literal_mantissa<Integral, Num, Denom, Size, Denominator, C, CC...>
        : dec_fixed_literal_mantissa<Integral, Num*10 + (C - '0'), Denom*10, Size, Denominator, CC...>
        {
            static_assert(C >= '0' && C <= '9', "decimal fixed literal must consist only of decimal digits");
        };
        
        template<size_t Integral, size_t Num, size_t Denom,
                 size_t Size, size_t Denominator, char...CC>
        struct dec_fixed_literal_mantissa<Integral, Num, Denom, Size, Denominator, 'e', CC...>
        {
            static_assert(Size != Size, "exponent in fixed literal not yet supported");
        };
        
        template<size_t Integral, size_t Num, size_t Denom,
        size_t Size, size_t Denominator, char...CC>
        struct dec_fixed_literal_mantissa<Integral, Num, Denom, Size, Denominator, 'E', CC...>
        {
            static_assert(Size != Size, "exponent in fixed literal not yet supported");
        };
    }
    
    template<size_t Size, size_t Denominator = (size_t(1) << (Size/2))>
    class fixed
    {
        static_assert(Denominator >= 0, "fixed denominator may not be zero or negative");
        static_assert(Denominator <= PTRDIFF_MAX, "fixed denominator must fit inside a ptrdiff_t");
        
        signed_t<Size> value;
        
        struct precooked_t {};
        constexpr explicit fixed(signed_t<Size> value, precooked_t) : value(value) {}

    public:
        using value_type = signed_t<Size>;
        static constexpr intmax_t denominator = intmax_t(Denominator);
        
        fixed() = default;
        
        template<typename Int>
        constexpr explicit fixed(Int integral,
                                 typename std::enable_if<std::is_integral<Int>::value>::type * = nullptr)
        : value(integral * denominator) {}
        
        explicit fixed(double fp) : value(signed_t<Size>(std::lrint(fp * denominator))) {}
        explicit fixed(long double fp) : value(std::lrintl(fp * denominator)) {}
        constexpr explicit fixed(value_type integral, value_type fraction)
        : value(integral * denominator + fraction) {}
        
        template<char...C>
        static constexpr fixed literal() {
            return fixed(internal::fixed_literal_value<Size, Denominator, C...>::value,
                         precooked_t{});
        }
        
        constexpr explicit operator bool() const { return value != 0; }
        constexpr explicit operator value_type() const { return value / denominator; }
        explicit operator double() const { return double(value) * (1.0/double(denominator)); }
        
        constexpr value_type trunc() const { return value / denominator; }
        
#define _FIXED_COMPARISON_OP(op) \
    constexpr bool operator op(fixed o) const { return value op o.value; }
        _FIXED_COMPARISON_OP(==)
        _FIXED_COMPARISON_OP(!=)
        _FIXED_COMPARISON_OP(<=)
        _FIXED_COMPARISON_OP(>=)
        _FIXED_COMPARISON_OP(<)
        _FIXED_COMPARISON_OP(>)
#undef _FIXED_COMPARISON_OP
        
#define _FIXED_ADDITIVE_OP(op) \
    constexpr fixed operator op(fixed o) const { return fixed(value op o.value, precooked_t{}); } \
    constexpr fixed operator op(value_type o) const { return fixed(value op (o*denominator), precooked_t{}); } \
    friend constexpr fixed operator op(value_type a, fixed b) { \
        return fixed((a*denominator) op b.value, precooked_t{}); \
    } \
    fixed &operator op##op() { value op##= denominator; return *this; } \
    fixed operator op##op(int) { fixed r = *this; r.value op##= denominator; return r; } \
    fixed &operator op##=(fixed o) { value op##= o.value; return *this; } \
    fixed &operator op##=(value_type o) { value op##= o * denominator; return *this; }
        _FIXED_ADDITIVE_OP(+)
        _FIXED_ADDITIVE_OP(-)
#undef _FIXED_ADDITIVE_OP
        
        constexpr fixed operator*(fixed o) const {
            return fixed(internal::mul_fixed(value, o.value, denominator), precooked_t{});
        }
        
        constexpr fixed operator*(value_type o) const {
            return fixed(value * o, precooked_t{});
        }
        
        friend constexpr fixed operator*(value_type a, fixed b) {
            return fixed(a * b.value, precooked_t{});
        }
        
        fixed &operator*=(fixed o) {
            value = multiply_values(value, o.value);
            return *this;
        }

        fixed &operator*=(value_type o) {
            value *= o;
            return *this;
        }
        
        constexpr fixed recip() const { return fixed(denominator/value, precooked_t{}); }
        
        constexpr fixed operator/(fixed o) const {
            return fixed(internal::div_fixed(value, o.value, denominator), precooked_t{});
        }

        constexpr fixed operator/(value_type o) const {
            return fixed(value / o, precooked_t{});
        }

        friend constexpr fixed operator/(value_type a, fixed b) {
            return fixed(multiply_temp_type(a) * denominator * denominator / multiply_temp_type(b),
                         precooked_t{});
        }
        
        fixed &operator/=(fixed o) {
            value = internal::div_fixed(value, o.value, denominator);
            return *this;
        }
        
        fixed &operator/=(value_type o) {
            value /= o;
            return *this;
        }
        
        constexpr fixed operator%(fixed o) const { return fixed(value % o.value, precooked_t{}); }
        constexpr fixed operator%(value_type o) const { return fixed(value % (o*denominator), precooked_t{}); }
        friend constexpr fixed operator%(value_type a, fixed b) {
            return fixed((a*denominator) % b.value, precooked_t{});
        }
        
        fixed &operator%=(fixed o) {
            value %= o.value;
            return *this;
        }
        
        fixed &operator%=(value_type o) {
            value %= o*denominator;
            return *this;
        }
    };
    
    template<size_t Size, size_t DenomSize>
    using fixed_bin = fixed<Size, (size_t(1) << DenomSize)>;
    
    template<size_t Size, size_t DecimalPoints>
    using fixed_dec = fixed<Size, internal::int_pow(10, DecimalPoints)>;

}

#define S_DEFINE_FIXED_LITERAL(Type, LiteralTag) \
    template<char...C> \
    constexpr Type operator"" LiteralTag() { \
        return Type::literal<C...>(); \
    }

#endif
