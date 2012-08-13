//
//  slice.hpp
//  shit
//
//  Created by Joe Groff on 6/21/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef shit_slice_hpp
#define shit_slice_hpp

#include <cassert>
#include <cstring>
#include <initializer_list>
#include <string>
#include <vector>

namespace shit {
    template<typename T>
    class basic_slice {
        T *the_data;
        std::size_t the_size;
    
    public:
        basic_slice() : the_data(nullptr), the_size(0) {}
        
        basic_slice(T &oneElt) : the_data(&oneElt), the_size(1) {}
        
        basic_slice(std::vector<T> &vec)
        : the_data(vec.data()), the_size(vec.size())
        {}
        basic_slice(std::vector<typename std::remove_cv<T>::type> const &vec)
        : the_data(vec.data()), the_size(vec.size())
        {}
        
        template<std::size_t N>
        basic_slice(T (&arr)[N])
        : the_data(&arr[0]), the_size(N)
        {}
        
        basic_slice(std::initializer_list<typename std::remove_cv<T>::type> i)
        : the_data(i.begin()), the_size(i.size())
        {}
        
        // slice(SmallVectorImpl)
        
        basic_slice(T *begin, T *end)
        : the_data(begin), the_size(end - begin)
        {}
        
        basic_slice(T *data, std::size_t size)
        : the_data(data), the_size(size)
        {}
        
        basic_slice(basic_slice const &) = default;
        basic_slice(basic_slice &&) = default;
        basic_slice &operator=(basic_slice const &) = default;
        basic_slice &operator=(basic_slice &&) = default;
        
        template<typename U>
        basic_slice(basic_slice<U> const &s,
                    typename std::enable_if<std::is_const<T>::value
                                            && std::is_same<U, typename std::remove_const<T>::type>::value>::type * = nullptr)
        : the_data(s.data()), the_size(s.size())
        {}
        
        typedef T *iterator;
        typedef T const *const_iterator;
        typedef std::size_t size_type;
        
        iterator begin() const { return the_data; }
        iterator end() const { return the_data + the_size; }
        const_iterator cbegin() const { return the_data; }
        const_iterator cend() const { return the_data + the_size; }
        
        bool empty() const { return the_size == 0; }
        std::size_t size() const { return the_size; }
        
        T *data() const { return the_data; }
        
        T &front() const {
            assert(!empty());
            return the_data[0];
        }
        
        T &back() const {
            assert(!empty());
            return the_data[the_size - 1];
        }
        
        T &operator[](std::size_t i) const {
            assert(i < the_size);
            return the_data[i];
        }
        
        bool equals(basic_slice other) const {
            if (the_size != other.the_size)
                return false;
            if (the_data == other.the_data)
                return true;
            for (std::size_t i = 0; i < the_size; ++i)
                if ((*this)[i] != other[i])
                    return false;
            return true;
        }
        // slice
    };
    
    template<typename T>
    using slice = basic_slice<const T>;
    template<typename T>
    using mutable_slice = basic_slice<T>;
    
    template<typename T>
    slice<T> make_slice(T const &oneElt) { return slice<T>(oneElt); }
    template<typename T>
    slice<T> make_slice(std::vector<T> const &vec) { return slice<T>(vec); }
    template<typename T, std::size_t N>
    slice<T> make_slice(T const (&arr)[N]) { return slice<T>(arr); }
    template<typename T>
    slice<T> make_slice(T const *begin, T const *end) { return slice<T>(begin, end); }
    template<typename T>
    slice<T> make_slice(T const *data, std::size_t size) { return slice<T>(data, size); }
    template<typename T>
    slice<T> make_slice(std::initializer_list<T> *list) { return slice<T>(list); }

    template<typename T>
    mutable_slice<T> make_slice(T &oneElt) { return mutable_slice<T>(oneElt); }
    template<typename T>
    mutable_slice<T> make_slice(std::vector<T> &vec) { return mutable_slice<T>(vec); }
    template<typename T, std::size_t N>
    mutable_slice<T> make_slice(T (&arr)[N]) { return mutable_slice<T>(arr); }
    template<typename T>
    mutable_slice<T> make_slice(T *begin, T *end) { return mutable_slice<T>(begin, end); }
    template<typename T>
    mutable_slice<T> make_slice(T *data, std::size_t size) { return mutable_slice<T>(data, size); }
    
