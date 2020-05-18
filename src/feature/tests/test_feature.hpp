#ifndef TEST_FEATURE_HPP
#define TEST_FEATURE_HPP

#include <cassert>
#include <cstdint>
#include <string>

#include "feature/feature.hpp"

using feature::detector;
using feature::global_detector;

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

struct1 some_function(double) {
    return {};
}

declare_feature_method(method, method);
declare_feature_method(find, find);
declare_feature_field(end_field, _end);
declare_feature_method_global(existed_global, some_function);
declare_feature_method_global(unknown_global, unknown_function);

template <typename type>
int get_method_if_possible(const type &value) {
    if constexpr (detector<method>::in<type>()) {
        return value.method();
    } else {
        return -1;
    }
}

template <typename type>
auto get_end_if_possible() {
    static_assert(std::is_enum_v<type>, "type must be an enumeration");
    if constexpr (detector<end_field>::in<type>()) {
        return static_cast<std::underlying_type_t<type>>(type::_end);
    } else {
        return -1;
    }
}

void test_detector() {
    static_assert(detector<method>::in<struct1>() == true);
    static_assert(detector<method>::in<struct2>() == false);

    static_assert(detector<method>::in_r<struct1, int>() == true);
    static_assert(detector<method>::in_r<struct1, char>() == false);
    static_assert(detector<method>::in_r<struct2, int>() == false);

    static_assert(detector<end_field>::in<result1>() == true);
    static_assert(detector<end_field>::in<result2>() == false);

    static_assert(detector<end_field>::in_r<result1, result1>() == true);
    static_assert(detector<end_field>::in_r<result1, result2>() == false);
    static_assert(detector<end_field>::in_r<result1, std::uint16_t>() == false);
    static_assert(detector<end_field>::in_r<result2, result2>() == false);

    static_assert(detector<find>::in<std::string, std::string>() == true);
    static_assert(detector<find>::in_r<std::string, std::size_t, std::string>() == true);
    static_assert(detector<find>::in_r<std::string, long, std::string>() == false);
    static_assert(detector<find>::in_r<std::string, std::size_t, int, int, int>() == false);

    static_assert(global_detector<existed_global>::in<double>() == true);
    static_assert(global_detector<existed_global>::in<std::string>() == false);
    static_assert(global_detector<existed_global>::in<>() == false);

    static_assert(global_detector<existed_global>::in_r<struct1, double>() == true);
    static_assert(global_detector<existed_global>::in_r<int, double>() == false);
    static_assert(global_detector<existed_global>::in_r<struct1>() == false);

    static_assert(global_detector<unknown_global>::in<>() == false);
    static_assert(global_detector<unknown_global>::in_r<int>() == false);

    assert(get_method_if_possible(struct1{}) == 567);
    assert(get_method_if_possible(struct2{}) == -1);
    assert(get_end_if_possible<result1>() == 113);
    assert(get_end_if_possible<result2>() == -1);
}

#endif
