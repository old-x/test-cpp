#ifndef PROMISE_HPP
#define PROMISE_HPP

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>

#include "exception.hpp"

namespace promise {

template <class type>
class promise {
    enum class state: std::uint8_t {
        initial,
        result,
        exception,
        canceled
    };

public:
    using handler = std::function<void(promise*)>;

    explicit promise():
        promise{state::initial, nullptr, nullptr} {

    }

    static constexpr promise completed(type &&result) {
        return {state::result, std::make_unique<type>(std::move(result)), nullptr};
    }

    template <class exception_type>
    static constexpr promise failed(exception_type &&e) {
        return {state::exception, nullptr, std::make_unique<promise_exception<exception_type>>(std::move(e))};
    }

    bool is_done() const {
        lock_guard l{_mutex};
        return _is_done();
    }

    bool is_canceled() const {
        lock_guard l{_mutex};
        return _state == state::canceled;
    }

    bool has_result() const {
        lock_guard l{_mutex};
        return _state == state::result;
    }

    bool has_exception() const {
        lock_guard l{_mutex};
        return _state == state::exception || _state == state::canceled;
    }

    std::unique_ptr<type> get_result() {
        lock_guard l{_mutex};
        return std::move(_result);
    }

    std::unique_ptr<polymorphic_exception> get_exception() {
        lock_guard l{_mutex};
        return std::move(_exception);
    }

    void complete(type &&result) {
        handlers local_handlers;
        {
            lock_guard l{_mutex};
            if (_is_done()) {
                return;
            }
            _handlers.swap(local_handlers);
            _result = std::make_unique<type>(std::move(result));
            _state = state::result;
        }

        _cond.notify_all();
        for (handler &h : local_handlers) {
            h(this);
        }
    }

    template <class exception_type>
    void complete_exceptionally(exception_type &&e) {
        handlers local_handlers;
        {
            lock_guard l{_mutex};
            if (_is_done()) {
                return;
            }
            _handlers.swap(local_handlers);
            _exception = std::make_unique<promise_exception<exception_type>>(std::move(e));
            _state = state::exception;
        }

        _cond.notify_all();
        for (handler &h : local_handlers) {
            h(this);
        }
    }

    void when_done(handler &&h) {
        bool done = false;
        {
            lock_guard l{_mutex};
            done = _is_done();
            if (!done) {
                _handlers.push_back(std::move(h));
            }
        }
        if (done) {
            h(this);
        }
    }

    void cancel() {
        handlers local_handlers;
        {
            lock_guard l{_mutex};
            if (_is_done()) {
                return;
            }
            _handlers.swap(local_handlers);
            _exception = std::make_unique<promise_exception<canceled_exception>>();
            _state = state::canceled;
        }

        _cond.notify_all();
        for (handler &h : local_handlers) {
            h(this);
        }
    }

    std::unique_ptr<type> wait() {
        unique_lock lock{_mutex};
        _cond.wait(lock, _is_done);
        if (_state == state::result)
            return std::move(_result);
        _exception->rethrow();
        return {};
    }

    template <class rep, class period>
    std::unique_ptr<type> wait(const std::chrono::duration<rep, period> &timeout) {
        unique_lock lock{_mutex};
        if (_cond.wait_for(lock, timeout, _is_done)) {
            if (_state == state::result)
                return std::move(_result);
            _exception->rethrow();
        }

        handlers local_handlers;
        _handlers.swap(local_handlers);
        _exception = std::make_unique<promise_exception<canceled_exception>>();
        _state = state::canceled;
        lock.unlock();

        _cond.notify_all();
        for (handler &h : local_handlers) {
            h(this);
        }
        throw canceled_exception{};
    }

private:
    using lock_guard = std::lock_guard<std::mutex>;
    using unique_lock = std::unique_lock<std::mutex>;
    using handlers = std::vector<handler>;

    promise(const state st, std::unique_ptr<type> &&result, std::unique_ptr<polymorphic_exception> &&e):
        _mutex{},
        _cond{},
        _state{st},
        _result{std::move(result)},
        _exception{std::move(e)},
        _handlers{},
        _is_done{[this] { return _state != state::initial; }} {

    }

    promise(const promise&) = delete;
    promise(promise&&) = delete;
    promise &operator=(const promise&) = delete;
    promise &operator=(promise&&) = delete;

    mutable std::mutex _mutex;
    std::condition_variable _cond;
    state _state;
    std::unique_ptr<type> _result;
    std::unique_ptr<polymorphic_exception> _exception;
    handlers _handlers;
    const std::function<bool()> _is_done;
};

}

#endif