    struct string_slice : slice<char>
    {
        static constexpr std::size_t npos = std::size_t(-1);
        
        string_slice() : slice<char>() {}
        string_slice(char const *data, std::size_t size) : slice<char>(data, size) {
            assert(data || size == 0);
        }
        string_slice(char const *c_str) : slice<char>((assert(c_str), c_str), strlen(c_str)) {}
        template<std::size_t N>
        string_slice(char const (&lit)[N]) : slice<char>(lit, N) {}
        string_slice(std::string const &s) : slice<char>(s.data(), s.size()) {}
        
        bool equals(string_slice other) const {
            if (size() != other.size())
                return false;
            if (size() == 0)
                return true;
            return memcmp(data(), other.data(), size()) == 0;
        }
        
        int compare(string_slice other) const {
            std::size_t sz = std::min(size(), other.size());
            int cmp = size() == 0 ? 0 : memcmp(data(), other.data(), sz);
            if (cmp != 0)
                return cmp;
            if (size() == other.size())
                return 0;
            if (size() < other.size())
                return -1;
            return 1;
        }
    };
    
    inline bool operator==(string_slice a, string_slice b) { return a.equals(b); }
    inline bool operator!=(string_slice a, string_slice b) { return !a.equals(b); }
    inline bool operator<=(string_slice a, string_slice b) { return a.compare(b) <= 0; }
    inline bool operator< (string_slice a, string_slice b) { return a.compare(b) <  0; }
    inline bool operator>=(string_slice a, string_slice b) { return a.compare(b) >= 0; }
    inline bool operator> (string_slice a, string_slice b) { return a.compare(b) >  0; }
    
