#ifndef TEST_FEATURE_HPP
#define TEST_FEATURE_HPP

#include <cassert>
#include <cstdint>
#include <type_traits>

#include "feature/feature.hpp"

struct has_method {
    int method() const {
        return 567;
    }
};

struct struct1: has_method {};

struct struct2 {};

enum class result1: std::uint16_t {
    success = 1,
    failure = 112,
    _end
};

enum class result2: std::uint16_t {
    success,
    failure = 112
};

template <typename type>
using method = decltype(std::declval<type>().method());

template <typename type>
int get_method_if_possible(const type &value) {
    if constexpr (feature::detector<method>::in<type>()) {
        return value.method();
    } else {
        return -1;
    }
}

template <typename type>
using end_field = decltype(type::_end);

template <typename type>
auto get_end_if_possible() {
    static_assert(std::is_enum_v<type>, "type must be an enumeration");
    if constexpr (feature::detector<end_field>::in<type>()) {
        return static_cast<std::underlying_type_t<type>>(type::_end);
    } else {
        return -1;
    }
}

void test_detector() {
    assert(get_method_if_possible(struct1{}) == 567);
    assert(get_method_if_possible(struct2{}) == -1);
    assert(get_end_if_possible<result1>() == 113);
    assert(get_end_if_possible<result2>() == -1);
}

#endif
