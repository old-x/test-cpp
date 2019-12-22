#ifndef TEST_PROMISE_HPP
#define TEST_PROMISE_HPP

#include <cassert>

#include <atomic>
#include <string_view>
#include <thread>

#include "promise/promise.hpp"

void test_empty_promise() {
    promise::promise<int> p;
    assert(p.is_done() == false);
    assert(p.is_canceled() == false);
    assert(p.has_result() == false);
    assert(p.has_exception() == false);
    assert(p.get_result().get() == nullptr);
    assert(p.get_exception().get() == nullptr);
}

void test_completed_promise() {
    auto p = promise::promise<int>::completed(123);
    assert(p.is_done() == true);
    assert(p.is_canceled() == false);
    assert(p.has_result() == true);
    assert(p.has_exception() == false);
    assert(*p.get_result() == 123);
    assert(p.get_exception().get() == nullptr);
}

void test_failed_promise() {
    auto p = promise::promise<int>::failed(std::logic_error{"failed promise"});
    assert(p.is_done() == true);
    assert(p.is_canceled() == false);
    assert(p.has_result() == false);
    assert(p.has_exception() == true);
    assert(p.get_result().get() == nullptr);
    assert(p.get_exception()->what() == std::string_view{"failed promise"});
}

void test_completion() {
    promise::promise<int> p;
    std::atomic_int counter = 0;
    p.when_done([&counter](auto *pr) {
        ++counter;
        assert(pr->is_done() == true);
        assert(pr->is_canceled() == false);
        assert(pr->has_result() == true);
        assert(pr->has_exception() == false);
        assert(*pr->get_result() == 234);
        assert(pr->get_exception().get() == nullptr);
    });
    std::thread t([&p] {
        p.complete(234);
    });
    t.join();
    p.when_done([&counter](auto*) {
        ++counter;
    });
    p.wait(std::chrono::milliseconds{100});
    assert(counter == 2);
}

void test_failure() {
    promise::promise<int> p;
    std::atomic_int counter = 0;
    p.when_done([&counter](auto *pr) {
        ++counter;
        assert(pr->is_done() == true);
        assert(pr->is_canceled() == false);
        assert(pr->has_result() == false);
        assert(pr->has_exception() == true);
        assert(pr->get_result().get() == nullptr);
    });
    std::thread t([&p] {
        p.complete_exceptionally(std::logic_error{"failed promise"});
    });
    t.join();
    p.when_done([&counter](auto*) {
        ++counter;
    });
    try {
        p.wait(std::chrono::milliseconds{100});
        assert(false);
    } catch (const std::logic_error &e) {
        assert(e.what() == std::string_view{"failed promise"});
    }
    assert(counter == 2);
}

void test_cancellation() {
    promise::promise<int> p;
    try {
        p.wait(std::chrono::milliseconds{100});
        assert(false);
    } catch (const promise::canceled_exception &e) {
        assert(e.what() == std::string_view{"canceled_exception"});
    }

    std::atomic_int counter = 0;
    p.when_done([&counter](auto *pr) {
        ++counter;
        assert(pr->is_done() == true);
        assert(pr->is_canceled() == true);
        assert(pr->has_result() == false);
        assert(pr->has_exception() == true);
        assert(pr->get_result().get() == nullptr);
    });

    std::thread t([&p, &counter] {
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
        p.cancel();
        ++counter;
    });

    try {
        p.wait();
        assert(false);
    } catch (const promise::canceled_exception &e) {
        assert(e.what() == std::string_view{"canceled_exception"});
    }

    t.join();
    assert(counter == 2);
}

#endif