    /*
    
    template<typename OpaqueT, typename T, T Deref(OpaqueT *, std::size_t)>
    struct OpaqueIterator {
        OpaqueT *ptr;
        std::size_t isize;

        typedef std::ptrdiff_t difference_type;
        typedef T value_type;
        typedef value_type *pointer;
        typedef value_type &reference;
        typedef std::random_access_iterator_tag iterator_category;

        OpaqueIterator() = default;
        OpaqueIterator(OpaqueT *ptr, std::size_t size)
        : ptr(ptr), isize(size)
        {}

#define _MEGA_COMPARE_OPERATOR(op) \
    bool operator op(OpaqueIterator const &p) const { return ptr op p.ptr; }

        _MEGA_COMPARE_OPERATOR(==)
        _MEGA_COMPARE_OPERATOR(!=)
        _MEGA_COMPARE_OPERATOR(<)
        _MEGA_COMPARE_OPERATOR(<=)
        _MEGA_COMPARE_OPERATOR(>=)
        _MEGA_COMPARE_OPERATOR(>)
#undef _MEGA_COMPARE_OPERATOR

#define _MEGA_INCDEC_OPERATOR(op, arop) \
    OpaqueIterator &operator op() { ptr arop isize; return *this; } \
    OpaqueIterator operator op(int) { \
        OpaqueIterator r = *this; \
        ptr arop isize; \
        return r; \
    }

        _MEGA_INCDEC_OPERATOR(--, -=)
        _MEGA_INCDEC_OPERATOR(++, +=)
#undef _MEGA_INCDEC_OPERATOR

#define _MEGA_MATH_OPERATOR(op) \
    OpaqueIterator operator op(std::ptrdiff_t dist) const { \
        return OpaqueIterator(ptr op (dist*isize), isize); \
    }

        _MEGA_MATH_OPERATOR(+)
        _MEGA_MATH_OPERATOR(-)
#undef _MEGA_MATH_OPERATOR

#define _MEGA_COMPOUND_OPERATOR(op) \
    OpaqueIterator &operator op(std::ptrdiff_t dist) { \
        ptr op (dist*isize); \
        return *this; \
    }

        _MEGA_COMPOUND_OPERATOR(+=)
        _MEGA_COMPOUND_OPERATOR(-=)
#undef _MEGA_COMPOUND_OPERATOR

        std::ptrdiff_t operator-(OpaqueIterator const &p) const { return ptr - p.ptr; }

        T operator*()  const { return Deref(ptr, isize); }
        DerefWrapper<T> operator->() const { return DerefWrapper<T>(Deref(ptr, isize)); }
        T operator[](std::ptrdiff_t i) const {
            return Deref(ptr + i*isize, isize);
        }
    };

    template<typename OpaqueT, typename T, T Deref(OpaqueT *, std::size_t)>
    struct OpaqueArrayRef {
        OpaqueT *data;
        std::size_t isize, length;

        OpaqueArrayRef() : data(nullptr), isize(0), length(0) {}
        OpaqueArrayRef(const OpaqueArrayRef &) = default;

        template<typename U>
        OpaqueArrayRef(U &oneElt)
        : data(reinterpret_cast<OpaqueT*>(&oneElt)), isize(sizeof(U)), length(1)
        {}

        template<typename U>
        OpaqueArrayRef(std::vector<U> &vec)
        : data(reinterpret_cast<OpaqueT*>(&vec[0])), isize(sizeof(U)), length(vec.size())
        {}
        // OpaqueArrayRef(U (&)[N])
        // OpaqueArrayRef(SmallVectorImpl)
        // OpaqueArrayRef(U *, U *)
        // OpaqueArrayRef(U *, std::size_t)
        OpaqueArrayRef(OpaqueT *data, std::size_t isize, std::size_t length)
        : data(data), isize(isize), length(length)
        {}

        typedef OpaqueIterator<OpaqueT, T, Deref> iterator;
        typedef std::size_t size_type;

        iterator begin() const { return iterator(data, isize); }
        iterator end() const { return iterator(data + isize*length, isize); }
        bool empty() const { return length == 0; }
        std::size_t size() const { return length; }

        T front() const {
            assert(!empty());
            return Deref(data, isize);
        }

        T back() const {
            assert(!empty());
            return Deref(data + isize*(length-1), isize);
        }

        T operator[](std::size_t i) const {
            assert(i < length);
            return Deref(data + isize*i, isize);
        }

        bool equals(OpaqueArrayRef other) const {
            if (isize != other.isize || length != other.length)
                return false;
            if (data == other.data)
                return true;
            for (std::size_t i = 0; i < length; ++i)
                if ((*this)[i] != other[i])
                    return false;
            return true;
        }
        // slice
    };

    template<typename T>
    struct Array2DRef : OpaqueArrayRef<T const, llvm::ArrayRef<T>, llvm::makeArrayRef<T>> {
        typedef OpaqueArrayRef<T const, llvm::ArrayRef<T>, llvm::makeArrayRef<T>> super;

        Array2DRef() : super(nullptr, 1, 0) {}
        Array2DRef(T const *begin, std::size_t width, std::size_t height)
        : super(begin, width, height)
        {}
        
        Array2DRef(llvm::ArrayRef<T> array, std::size_t span)
        : super(array.data(), span, array.size() / span)
        {}

        Array2DRef(std::vector<T> const &vec, std::size_t span)
        : super(vec.data(), span, vec.size() / span)
        {}
    };
    
    template<typename T>
    llvm::MutableArrayRef<T> makeMutableArrayRef(T *begin, size_t size) {
        return {begin, size};
    }

    template<typename T>
    struct MutableArray2DRef : OpaqueArrayRef<T, llvm::MutableArrayRef<T>, makeMutableArrayRef<T>> {
        typedef OpaqueArrayRef<T, llvm::MutableArrayRef<T>, makeMutableArrayRef<T>> super;
        
        MutableArray2DRef() : super(nullptr, 1, 0) {}
        MutableArray2DRef(T *begin, std::size_t width, std::size_t height)
        : super(begin, width, height)
        {}
        
        MutableArray2DRef(llvm::MutableArrayRef<T> array, std::size_t span)
        : super(array.data(), span, array.size() / span)
        {}
        
        MutableArray2DRef(std::vector<T> &vec, std::size_t span)
        : super(vec.data(), span, vec.size() / span)
        {}
    };
     */
}

#endif
