#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <cstdint>
#include <utility>

#include "lifecycle.hpp"

namespace ioc {

template <class instance>
class storage final {
public:
    constexpr storage() noexcept:
    _instance{nullptr},
    _memory{} {

    }

    ~storage() noexcept {
        if (_instance) {
            lifecycle::destroy(_instance);
            _instance->~instance();
        }
    }

    explicit storage(const storage&) = delete;
    storage &operator=(const storage&) = delete;
    constexpr explicit storage(storage&&) = default;
    constexpr storage &operator=(storage&&) = default;

    constexpr operator bool() const noexcept {
        return _instance;
    }

    constexpr instance *get() const noexcept {
        return _instance;
    }

    template <class ...args>
    constexpr void operator()(args &&...values) {
        _instance = new(_memory) instance{std::forward<decltype(values)>(values)...};
        lifecycle::init(_instance);
    }

private:
    instance *_instance;
    std::int8_t _memory[sizeof(instance)];
};

}

#endif
