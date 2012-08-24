# Durian C++11 libraries

This is a collection of MIT-licensed, header-only C++11 libraries. To use, simply add the
`include` directory to your header search path. A test suite is included in the `test`
directory; to build and run, simply compile using a command similar to the following:

```
clang++ -Iinclude -std=c++11 -stdlib=libc++ test/*.cpp
./a.out
```

The test program will print `ok` if successful, or abort with an assertion failure if a test
fails.

These libraries are primarily developed and tested with Xcode 4.4, using Clang and the libc++
standard library.

All symbols described below are defined in the `durians` namespace unless specified otherwise.
Preprocessor macros are of course not namespaced.

* [durians/enum_meta.hpp](#duriansenum_metahpp)
* [durians/io.hpp](#duriansiohpp)
* [durians/maybe.hpp](#duriansmaybehpp)
* [durians/misc.hpp](#duriansmischpp)
* [durians/packs.hpp](#durianspackshpp)
* [durians/print.hpp](#duriansprinthpp)
* [durians/reftype.hpp](#duriansreftypehpp)
* [durians/scope.hpp](#duriansscopehpp)
* [durians/slice.hpp](#duriansslicehpp)
* [durians/struct_meta.hpp](#duriansstruct_metahpp)

## durians/enum_meta.hpp

This header provides preprocessor macros for defining `enum` types with facilities for converting
to and from string representations of the member names. To define an enum, define a macro
named `META_MEMBERS_TypeName(member)` as follows:

```c++
#define META_MEMBERS_TypeName(member) \
    member(FOO, 0) \
    member(BAR, 1) \
    member(BAS, 2)
```

Then define the type by invoking one of the following macros:

* `META_ENUM(TypeName)` to define an `enum` named `TypeName` in the current namespace
* `META_ENUM_CLASS(TypeName)` to define an `enum class`
* `META_ENUM_OF_TYPE(TypeName, underlying_type)` to define an `enum` with underlying type
    `underlying_type`
* `META_ENUM_CLASS_OF_TYPE(TypeName, underlying_type)` to define an `enum class` with underlying
    type `underlying_type`

A type `E` created by the `META_ENUM` macros can be used with the following functions:

* `char const *enum_member_name(E e)` returns the name of the value of `e` as a C string.
* `E enum_member_from_name<E>(char const *name)` returns the value of type `E` named by the C
    string `name`. The program aborts if there is no member of `E` named by `name`.

## durians/io.hpp

This header provides a thin wrapper over `cstdio`. On Unix platforms, these functions will
automatically retry if an `EINTR` error is encountered.

* Type aliases `unique_file` and `shared_file` are provided for `std::unique_ptr<FILE>` and
    `std::shared_ptr<FILE>` with deletion performed by `fclose`.
* `unique_file open_unique_file(char const *name, char const *mode)` and
    `shared_file open_shared_file(char const *name, char const *mode)` construct a
    `unique_file` or `shared_file` using `fopen`.
* `size_t write_file(FILE *f, slice<T> slice)` writes the data referenced by `slice` to the file
    `f` as if by `fwrite(slice.data(), slice.size(), sizeof(T), f)`.
* `size_t read_file(FILE *f, mutable_slice<T> slice)` reads data from `f` into the memory referenced
    by `slice` as if by `fread(slice.data(), slice.size(), sizeof(T), f)`.
* `int flush_file(FILE *f)` flushes any data buffered for `f`, as if by `fflush(f)`.
    
## durians/maybe.hpp

This header defines a class template `maybe<T>` for representing optional values. `maybe<T>` is
laid out like a `struct { bool; T; };`; the `T` is uninitialized if no value is present. `T`
does not need to be movable, copyable, or default-constructible. `maybe<T>` is always
default-constructible, and is movable, copyable, move-assignment or copy-assignable when `T` is.
`maybe<T>` is standard-layout if `T` is standard-layout, and is trivially copyable, movable,
assignable, and destructible if `T` is. `maybe<T>` is however never trivial because it defines
a nontrivial default constructor.

A specialization is provided for `maybe<T&>`, which is represented by a `T*` with `nullptr`
representing no value. `maybe<T&>` is always standard-layout and trivially copyable, movable,
assignable, and destructible.

In addition to the standard value semantics methods, `maybe` types provide the following methods:

* `maybe<T>()` default-constructs a `maybe` containing no value.
* `maybe<T>(T const &)` and `maybe<T>(T &&)` construct a `maybe` containing a value copied or moved
    from a `T`.
* `maybe<T>{args...}` constructs a `maybe` containing the value `T{args...}`, which is emplaced
    inside the `maybe` value.
* `explicit operator bool` returns true if a `maybe` contains a value.
* `operator*` returns a reference
    to the contained value; it is undefined if `operator*` is applied to a `maybe` containing no
    value.
* `operator=(T const &)` and `operator=(T &&)` assign a new value to the `maybe`.
    `operator=(std::nullptr_t)` discards the value inside a `maybe`, if any.
    The method `emplace(args...)` constructs the value `T{args...}` inside the maybe, destroying
    the current value if any.

The following top-level functions are provided for working with `maybe` types:

* `just(x)` constructs a maybe containing the value `x` as if by `maybe<decltype(x)>(x)`.
* `when(m, if_true, if_false)` calls `if_true(*m)` if `m` contains a value or `if_false()` if
    `m` contains no value.
* `or_else(m, dflt)` evaluates to `*m` if `m` contains a value, or `dflt` if `m` contains no value.

## durians/misc.hpp

This header defines the following symbols:

* The macro `S_CAT(a,b)` token–pastes the expansions of `a` and `b`.
* The macro `S_UNREACHABLE;` expands to a statement marking the current code path as unreachable
    by proper code execution.
* The macro `S_AUTO(...)` expands to `-> decltype(...) { return ...; }`. It can be used in place
    of a simple `auto` function body, for example:
    
        template<typename T, typename U> auto sum(T a, U b) S_AUTO(a + b);

* The template type alias `type<T>` is an alias to `T`. This alias can simplify declarations
    involving function and array types, for example:
    
        // traditional definition of signal
        void (*signal(int sig, void (*func)(int)))(int);
        // equivalent definition using type<T> alias
        type<void(int)> *signal(int sig, type<void(int)> *func);

* The template class `static_function<R(A...), func>` is an empty functor type whose `operator()`
    invokes the function `R func(A...)`. This can be used to reference functions in template classes
    without runtime overhead, for example, as a deleter type for a `std::unique_ptr`:
    
        // unique_ptr to a buffer that should be deleted using free()
        unique_ptr<char[], static_function<void(void*), free>> buf = strdup("foo");
        
        // unique_ptr to a FILE that should be deleted using fclose()
        unique_ptr<FILE, static_function<int(FILE*), fclose>> file = fopen("/dev/null", "wb");

    The macro `S_STATIC_FUNCTION(f)` expands to `static_function<decltype(f), f>`; this can be used
    as a shorthand for referencing non-overloaded functions.

* The template class `only_same<A, B>` is like the type trait `std::is_same`, but is only defined
    in the case `only_same<A, A>`. The instantiation errors generated by `only_same<A, B>` can help
    diagnose bugs in template metaprograms.

* The template functions `lvalue<T>()`, `xvalue<T>()`, and `rvalue<T>()` are undefined functions
    declared to return `T &`, `T &&`, and `T`, respectively. These functions may be used in `decltype`
    expressions to determine the type of an expression involving a value of type `T` without
    imposing any requirements on the type, for example `decltype(lvalue<foo const>().foo())`.

## durians/packs.hpp

This header defines types and trait classes for manipulating variadic parameter packs.

* The template class `types<T...>` is an empty class referencing the type pack `T...`.
* The template class `values<T, N...>` is an empty class referencing the value pack `N...` of
    type `T`.
* The trait `is_type_in<T, TT...>::value` is true if `T` is a member of the pack `TT...`.
* The trait `type_index<T, TT...>::value` is equal to the index of `T` within `TT...`.
* The type alias `type_at<N, TT...>` is aliased to the `N`th type within `TT...`.
* The traits `is_any_type<Trait, TT...>::value` and `is_every_type<Trait, TT...>::value` are
    true if `Trait<T>::value` is true for any or every type `T` in `TT...`.
* The type alias `integers<N>` is aliased to `values<size_t, 0, 1, ..., N-1>`.

## durians/print.hpp

This header provides generic interfaces to the standard `printf` family of functions. An
appropriate format string for a set of arguments is generated at compile time and passed down
to `fprintf` or `snprintf` as approprite to the destination type.

* `print_to(dest, args...)` prints each value in `args` to `dest`. `dest` may be one of the following
    types:
    * a `FILE*`, which will be printed to using `fprintf`, or
    * a `mutable_slice<char>`, whose referenced memory will be written to using `snprintf`, or
    * a `std::string &`, which will be appended to using `snprintf` and grown as necessary
* `println_to(dest, args...)` prints each value in `args` to `dest`, followed by a newline.
* `print` and `println` behave like `print_to` and `println_to` with a `dest` argument of `stdout`.

The `print` functions support all types normally supported by `printf`:
* `bool` is printed by `%s`, with an argument `"true"` or `"false"`.
* `char` is printed by `%c`. `wchar_t` is printed by `%lc`.
* Signed integer types from `signed char` up to `intmax_t` are printed by `%d`, `%ld`, `%lld`, or
    `%jd` as appropriate for their size.
* Unsigned integer types from `unsigned char` up to `uintmax_t` are printed by `%u`, `%lu`, `%llu`,
    or `%ju` as appropriate for their size.
* `float` and `double` are printed by `%g`. `long double` is printed by `%Lg`.
* `char const *` is printed by `%s`. `wchar_t const *` is printed by `%ls`.
* Pointer types other than `char*` are printed by `%p`.

User-defined types can add `print` function support by defining a method template:
```c++
template<typename Dest> void print(Dest &&dest) const;
```
Alternately, individual methods may be defined:
```c++
void print(FILE *dest) const;
void print(mutable_slice<char> dest) const;
void print(std::string &dest) const;
```

Some examples of `print`'s behavior:

* `print("hello world")` is equivalent to `fprintf(stdout, "%s", "hello world")`.
* `println("hello ", 123)` is equivalent to `fprintf(stdout, "%s%d\n", "hello ", 123)`.
* `print("hello ", printable{}, 123)`, where `printable` is a type with a `print` method, 
  is equivalent to the following sequence of calls:

        fprintf(stdout, "%s", "hello ");
        printable{}.print(stdout);
        fprintf(stdout, "%d\n", 123);

## durians/reftype.hpp

This header defines macros and templates to help implement the "pimpl" pattern for implementation
hiding. The public type of a reftype is POD and pointer-sized and so can be passed around by value
like a pointer; the contained pointer is opaque to user code. A template `owner<T>` provides
unique ownership semantics to a reftype's implementation; the implementation is allocated by
a static `T::make` method and released when the returned `owner<T>` reference is destroyed.
`owner<T>` is movable but noncopyable, similar to `std::unique_ptr`.

Declare the public interface for a reftype in a header as follows:

```c++
//
// Project/Foo.hpp
//

#pragma once
#include <durians/reftype.hpp>

namespace Project {
    // The type Foo will reference a const implementation type
    struct Foo : durians::reftype<Foo> {
        // S_DECLARE_REFTYPE() provides necessary boilerplate
        S_DECLARE_REFTYPE();
        
        // S_DECLARE_MAKE(...) defines a static function
        //   owner<Foo> make(...)
        // to allocate an instance and return an owning reference to it
        S_DECLARE_MAKE(int bar, float bas);

        // Declare read-only methods
        int bar() const;
        float bas() const;
        size_t bas_version() const;
        float sum() const;
    };
    
    // The type Foo::mut references a mutable implementation type
    struct Foo::mut : Foo {
        // Declare mutator methods
        // NB: mutator methods are still const because *this is just a reference and
        // not itself modified
        
        void set_bar(int newbar) const;
        void set_bas(float newbas) const;
        void update(int newbar, float newbas) const;
    };
}
```

Then define the implementation in a source file as follows:

```c++
//
// Project/Foo.cpp
//

#include "Foo.hpp"

namespace Foo {
    // The type Foo::impl is the private concrete representation of objects referenced
    // by Foo and Foo::mut
    struct Foo::impl : durians::reftype_impl<Foo> {
        int bar;
        float bas;
        size_t bas_version;
        
        impl(int bar, float bas) : bar(bar), bas(bas), bas_version(1) {}
        
        float sum() { return float(bar) + bas; }
        
        void update(int newbar, float newbas) {
            ++bas_version;
            bar = newbar;
            bas = newbas;
        }

        void set_bas(float newbas) { ++bas_version; bas = newbas; }
    };
    
    // Once Foo::impl is defined, the public type Foo can be defined with the following macros
    
    // S_DEFINE_REFTYPE(Foo) defines necessary boilerplate
    S_DEFINE_REFTYPE(Foo);
    
    // S_DEFINE_MAKE(Foo, (args...), (params...)) defines the 'make' static method corresponding
    // to S_DECLARE_MAKE(args...). It will construct the impl using impl(params...)
    S_DEFINE_MAKE(Foo, (int bar, float bas), (bar, bas));
    
    // S_DEFINE_SIMPLE_READER(Foo, name, type) defines the Foo method
    //   type name() const
    // as a simple reader { return impl->name; }
    S_DEFINE_SIMPLE_READER(Foo, bar, int);
    S_DEFINE_SIMPLE_READER(Foo, bas, float);
    S_DEFINE_SIMPLE_READER(Foo, bas_version, size_t);
    
    // S_DEFINE_SIMPLE_WRITER(Foo, name, type) defines the Foo::mut method
    //   void set_name(type x) const
    // as a simple writer { impl->name = x; }
    S_DEFINE_SIMPLE_WRITER(Foo, bar, int);

    // S_DEFINE_IMPL_FORWARD(Foo, type, name, (args...), (params...)) defines the Foo method
    //   type name(args...) const
    // by invoking the method of the same name { return impl->name(params...); }
    S_DEFINE_IMPL_FORWARD(Foo, float, sum, (), ());
    S_DEFINE_IMPL_FORWARD(Foo::mut, void, set_bas, (float x), (x));
    S_DEFINE_IMPL_FORWARD(Foo::mut, void, update, (int newbar, float newbas), (newbar, newbas));
}
```

## durians/scope.hpp

This header defines macros for installing scope guard statements.

* `SCOPE_EXIT(statements);` will cause `statements` to be executed when the current scope is exited
    for any reason, whether by normal control flow or by a thrown exception.
* `SCOPE_SUCCESS(statements);` will cause `statements` to be executed when the current scope is
    exited by normal control flow.
* `SCOPE_FAILURE(statements);` will cause `statements` to be executed when the current scope is
    exited by a thrown exception.
    
For example:

```c++
char const *foo()
{
    char const *r = malloc(100);
    SCOPE_FAILURE(free(r)); // free r if we fail before returning it
    ...
    return r;
}

void bar()
{
    char const *r = malloc(100);
    SCOPE_EXIT(free(r)); // free r in any case
    ...
}

void bas()
{
    auto start = chrono::high_resolution_clock::now();
    SCOPE_SUCCESS(println("bar succeeded in ",
                          chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start),
                          "ns"));
    ...
}
```

## durians/slice.hpp

This header defines template types for slices, which are nonowning references to arrays of objects.
All slice can reference all or part of a C array, `std::array`, `std::vector`, `std::string`, or
other sequence of objects that is contiguous in memory.

### One-dimensional slices

The type `basic_slice<T>` is a reference to an array of `T`. The type alias `slice<T>` is aliased
to `basic_slice<T const>`, and `mutable_slice<T>` to `basic_slice<T>`. Slice types are implicitly
convertible from C arrays, `std::array`, `std::vector`, and `std::initializer_list`. `slice<T>`
is also conditionally convertible from `mutable_slice<T>`. Slice types
are trivially copyable, movable, assignable, and destructible; copying the slice does not copy
the referenced data. Slice types fulfill the requirements of an STL Random Access Sequence using
`T*` as the iterator type.

Slice types provide the following additional constructors:

* `basic_slice<T>(T &value)` constructs a slice of size 1 referencing the single object `value`.
* `basic_slice<T>(T *data, size_t size)` constructs a slice referencing `size` objects starting with
    the one pointed to by `data`.
* `basic_slice<T>(T *begin, T *end)` constructs a slice referencing the objects in the half-open range
    `[begin, end)`.

A function `make_slice` is also provided to construct a `slice<T>` with the type `T` inferred from
the constructor arguments.

### String slices

The type `string_slice` is defined for referring to strings. `string_slice` inherits from `slice<char>`,
and provides additional conversions from `char const *` and `std::string` and comparison operators
for comparing strings. Note that since `string_slice` can refer into the middle of a string, it
cannot be assumed to be null-terminated.

### Two-dimensional slices

The type `basic_slice2d<T>` (with aliases `slice2d<T>` and `mutable_slice2d<T>`) is a reference
to a two-dimensional array. `basic_slice2d<T>` is an STL Random Access Sequence with value type
`basic_slice<T>`; indexing into a `slice2d` gives a `slice` referencing the `i`th row of the array.
A slice may have a stride independent of its width. `basic_slice2d<T>` provides the following
constructors:

* `basic_slice2d(basic_slice<T> slice)` constructs a `basic_slice2d` of height 1 with `slice`
    as its single row.
* `basic_slice2d(basic_slice<T> slice, size_t width)` constructs a `basic_slice2d` of width `width`
    and height `slice.size()/width` over the objects referenced by `slice`.
* `basic_slice2d(basic_slice<T> slice, size_t width, size_t stride)` constructs a `basic_slice2d`
    of width `width`, stride `stride`, and height `slice.size()/stride` over the objects referenced
    by `slice`.
* `basic_slice2d(basic_slice<T> slice, size_t width, size_t stride, size_t height)` constructs a `basic_slice2d`
    of width `width`, stride `stride`, and height `height` over the objects referenced
    by `slice`.
* `basic_slice2d(T *data, size_t width, size_t stride, size_t height)` constructs a `basic_slice2d`
    of width `width`, stride `stride`, and height `height` over the objects starting from the one
    pointed to by `data`.

## durians/struct_meta.hpp

This header provides preprocessor macros for defining `struct` types with metaprogramming support
for the struct's fields. To define a struct, define a macro named `META_FIELDS_TypeName(field)` as
follows:

```c++
#define META_FIELDS_TypeName(field) \
    field(foo, int) \
    field(bar, float) \
    field(bas, char)
```

Then invoke the macro `META_STRUCT(TypeName)` to define a struct named `TypeName` in the current
namespace. A type `T` created by `META_STRUCT` can be used with the following functions:

* `each_field_metadata<T, Trait>(f)` will invoke a function object callable as
    `void f(char const *name, size_t size, size_t offset, decltype(Trait<U>::value()) trait)`
    with the name, `sizeof`, `offsetof`, and type trait of each field in the struct type `T`.
    For example:
    
        #define META_FIELDS_Foo(field) \
            field(foo, int) \
            field(bar, float)
        META_STRUCT(Foo)
        
        struct get_typeid<T> { static char const *value() { return typeid(T).name(); } };
        
        void describe_field(char const *name, size_t size, size_t offset, char const *id)
        {
            printf("field %s, size %zu, offset %zu, typeid %s\n",
                   name, size, offset, id);
        }
        
        int main()
        {
            each_field_metadata<Foo>(describe_field);
        }
        
* `each_field(instance, f)` will invoke a function object `f` with a reference to every field in
    `instance`. `f` must have an `operator()` for every field type in `instance`. For example:
    
        #define META_FIELDS_Foo(field) \
            field(foo, int) \
            field(bar, float)
        META_STRUCT(Foo)

        struct set_to_one {
            template<typename T>
            void operator()(T &x)
            {
                x = 1;
            }
        };
        
        int main()
        {
            Foo foo;
            each_field(foo, set_to_one());
            assert(foo.foo == 1);
            assert(foo.bar == 1.0f);
        }

The trait classes `aggregate_size<T>` and `aggregate_element<N, T>` are also defined. These
are supersets of `std::tuple_size` and `std::tuple_element`; in addition to being defined for
`std::tuple`, `std::pair`, and `std::array`, `aggregate_size` and `aggregate_element` are
also provided for structs defined by `META_STRUCT`. `aggregate_size<T>::value` is equal to
the number of fields in `T`, and `typename aggregate_element<N, T>::type` is aliased to the type
of the `N`th field in `T`. `get<N>(instance)` will give a reference to the `N`th field in `instance`.
The type trait `is_aggregate<T>::value` is true if `aggregate_size`, `aggregate_element`, and
`get` are valid for the type `T`.

## durians/timing.hpp

This header provides a couple of template functions for simple execution time measurement. These
functions use `std::chrono::high_resolution_clock::now()` to measure time.

* `time(D &duration, F &&f)` measures the time taken to execute `f()`, which is written to `duration`.
    `duration` may be of any `std::chrono::duration` type. The result of `f()` is returned.
    If `f()` throws an exception, the time taken to execute `f()` until the exception was thrown
    is written to `duration`, and the exception is rethrown.
* `log_time(char const *name, F &&f)` measures the time taken to execute `f()`, and prints it to
    `stderr` with the label `name`. The result of `f()` is returned. If `f()` throws an exception,
    the time taken to execute `f()` until the exception was thrown is printed, and the exception
    is rethrown.
