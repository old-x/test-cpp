#ifndef IOC_BEAN_HPP
#define IOC_BEAN_HPP

#include <tuple>
#include <utility>

#include "storage.hpp"

namespace ioc {

template <class interface, class implementation, class ...args>
class bean final {
    static_assert(std::is_base_of_v<interface, implementation>, "implementation must inherit from interface");

public:
    constexpr explicit bean(args &&...values):
    _args{std::forward<args>(values)...},
    _storage{} {

    }

    explicit bean(const bean&) = delete;
    bean &operator=(const bean&) = delete;
    constexpr explicit bean(bean&&) = default;
    constexpr bean &operator=(bean&&) = default;

    constexpr operator bool() const noexcept {
        return _storage;
    }

    constexpr implementation *get() const noexcept {
        return _storage.get();
    }

    template <class type>
    static constexpr bool is_interface() noexcept {
        return std::is_same_v<type, interface>;
    }

    template <class context>
    constexpr void init([[maybe_unused]] context *ctx) {
        constexpr bool is_ctx_args_constructible = std::is_constructible_v<implementation, context*, args...>;
        constexpr bool is_args_constructible = std::is_constructible_v<implementation, args...>;
        static_assert(is_ctx_args_constructible || is_args_constructible, "bean cannot be constructed");

        if constexpr (is_ctx_args_constructible) {
            std::apply(_storage, std::tuple_cat(std::make_tuple(ctx), _args));
        } else if constexpr (is_args_constructible) {
            std::apply(_storage, _args);
        }
    }

private:
    std::tuple<args...> _args;
    storage<implementation> _storage;
};

template <class interface, class implementation, class ...args>
constexpr auto new_bean(args &&...values) {
    return bean<interface, implementation, args...>{std::forward<args>(values)...};
}

}

#endif
