#ifndef IOC_CONTEXT_HPP
#define IOC_CONTEXT_HPP

#include <mutex>
#include <tuple>
#include <utility>

namespace ioc {

template <class ...beans>
class context final {
    static_assert(sizeof...(beans) > 0, "context must contain at least one bean");

public:
    constexpr explicit context(beans &&...values):
    _beans{std::forward<beans>(values)...},
    _locks{} {

    }

    explicit context(const context&) = delete;
    context &operator=(const context&) = delete;
    explicit context(context&&) = delete;
    context &operator=(context&&) = delete;

    template <class interface>
    interface *get() {
        constexpr auto index = search_bean<interface>();
        using type = std::remove_const_t<decltype(index)>;
        static_assert(std::is_same_v<std::size_t, type>, "bean not found");
        auto &bean = std::get<index>(_beans);
        init(bean, index);
        return bean.get();
    }

private:
    using beans_t = std::tuple<beans...>;

    struct not_found_tag {};

    template <class interface, std::size_t index = 0>
    static constexpr auto search_bean() noexcept {
        if constexpr (index >= std::tuple_size_v<beans_t>) {
            return not_found_tag{};
        } else {
            using type = std::tuple_element_t<index, beans_t>;
            if constexpr (type::template is_interface<interface>()) {
                return index;
            } else {
                return search_bean<interface, index + 1>();
            }
        }
    }

    template <class bean>
    void init(bean &instance, const std::size_t index) {
        std::lock_guard<std::mutex> lock{_locks[index]};
        if (!instance) {
            instance.init(this);
        }
    }

    beans_t _beans;
    std::mutex _locks[std::tuple_size_v<beans_t>];
};

template <class interface, class context>
constexpr auto get(context *ctx) {
    return ctx->template get<interface>();
}

}

#endif
