#ifndef TEST_STORAGE_HPP
#define TEST_STORAGE_HPP

#include <cassert>
#include <string_view>

#include "storage/storage.hpp"

using namespace std::string_view_literals;

struct small {
    int i1;
    int i2;
};

struct big {
    char buf[65];
};

void test_storage() {
    storage::storage<small> s1{123, 234};
    static_assert(sizeof(s1) == sizeof(small), "small is on the stack");
    assert(s1.get().i1 == 123);
    assert(s1.get().i2 == 234);

    storage::storage<big> s2{'h', 'e', 'l', 'l', 'o', '\x0'};
    static_assert(sizeof(s2) == sizeof(big*), "big is on the heap");
    assert(s2.get().buf == "hello"sv);

    storage::storage<small, 6> s3{345, 456};
    static_assert(sizeof(s3) == sizeof(small*), "small is on the heap");
    assert(s3.get().i1 == 345);
    assert(s3.get().i2 == 456);

    storage::storage<big, 66> s4{'w', 'o', 'r', 'l', 'd', '\x0'};
    static_assert(sizeof(s4) == sizeof(big), "big is on the stack");
    assert(s4.get().buf == "world"sv);

    auto s5{s1};
    assert(s5.get().i1 == 123);
    assert(s5.get().i2 == 234);

    auto s6{s3};
    assert(s6.get().i1 == 345);
    assert(s6.get().i2 == 456);

    storage::storage<small> s7;
    s7 = s5;
    assert(s7.get().i1 == 123);
    assert(s7.get().i2 == 234);

    storage::storage<small, 6> s8;
    s8 = s6;
    assert(s8.get().i1 == 345);
    assert(s8.get().i2 == 456);

    s5 = std::move(s1);
    assert(s5.get().i1 == 123);
    assert(s5.get().i2 == 234);

    s6 = std::move(s3);
    assert(s6.get().i1 == 345);
    assert(s6.get().i2 == 456);

    auto s9{s2};
    assert(s9.get().buf == "hello"sv);

    auto s10{s4};
    assert(s10.get().buf == "world"sv);
}

#endif
