#ifndef LIBRARY_INSTANCE_HPP
#define LIBRARY_INSTANCE_HPP

#include <dlfcn.h>

#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

#include "function.hpp"
#include "mode.hpp"

namespace library {

class instance {
public:
    template <typename ...modes>
    static instance load(const std::string_view path, const modes ...values) {
        static_assert((std::is_same_v<mode, modes> && ...), "Only library::mode values are allowed");
        mode_detector detector;
        (detector(values), ...);
        void *ptr = dlopen(path.data(), detector.get_mode());
        if (ptr) {
            return instance{ptr};
        }
        throw std::logic_error{dlerror()};
    }

    template <typename result, typename ...args>
    function<result, args...> resolve(const std::string_view name) {
        void *ptr = dlsym(_ptr, name.data());
        if (ptr) {
            return function<result, args...>{ptr};
        }
        throw std::logic_error{dlerror()};
    }

    instance(instance &&rhs):
        _ptr{std::exchange(rhs._ptr, nullptr)}
    {}

    instance &operator=(instance &&rhs) {
        _ptr = std::exchange(rhs._ptr, nullptr);
        return *this;
    }

    ~instance() {
        if (_ptr)
            dlclose(_ptr);
    }

private:
    instance(void *ptr):
        _ptr{ptr}
    {}

    instance(const instance&) = delete;
    instance &operator=(const instance&) = delete;

    void *_ptr;

    class mode_detector {
    public:
        constexpr mode_detector() = default;

        constexpr void operator()(const mode m) {
            switch (m) {
            case mode::resolve_all:
                _required = RTLD_NOW;
                break;
            case mode::global:
                _optional |= RTLD_GLOBAL;
                break;
            case mode::no_delete:
                _optional |= RTLD_NODELETE;
                break;
            case mode::no_load:
                _optional |= RTLD_NOLOAD;
                break;
            case mode::deep_bind:
                _optional |= RTLD_DEEPBIND;
                break;
            }
        }

        constexpr int get_mode() const {
            return _required | _optional;
        }

    private:
        int _required = RTLD_LAZY;
        int _optional = 0;
    };
};

}

#endif
