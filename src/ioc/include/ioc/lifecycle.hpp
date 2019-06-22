#ifndef LIFECYCLE_HPP
#define LIFECYCLE_HPP

#include <utility>

namespace ioc {

struct lifecycle final {
    lifecycle() = delete;
    explicit lifecycle(const lifecycle&) = delete;
    lifecycle &operator=(const lifecycle&) = delete;
    explicit lifecycle(lifecycle&&) = delete;
    lifecycle &operator=(lifecycle&&) = delete;

    template <class bean, typename = decltype(std::declval<bean>().init())>
    static constexpr void init(bean *instance) {
        instance->init();
    }

    static void init(...) {
    }

    template <class bean, typename = decltype(std::declval<bean>().destroy())>
    static constexpr void destroy(bean *instance) {
        instance->destroy();
    }

    static constexpr void destroy(...) {
    }
};

}

#endif
