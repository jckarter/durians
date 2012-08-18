//
//  static_string.hpp
//  durians
//
//  Created by Joe Groff on 8/16/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#ifndef durians_static_string_hpp
#define durians_static_string_hpp

#include <durians/misc.hpp>
#include <durians/packs.hpp>
#include <algorithm>
#include <array>

namespace durians {
    template<typename T, typename U>
    constexpr typename std::common_type<T, U>::type static_min(T a, U b) { return a < b ? a : b; }
    template<typename T, typename U>
    constexpr typename std::common_type<T, U>::type static_max(T a, U b) { return a > b ? a : b; }
    
    struct static_string_node {
        char const *data;
        size_t size;
    };
    
    struct static_string_iterator {
        static_string_node const *nodes;
        size_t node, i;
        
        constexpr char operator*() const { return nodes[node].data[i]; }
        constexpr static_string_iterator next() const {
            return i+1 == nodes[node].size
            ? static_string_iterator{nodes, node+1, 0}
            : static_string_iterator{nodes, node, i+1};
        }
        
        constexpr bool operator==(static_string_iterator const &o) const
        {
            return nodes == o.nodes && node == o.node && i == o.i;
        }
    };
    
    namespace internal {
        constexpr int compare_static_strings(static_string_iterator begin1,
                                             static_string_iterator end1,
                                             static_string_iterator begin2,
                                             static_string_iterator end2)
        {
            return begin1 == end1
            ? (begin2 == end2 ? 0 : -1)
            : begin2 == end2
            ? 1
            : *begin1 == *begin2
            ? compare_static_strings(begin1.next(), end1, begin2.next(), end2)
            : *begin1 < *begin2
            ? -1 : 1;
        }
    }
    
    template<size_t N = 1>
    struct static_string {
        using node = static_string_node;
        using iterator = static_string_iterator;
        
        node nodes[N];
        
        template<size_t M>
        constexpr static_string(char const (&x)[M])
        : nodes{{x, M-1}}
        {}
        
        template<size_t M1, size_t M2, size_t...MM1, size_t...MM2>
        constexpr static_string(static_string<M1> const &m1, static_string<M2> const &m2,
                                values<size_t, MM1...>, values<size_t, MM2...>)
        : nodes{{m1.nodes[MM1].data, m1.nodes[MM1].size}...,
                {m2.nodes[MM2].data, m2.nodes[MM2].size}...}
        {}
        
        template<size_t M1, size_t M2>
        constexpr static_string(static_string<M1> const &m1, static_string<M2> const &m2)
        : static_string(m1, m2, integers<M1>(), integers<M2>())
        {}

        template<size_t M, size_t...NN>
        constexpr static_string(static_string<N-1> const &m1, char const (&s)[M],
                                values<size_t, NN...>)
        : nodes{{m1.nodes[NN].data, m1.nodes[NN].size}..., node{s, M-1}}
        {}
        
        template<size_t M>
        constexpr static_string(static_string<N-1> const &m1, char const (&s)[M])
        : static_string(m1, s, integers<N-1>())
        {}
        
        template<typename...Node>
        constexpr static_string(node const &n, Node const &...nodes)
        : nodes{{n.data, n.size}, {nodes.data, nodes.size}...}
        {}
        
        template<size_t M>
        constexpr static_string<N+M> operator+(static_string<M> const &o) const
        {
            return {*this, o};
        }

        template<size_t M>
        constexpr static_string<N+1> operator+(char const (&o)[M]) const
        {
            return {*this, o};
        }
        
        template<typename...Node>
        constexpr static_string<N> slice(ptrdiff_t start, ptrdiff_t len,
                                         typename std::enable_if<(sizeof...(Node) < N)>::type*,
                                         Node const &...newnodes) const
        {
            return slice(start - ptrdiff_t(nodes[sizeof...(Node)].size),
                         len,
                         nullptr,
                         newnodes...,
                         node{nodes[sizeof...(Node)].data + static_min(static_max(start, 0),
                                                                       nodes[sizeof...(Node)].size),
                              static_min(static_max(start + len, 0),
                                         nodes[sizeof...(Node)].size) - static_max(start, 0)});
        }

        template<typename...Node>
        constexpr static_string<N> slice(ptrdiff_t start, ptrdiff_t len,
                                         typename std::enable_if<(sizeof...(Node) == N)>::type*,
                                         Node const &...newnodes) const
        {
            return {newnodes...};
        }
        
        constexpr static_string<N> slice(ptrdiff_t start, ptrdiff_t len) const
        {
            return slice(start, len, nullptr);
        }
        
        constexpr size_t size(size_t i = 0, size_t len = 0) const
        {
            return i < N
                ? size(i+1, len + nodes[i].size)
                : len;
        }
        
        constexpr size_t at(size_t i, size_t node = 0) const
        {
            return node < N
            ? (i < nodes[node].size ? nodes[node].data[i] : at(i - nodes[node].size, node + 1))
            : throw "out of bounds";
        }
        
        constexpr char operator[](size_t i) const
        {
            return at(i);
        }
        
        constexpr iterator begin() const { return {nodes, 0, 0}; }
        constexpr iterator end() const { return {nodes, N, 0}; }
                
        template<size_t M>
        constexpr int compare(static_string<M> const &other) const
        {
            return internal::compare_static_strings(begin(), end(), other.begin(), other.end());
        }
        
        template<size_t M>
        constexpr bool operator==(static_string<M> const &o) const { return compare(o) == 0; }
        template<size_t M>
        constexpr bool operator!=(static_string<M> const &o) const { return compare(o) != 0; }
        template<size_t M>
        constexpr bool operator<(static_string<M> const &o) const { return compare(o) < 0; }
        template<size_t M>
        constexpr bool operator<=(static_string<M> const &o) const { return compare(o) <= 0; }
        template<size_t M>
        constexpr bool operator>(static_string<M> const &o) const { return compare(o) > 0; }
        template<size_t M>
        constexpr bool operator>=(static_string<M> const &o) const { return compare(o) >= 0; }
    };
    
    static_assert(static_string<>("asdf").size() == 4, "single string");
    static_assert((static_string<>("asdf") + "hjkl").size() == 8, "catted string");
    static_assert((static_string<>("asdf") + "hjkl").slice(2, 4).size() == 4, "sliced string");
    
    static_assert(static_string<>("asdf")[0] == 'a', "[]");
    static_assert(static_string<>("asdf")[3] == 'f', "[]");
    static_assert((static_string<>("asdf") + "hjkl")[0] == 'a', "[]");
    static_assert((static_string<>("asdf") + "hjkl")[3] == 'f', "[]");
    static_assert((static_string<>("asdf") + "hjkl")[4] == 'h', "[]");
    static_assert((static_string<>("asdf") + "hjkl")[7] == 'l', "[]");
    static_assert((static_string<>("asdf") + "hjkl").slice(2,4)[0] == 'd', "[]");
    static_assert((static_string<>("asdf") + "hjkl").slice(2,4)[3] == 'j', "[]");
    
    static_assert(static_string<>("asdf") == static_string<>("asdf"), "==");
    static_assert(static_string<>("asdf") == static_string<>("as") + "df", "==");
    static_assert(static_string<>("asdf") != static_string<>("asdg"), "!=");
    static_assert(static_string<>("asdf") < static_string<>("asdg"), "!=");
    static_assert(static_string<>("asd") < static_string<>("asdg"), "!=");
    static_assert(static_string<>("asdf") > static_string<>("asde"), "!=");
    static_assert(static_string<>("asdf") > static_string<>("asd"), "!=");
}

#endif
