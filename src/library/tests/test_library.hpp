#ifndef TEST_LIBRARY_HPP
#define TEST_LIBRARY_HPP

#include <cassert>

#include "library/library.hpp"

void test_library() {
    try {
        library::instance::load("unknown.so");
        assert(false);
    } catch (const std::logic_error&) {
    }

    library::instance lib = library::instance::load("./libshared.so",
        library::mode::global, library::mode::resolve_all
    );

    auto get = lib.resolve<int()>("get");
    auto sum = lib.resolve<int(int, int)>("sum");

    assert(get() == 42);
    assert(sum(7, 4) == 11);

    try {
        lib.resolve<int()>("find");
        assert(false);
    } catch (const std::logic_error&) {
    }
}

#endif
