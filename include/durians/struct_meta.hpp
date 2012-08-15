//
//  struct_meta.hpp
//  durians
//
//  Created by Joe Groff on 7/23/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef durians_struct_meta_hpp
#define durians_struct_meta_hpp

#include <durians/misc.hpp>
#include <tuple>
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

namespace durians {
    
    namespace internal {
        template<size_t N>
        struct element_t {};
    }
    
#define META_STRUCT_FIELD(name, ...) \
    ::durians::type<__VA_ARGS__> name;

#define META_STRUCT_APPLY_FIELD_METADATA(name, ...) \
    f(#name, sizeof(__VA_ARGS__), offsetof(self_type, name), \
      Trait<__VA_ARGS__>::value());

#define META_STRUCT_APPLY_FIELD(name, ...) \
    f(#name, instance.name);

#define META_STRUCT_PLUS_ONE(...) +1
    
#define META_STRUCT_GETTER(name, ...) \
    static ::durians::type<__VA_ARGS__> &get(self_type &Q_x, ::durians::internal::element_t<size_t(aggregate_element_index::name)>) { return Q_x.name; } \
    static ::durians::type<__VA_ARGS__> const &get(self_type const &Q_x, ::durians::internal::element_t<size_t(aggregate_element_index::name)>) { return Q_x.name; } \
    static ::durians::type<__VA_ARGS__> &&get(self_type &&Q_x, ::durians::internal::element_t<size_t(aggregate_element_index::name)>) { return ::std::move(Q_x.name); }

#define META_STRUCT_TYPE_LIST(name, ...) __VA_ARGS__,
#define META_STRUCT_NAME_LIST(name, ...) name,

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
            static constexpr ::std::size_t aggregate_size() { return 0 META_FIELDS_##NAME(META_STRUCT_PLUS_ONE); } \
            template<size_t N> \
            using aggregate_element = ::durians::type_at<N, META_FIELDS_##NAME(META_STRUCT_TYPE_LIST) void>; \
            enum class aggregate_element_index : size_t { META_FIELDS_##NAME(META_STRUCT_NAME_LIST) }; \
            META_FIELDS_##NAME(META_STRUCT_GETTER) \
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
    
    template<typename T, typename If = void>
    struct is_aggregate : std::integral_constant<bool, false> {};
    template<typename...T>
    struct is_aggregate<std::tuple<T...>> : std::integral_constant<bool, true> {};
    template<typename T, typename U>
    struct is_aggregate<std::pair<T, U>> : std::integral_constant<bool, true> {};
    template<typename T, size_t N>
    struct is_aggregate<std::array<T, N>> : std::integral_constant<bool, true> {};
    template<typename T>
    struct is_aggregate<T, typename std::enable_if<std::is_same<typename T::Q_struct_traits::self_type, T>::value>::type>
    : std::integral_constant<bool, true> {};
    
    template<typename T, typename If = void>
    struct aggregate_size : std::tuple_size<T> {};
    template<size_t N, typename T, typename If = void>
    struct aggregate_element : std::tuple_element<N, T> {};
    
    template<typename T>
    struct aggregate_size<T, typename std::enable_if<std::is_same<typename T::Q_struct_traits::self_type, T>::value>::type>
    : std::integral_constant<size_t, T::Q_struct_traits::aggregate_size()>
    {};
    
    template<size_t N, typename T>
    struct aggregate_element<N, T, typename std::enable_if<std::is_same<typename T::Q_struct_traits::self_type, T>::value>::type> {
        using type = typename T::Q_struct_traits::template aggregate_element<N>;
    };
    
    template<typename T> struct aggregate_size<T&> : aggregate_size<T> {};
    template<typename T> struct aggregate_size<T&&> : aggregate_size<T> {};
    template<typename T> struct aggregate_size<T const> : aggregate_size<T> {};
    template<typename T> struct aggregate_size<T volatile> : aggregate_size<T> {};

    template<size_t N, typename T> struct aggregate_element<N,T&> : aggregate_element<N, T> {};
    template<size_t N, typename T> struct aggregate_element<N,T&&> : aggregate_element<N, T> {};
    template<size_t N, typename T> struct aggregate_element<N,T const> : aggregate_element<N, T> {};
    template<size_t N, typename T> struct aggregate_element<N,T volatile> : aggregate_element<N, T> {};
    
    template<size_t N, typename T>
    inline typename aggregate_element<N, T>::type &
    get(T &agg, typename T::Q_struct_traits* = nullptr)
    {
        return T::Q_struct_traits::get(agg, internal::element_t<N>());
    }
    template<size_t N, typename T>
    inline typename aggregate_element<N, T>::type const &
    get(T const &agg, typename T::Q_struct_traits* = nullptr)
    {
        return T::Q_struct_traits::get(agg, internal::element_t<N>());
    }
    template<size_t N, typename T>
    inline typename aggregate_element<N, T>::type &&
    get(T &&agg, typename T::Q_struct_traits* = nullptr)
    {
        return T::Q_struct_traits::get(std::move(agg), internal::element_t<N>());
    }
    
    
}

#endif
