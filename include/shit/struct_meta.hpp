//
//  StructMeta.hpp
//  Megacanvas
//
//  Created by Joe Groff on 7/23/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef shit_struct_meta_hpp
#define shit_struct_meta_hpp

#include "misc.hpp"
#include <type_traits>
#include <utility>

//
// Define a macro META_FIELDS_StructName to define a struct's fields,
// then use META_STRUCT to define the struct:
//
// #define META_FIELDS_StructName(x) \
//     x(foo, int) \
//     x(bar, char) \
//     x(bas, float)
// META_STRUCT(StructName)
//
// You can use META_FIELDS_StructName(FOO)
// to apply a macro FOO(name, type...) over all fields of the struct.
// Note that because the preprocessor does not understand template argument
// lists such as foo<bar,bas> as a single macro argument, the type macro
// argument must be variadic.
//

namespace shit {
    
#define META_STRUCT_FIELD(name, ...) \
    ::shit::type<__VA_ARGS__> name;

#define META_STRUCT_APPLY_FIELD_METADATA(name, ...) \
    f(#name, sizeof(__VA_ARGS__), offsetof(self_type, name), \
      Trait<__VA_ARGS__>::value());

#define META_STRUCT_APPLY_FIELD(name, ...) \
    f(#name, instance.name);

#define META_STRUCT(NAME) \
    struct NAME { \
        META_FIELDS_##NAME(META_STRUCT_FIELD) \
        struct Q_struct_traits { \
            using self_type = NAME; \
            template<template<typename> class Trait, typename Function> \
            static void each_field_metadata(Function &&f) { \
                META_FIELDS_##NAME(META_STRUCT_APPLY_FIELD_METADATA) \
            } \
            template<typename Function> \
            static void each_field(NAME &instance, Function &&f) { \
                META_FIELDS_##NAME(META_STRUCT_APPLY_FIELD) \
            } \
            template<typename Function> \
            static void each_field(NAME const &instance, Function &&f) { \
                META_FIELDS_##NAME(META_STRUCT_APPLY_FIELD) \
            } \
            template<typename Function> \
            static void each_field(NAME &&instance, Function &&f) { \
                META_FIELDS_##NAME(META_STRUCT_APPLY_FIELD) \
            } \
        }; \
    };

    template<typename T>
    using struct_traits = typename T::Q_struct_traits;

    template<typename T, template<typename> class Trait, typename Function>
    void each_field_metadata(Function &&f) {
        struct_traits<T>::template each_field_metadata<Trait>(std::forward<Function>(f));
    }
    template<typename T, typename Function>
    void each_field(T &&instance, Function &&f) {
        struct_traits<typename std::remove_reference<T>::type>
        ::each_field(std::forward<T>(instance), std::forward<Function>(f));
    }
}

#endif
