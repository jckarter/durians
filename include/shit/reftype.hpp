//
//  reftype.hpp
//  shit
//
//  Created by Joe Groff on 8/14/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef shit_reftype_hpp
#define shit_reftype_hpp

#include <shit/misc.hpp>
#include <utility>

namespace shit {
#define S_DECLARE_REFTYPE() \
    struct impl; \
    struct mut; \
    void Q_delete();

#define S_DEFINE_REFTYPE(T) \
    void T::Q_delete() { delete reinterpret_cast<impl*>(Q_pimpl); }

#define S_DECLARE_MAKE(...) \
    static ::shit::owner<Q_type> make(__VA_ARGS__);
    
#define S_DEFINE_MAKE(T, types, names) \
    ::shit::owner<T> T::make types { return ::shit::make_owner<T> names; }
    
#define S_DECLARE_READER(propname, ...) \
    ::shit::type<__VA_ARGS__> propname();
#define S_DECLARE_WRITER(propname, ...) \
    void set_##propname(::shit::type<__VA_ARGS__>);
    
#define S_DEFINE_SIMPLE_READER(T, propname, ...) \
    ::shit::type<__VA_ARGS__> T::propname() { return $.propname; }
#define S_DEFINE_SIMPLE_WRITER(T, propname, ...) \
    void T::mut::set_##propname(::shit::type<__VA_ARGS__> Q_value) { $.propname = Q_value; }

#define S_DEFINE_IMPL_FORWARD(T, ret, func, types, names) \
    ret T::func types { return $.func names; }
    
    template<typename T>
    struct owner;
    
    template<typename T>
    struct reftype {
        using Q_type = T;
        void *Q_pimpl;
        
        explicit operator bool() const { return Q_pimpl != nullptr; }
    };
    
    template<typename T>
    struct reftype_impl {
        using Q_type = T;
    };
    
    namespace internal {
        template<typename T>
        typename T::impl const &reftype_impl(T x)
        {
            return *reinterpret_cast<typename T::impl const *>(x.Q_pimpl);
        }
        
        template<typename T>
        typename T::impl &reftype_impl(typename T::mut x)
        {
            return *reinterpret_cast<typename T::impl *>(x.Q_pimpl);
        }
        
        template<typename T>
        typename T::impl &reftype_impl(typename T::impl &x)
        {
            return x;
        }

        template<typename T>
        typename T::impl const &reftype_impl(typename T::impl const &x)
        {
            return x;
        }
    }
    
#define $ ::shit::internal::reftype_impl<Q_type>(*this)
    
    template<typename T>
    struct owner : T::mut {
        owner() { this->Q_pimpl = nullptr; }
        
        explicit owner(typename T::impl *pimpl) { this->Q_pimpl = pimpl; }
        
        owner(const owner &) = delete;
        void operator=(const owner &) = delete;
        
        owner(owner &&x) { this->Q_pimpl = x.Q_pimpl; x.Q_pimpl = nullptr; }
        owner &operator=(owner &&x) {
            std::swap(this->Q_pimpl, x.Q_pimpl);
            return *this;
        }
        owner &operator=(std::nullptr_t) {
            if (this->Q_pimpl) this->Q_delete();
            this->Q_pimpl = nullptr;
            return *this;
        }
        
        ~owner() { if (this->Q_pimpl) this->Q_delete(); }
    };
    
    template<typename T, typename...AA>
    owner<T> make_owner(AA &&...args)
    {
        return owner<T>(new typename T::impl{std::forward<AA>(args)...});
    }
}

#endif
