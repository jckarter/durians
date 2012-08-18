//
//  slice.hpp
//  durians
//
//  Created by Joe Groff on 6/21/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef durians_slice_hpp
#define durians_slice_hpp

#include <array>
#include <cassert>
#include <cstring>
#include <initializer_list>
#include <string>
#include <vector>

namespace durians {
    template<typename> class slice2d_iterator;
    
    namespace internal {
        template<typename, typename> struct is_const_of : std::false_type {};
        template<typename T>
        struct is_const_of<T const, T> : std::true_type {};
    }
    
    template<typename T>
    class basic_slice {
        T *the_data;
        std::size_t the_size;
        
        friend class slice2d_iterator<T>;
    
    public:
        basic_slice() : the_data(nullptr), the_size(0) {}
        
        basic_slice(T &oneElt) : the_data(&oneElt), the_size(1) {}
        
        basic_slice(std::vector<T> &vec)
        : the_data(vec.data()), the_size(vec.size())
        {}
        basic_slice(std::vector<typename std::remove_cv<T>::type> const &vec)
        : the_data(vec.data()), the_size(vec.size())
        {}
        
        template<size_t N>
        basic_slice(std::array<T, N> &arr)
        : the_data(arr.data()), the_size(N)
        {}
        template<size_t N>
        basic_slice(std::array<typename std::remove_cv<T>::type, N> const &arr)
        : the_data(arr.data()), the_size(N)
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
                    typename std::enable_if<internal::is_const_of<T, U>::value>::type * = nullptr)
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
        string_slice(char const (&lit)[N]) : slice<char>(lit, N-1) {}
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
    
    template<typename T>
    class slice2d_iterator {
        basic_slice<T> item;
        size_t the_stride;

    public:
        typedef std::ptrdiff_t difference_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef std::random_access_iterator_tag iterator_category;
        
        slice2d_iterator() = default;
        slice2d_iterator(T *data, std::size_t size, std::size_t stride)
        : item(data, size), the_stride(stride) {}
        slice2d_iterator(T *data, std::size_t size)
        : item(data, size), the_stride(size) {}
        
        template<typename U>
        slice2d_iterator(slice2d_iterator<U> const &s,
                         typename std::enable_if<internal::is_const_of<T, U>::value>::type * = nullptr)
        : item(*s), the_stride(s.stride())
        {}

#define _S_COMPARE_OPERATOR(op) \
    bool operator op(slice2d_iterator const &p) const { return item.data() op p.item.data(); }
        
        _S_COMPARE_OPERATOR(==)
        _S_COMPARE_OPERATOR(!=)
        _S_COMPARE_OPERATOR(<)
        _S_COMPARE_OPERATOR(<=)
        _S_COMPARE_OPERATOR(>=)
        _S_COMPARE_OPERATOR(>)
#undef _S_COMPARE_OPERATOR

#define _S_INCDEC_OPERATOR(op, arop) \
    slice2d_iterator &operator op() { item.the_data arop the_stride; return *this; } \
    slice2d_iterator operator op(int) { \
        slice2d_iterator r = *this; \
        item.the_data arop the_stride; \
        return r; \
    }
        _S_INCDEC_OPERATOR(--, -=)
        _S_INCDEC_OPERATOR(++, +=)
#undef _S_INCDEC_OPERATOR
        
#define _S_MATH_OPERATOR(op) \
    slice2d_iterator operator op(std::ptrdiff_t dist) const { \
        return slice2d_iterator(item.the_data op (dist*std::ptrdiff_t(the_stride)), item.the_size, stride); \
    } \
    slice2d_iterator &operator op##=(std::ptrdiff_t dist) { \
        item.the_data op##= (dist*std::ptrdiff_t(the_stride)); \
        return *this; \
    }
        
        _S_MATH_OPERATOR(+)
        _S_MATH_OPERATOR(-)
#undef _S_MATH_OPERATOR
                
        std::ptrdiff_t operator-(slice2d_iterator const &p) const {
            return (item.data() - p.item.data())/std::ptrdiff_t(the_stride);
        }
        
        basic_slice<T> operator*()  const { return item; }
        basic_slice<T> const *operator->() const { return &item; }
        
        basic_slice<T> operator[](std::ptrdiff_t i) const {
            return {item.data() + i*std::ptrdiff_t(the_stride), item.size()};
        }
        
        T *data() const { return item.data(); }
        std::size_t size() const { return item.size(); }
        std::size_t stride() const { return the_stride; }
    };
    
    template<typename T>
    class basic_slice2d
    {
        T *the_data;
        std::size_t the_width, the_stride, the_height;
    
    public:
        basic_slice2d() : the_data(nullptr), the_width(0), the_stride(0), the_height(0)
        {}
                
        basic_slice2d(basic_slice<T> slice)
        : the_data(slice.data()), the_width(slice.size()), the_stride(slice.size()), the_height(1)
        {}
        
        basic_slice2d(basic_slice<T> slice, std::size_t width)
        : the_data(slice.data()), the_width(width), the_stride(width), the_height(slice.size()/width)
        {}

        basic_slice2d(basic_slice<T> slice, std::size_t width, std::size_t stride)
        : the_data(slice.data()), the_width(width), the_stride(stride), the_height(slice.size()/stride)
        {}

        basic_slice2d(basic_slice<T> slice, std::size_t width, std::size_t stride, std::size_t height)
        : the_data(slice.data()), the_width(width), the_stride(stride), the_height(height)
        {
            assert(height*stride <= slice.size());
        }

        basic_slice2d(T *data, std::size_t width, std::size_t stride, std::size_t height)
        : the_data(data), the_width(width), the_stride(stride), the_height(height)
        {}
        
        template<typename U>
        basic_slice2d(basic_slice2d<U> const &s,
                      typename std::enable_if<internal::is_const_of<T, U>::value>::type * = nullptr)
        : the_data(s.data()), the_width(s.width()), the_stride(s.stride()), the_height(s.height())
        {}

        using iterator = slice2d_iterator<T>;
        using const_iterator = slice2d_iterator<T const>;
        using size_type = std::size_t;
        
        iterator begin() const { return {the_data, the_width, the_stride}; }
        iterator end() const { return {the_data + the_stride*the_height, the_width, the_stride}; }
        const_iterator cbegin() const { return {the_data, the_width, the_stride}; }
        const_iterator cend() const { return {the_data + the_stride*the_height, the_width, the_stride}; }
        bool empty() const { return the_height == 0; }
        std::size_t size() const { return the_height; }
        std::size_t width() const { return the_width; }
        std::size_t stride() const { return the_stride; }
        std::size_t height() const { return the_height; }
        
        basic_slice<T> front() const {
            assert(!empty());
            return {the_data, the_width};
        }
        
        basic_slice<T> back() const {
            assert(!empty());
            return {the_data + the_stride*(the_height-1), the_width};
        }
        
        basic_slice<T> operator[](std::size_t i) const {
            assert(i < the_height);
            return {the_data + the_stride*i, the_width};
        }
    };

    template<typename T>
    using slice2d = basic_slice2d<const T>;
    template<typename T>
    using mutable_slice2d = basic_slice2d<T>;
}

#endif
