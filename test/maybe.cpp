//
//  maybe.cpp
//  durians
//
//  Created by Joe Groff on 8/12/12.
//  Copyright (c) 2012 Durian Software. MIT license.
//

#include <durians/maybe.hpp>
#include <vector>
#undef NDEBUG
#include <cassert>

using namespace std;
using namespace durians;

static_assert(is_trivially_copy_constructible<maybe<int>>::value,
              "maybe should be trivially copyable for trivial type");
static_assert(is_trivially_copy_assignable<maybe<int>>::value,
              "maybe should be trivially copy-assignable for trivial type");
static_assert(is_trivially_move_constructible<maybe<int>>::value,
              "maybe should be trivially movable for trivial type");
static_assert(is_trivially_move_assignable<maybe<int>>::value,
              "maybe should be trivially move-assignable for trivial type");
static_assert(is_trivially_destructible<maybe<int>>::value,
              "maybe should be trivially destructible for trivial type");

static_assert(is_trivially_copy_constructible<maybe<int&>>::value,
              "maybe should be trivially copyable for reference type");
static_assert(is_trivially_copy_assignable<maybe<int&>>::value,
              "maybe should be trivially copy-assignable for reference type");
static_assert(is_trivially_move_constructible<maybe<int&>>::value,
              "maybe should be trivially movable for reference type");
static_assert(is_trivially_move_assignable<maybe<int&>>::value,
              "maybe should be trivially move-assignable for reference type");
static_assert(is_trivially_destructible<maybe<int&>>::value,
              "maybe should be trivially destructible for reference type");

static_assert(is_copy_constructible<maybe<vector<int>>>::value,
              "maybe should be copyable");
static_assert(is_copy_assignable<maybe<vector<int>>>::value,
              "maybe should be copy-assignable");
static_assert(is_move_constructible<maybe<vector<int>>>::value,
              "maybe should be movable");
static_assert(is_move_assignable<maybe<vector<int>>>::value,
              "maybe should be move-assignable");
static_assert(is_destructible<maybe<vector<int>>>::value,
              "maybe should be destructible");

struct blub {
    int x; float y;
    bool operator==(blub const &o) const {
        return x == o.x && y == o.y;
    }
};

static void test_maybe_trivial()
{
    maybe<blub> nothing;
    assert(!nothing);
    
    maybe<blub> just = blub{1, 2.0f};
    assert(just);
    assert((*just == blub{1, 2.0f}));
    assert(just->x == 1);
    assert(just->y == 2.0f);
    
    just = blub{3, 4.0f};
    assert(just);
    assert(just->x == 3);
    assert(just->y == 4.0f);
    
    maybe<blub> just2{5, 6.0f};
    just = just2;
    assert(just);
    assert(just->x == 5);
    assert(just->y == 6.0f);
    
    just = nothing;
    assert(!just);
    
    just = just2;
    assert(just);
    assert(just == just2);
    assert(just != nothing);
    assert(nothing != just);
    assert(just->x == 5);
    assert(just->y == 6.0f);
    
    just = nullptr;
    assert(!just);
    assert(just == nothing);
    
    maybe<blub> just3 = just2;
    assert(just3);
    assert(just3->x == 5);
    assert(just3->y == 6.0f);
    
    maybe<blub> nothing2 = nothing;
    assert(!nothing2);
    
    nothing2 = just3;
    assert(nothing2);
    assert(nothing2->x == 5);
    assert(nothing2->y == 6.0f);
    
    nothing2 = nothing;
    assert(!nothing2);
    
    nothing2 = nothing;
    assert(!nothing2);
}

static void test_maybe_nontrivial()
{
    maybe<vector<int>> nothing;
    assert(!nothing);
    
    maybe<vector<int>> just = vector<int>{1, 2, 3};
    assert(just);
    assert((*just == vector<int>{1,2,3}));
    
    just = vector<int>{4, 5};
    assert(just);
    assert((*just == vector<int>{4, 5}));
    
    maybe<vector<int>> just2{6,7,8};
    assert(just2);
    just = just2;
    assert(just == just2);
    assert(just != nothing);
    assert(nothing != just);
    assert((*just == vector<int>{6,7,8}));
    
    just = nothing;
    assert(!just);
    assert(just == nothing);
    
    just = just2;
    assert(just);
    assert(just == just2);
    
    maybe<vector<int>> just3 = just2;
    assert(just3);
    assert((*just3 == vector<int>{6,7,8}));

    maybe<vector<int>> just4 = move(just3);
    assert(just3->empty());
    assert(just4);
    assert((*just4 == vector<int>{6,7,8}));
    
    maybe<vector<int>> nothing2 = nothing;
    assert(!nothing2);
    
    maybe<vector<int>> nothing3 = move(nothing);
    assert(!nothing3);
    
    nothing2 = just4;
    assert(nothing2);
    assert((*nothing2 == vector<int>{6,7,8}));
    
    nothing2 = nothing3;
    assert(!nothing3);
    
    nothing2 = nothing3;
    assert(!nothing3);
    
    nothing2 = move(just4);
    assert(nothing2);
    assert((*nothing2 == vector<int>{6,7,8}));
    
    nothing2 = move(nothing3);
    assert(!nothing3);
    
    just4 = move(just2);
    assert(just4);
    assert((*just4 == vector<int>{6,7,8}));
    
    just4 = nullptr;
    assert(!just4);
    
    just4 = nullptr;
    assert(!just4);
}

static void test_maybe_reference()
{
    int x, y;
    
    maybe<int&> nothing;
    assert(!nothing);
    
    maybe<int&> just = x;
    assert(just);
    assert(&*just == &x);
    *just = 3;
    assert(x == 3);
    
    just = nothing;
    assert(!just);
    
    just = nothing;
    assert(!just);
    
    just = y;
    assert(just);
    assert(&*just == &y);
    *just = 4;
    assert(y == 4);
}

static void test_just()
{
    maybe<int> x = just(1);
    assert(x);
    assert(*x == 1);
    
    int a = 22;
    maybe<int> y = just(a);
    assert(y);
    assert(*y == 22);
}

static void test_when()
{
    int x;
    
    when(maybe<int>(1),
         [&](int y) {
             x = y;
         },
         [&]() {
             x = -1;
         });
    
    assert(x == 1);
    
    x = when(maybe<int>(),
             [&](int const &y) -> int const& { return y; },
             [&]() { return -1; });
    assert(x == -1);
    
    maybe<vector<int>> v;
    when(v, [](vector<int> &v) { v.push_back(1); });
    assert(!v);

    v = vector<int>{3, 2};
    when(v, [](vector<int> &v) { v.push_back(1); });
    assert((*v == vector<int>{3,2,1}));
}

static void test_or_else()
{
    assert(or_else(just(2), 1) == 2);
    assert(or_else(maybe<int>(), 1) == 1);
}

void test_maybe()
{
    test_maybe_trivial();
    test_maybe_nontrivial();
    test_maybe_reference();
    test_just();
    test_when();
    test_or_else();
}