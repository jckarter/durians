//
//  enum_meta.hpp
//  durians
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef durians_enum_meta_hpp
#define durians_enum_meta_hpp

#include <durians/misc.hpp>
#include <cstdlib>
#include <cstring>

// Define a macro META_MEMBERS_EnumName to define an enum's members, then use
// META_ENUM[_OF_TYPE](EnumName[, UnderlyingType]) or
// META_ENUM_CLASS[_OF_TYPE](EnumName[, UnderlyingType]) to define the enum type:
//
// #define META_MEMBERS_EnumType(x) \
//     x(foo, 0) \
//     x(bar, 1) \
//     x(bas, 2)
// META_ENUM_CLASS(EnumType)
//
// You can use META_MEMBERS_EnumType(FOO) to apply a macro FOO(name, value) to each
// member name/value pair.

#define META_ENUM_MEMBER(name, value) \
    name = value,

#define META_ENUM(NAME) \
    enum NAME { META_MEMBERS_##NAME(META_ENUM_MEMBER) }; \
    _META_ENUM_TRAITS(NAME)
#define META_ENUM_CLASS(NAME) \
    enum class NAME { META_MEMBERS_##NAME(META_ENUM_MEMBER) }; \
    _META_ENUM_TRAITS(NAME)

#define META_ENUM_OF_TYPE(NAME, UNDERLYING) \
    enum NAME : UNDERLYING { META_MEMBERS_##NAME(META_ENUM_MEMBER) }; \
    _META_ENUM_TRAITS(NAME)
#define META_ENUM_CLASS_OF_TYPE(NAME, UNDERLYING) \
    enum class NAME : UNDERLYING { META_MEMBERS_##NAME(META_ENUM_MEMBER) }; \
    _META_ENUM_TRAITS(NAME)

#define _META_ENUM_MEMBER_VALUE_TO_NAME(name, value) \
    if (x == E::name) \
        return #name;

#define _META_ENUM_MEMBER_NAME_TO_VALUE(name, value) \
    if (::std::strcmp(x, #name) == 0) \
        return E::name;

#define _META_ENUM_TRAITS(NAME) \
    inline char const *Q_enum_member_name(NAME x) { \
        using E = NAME; \
        META_MEMBERS_##NAME(_META_ENUM_MEMBER_VALUE_TO_NAME) \
        return nullptr; \
    } \
    inline NAME Q_enum_from_name(NAME dummy, char const *x) { \
        using E = NAME; \
        META_MEMBERS_##NAME(_META_ENUM_MEMBER_NAME_TO_VALUE) \
        S_UNREACHABLE; \
    }

namespace durians {
    template<typename E>
    inline char const *enum_member_name(E x)
    {
        return Q_enum_member_name(x);
    }
    
    template<typename E>
    inline E enum_from_name(char const *name)
    {
        return Q_enum_from_name(E(), name);
    }
}

#endif
