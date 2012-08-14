//
//  maybe.hpp
//  shit
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef shit_maybe_hpp
#define shit_maybe_hpp

#include <type_traits>
#include <utility>
#include <cassert>
#include <cstddef>

namespace shit {
    template<typename T, typename If = void>
    class maybe;
    
    template<typename T>
    class maybe<T, typename std::enable_if<std::is_trivial<T>::value>::type>
    {
    protected:
        bool present;
        T value;

    public:
        maybe() : present(false) {}
        maybe(T const &x) : present(true), value(x) {}
        
        maybe &operator=(T const &x) {
            present = true;
            value = x;
            return *this;
        }
        
        maybe &operator=(std::nullptr_t) {
            present = false;
            return *this;
        }
        
        explicit operator bool() const { return present; }
        T &operator*() & { assert(present); return value; }
        T const &operator*() const& { assert(present); return value; }
        T &&operator*() && { assert(present); return value; }
        
        T *operator->() { assert(present); return &value; }
        T const *operator->() const { assert(present); return &value; }
        
        bool operator==(maybe const &x) const {
            return (!present && !x.present)
            || (present && x.present && value == x.value);
        }
        
        bool operator!=(maybe const &x) const {
            return !(*this == x);
        }
    };
        
    template<typename T>
    class maybe<T, typename std::enable_if<!std::is_trivial<T>::value
                                           && !std::is_reference<T>::value>::type>
    {
    protected:
        bool present;
        union {
            T value;
        };
        
    public:
        maybe() : present(false) {}
        maybe(T const &x) : present(true), value(x) {}
        maybe(T &&x) : present(true), value(std::move(x)) {}
        
        maybe(maybe const &x) : present(x.present)
        {
            if (present)
                new (&value) T(x.value);
        }
        
        maybe(maybe &&x) : present(x.present)
        {
            if (present)
                new (&value) T(std::move(x.value));
        }
        
        ~maybe() {
            if (present)
                value.~T();
        }
        
        maybe &operator=(maybe const &x) {
            if (this == &x) {
                // nothing
            }
            else if (present && x.present) {
                value = x.value;
            }
            else if (present) {
                value.~T();
                present = false;
            }
            else if (x.present) {
                new (&value) T(x.value);
                present = true;
            }
            return *this;
        }
        
        maybe &operator=(maybe &&x) {
            if (present && x.present) {
                value = std::move(x.value);
            }
            else if (present) {
                present = false;
                value.~T();
            }
            else if (x.present) {
                present = true;
                new (&value) T(std::move(x.value));
            }
            return *this;
        }
        
        maybe &operator=(std::nullptr_t) {
            if (present)
                value.~T();
            present = false;
            return *this;
        }
        
        explicit operator bool() const { return present; }
        T &operator*() & { assert(present); return value; }
        T const &operator*() const& { assert(present); return value; }
        T &&operator*() && { assert(present); return value; }
        
        T *operator->() { assert(present); return &value; }
        T const *operator->() const { assert(present); return &value; }

        bool operator==(maybe const &x) const {
            return (!present && !x.present)
            || (present && x.present && value == x.value);
        }
        
        bool operator!=(maybe const &x) const {
            return !(*this == x);
        }
    };
    
    template<typename T>
    class maybe<T &> {
        T *value;
    public:
        maybe() : value(nullptr) {}
        maybe(T &x) : value(&x) {}
        
        maybe &operator=(T &x) { value = &x; return *this; }
        maybe &operator=(std::nullptr_t) { value = nullptr; return *this; }
        
        explicit operator bool() const { return value != nullptr; }
        T &operator*() const { assert(value); return *value; }
        T *operator->() const { assert(value); return value; }
        
        bool operator==(maybe const &x) const { return value == x.value; }
        bool operator!=(maybe const &x) const { return value != x.value; }
    };
    
    template<typename T>
    maybe<typename std::remove_reference<T>::type>
    just(T &&value) { return {value}; }
    
    template<typename Maybe, typename IfJust, typename IfNothing>
    auto when(Maybe &&m, IfJust &&if_just, IfNothing &&if_nothing)
    -> decltype(m ? if_just(*m) : if_nothing())
    {
        return m ? if_just(*m) : if_nothing();
    }
    
    template<typename Maybe, typename IfJust>
    void when(Maybe &&m, IfJust &&if_just)
    {
        if (m) if_just(*m);
    }
    
    template<typename Maybe, typename Default>
    auto or_else(Maybe &&m, Default &&dflt)
    -> decltype(m ? *m : dflt)
    {
        return m ? *m : dflt;
    }
}

#endif
