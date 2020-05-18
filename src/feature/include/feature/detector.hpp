#ifndef FEATURE_DETECTOR_HPP
#define FEATURE_DETECTOR_HPP

#include <type_traits>

namespace feature {

template <template<typename, typename...> typename feature>
class detector {
public:
    template <typename type, typename ...args>
    static constexpr bool in() {
        return in_impl<type, args...>(nullptr);
    }

    template <typename type, typename result, typename ...args>
    static constexpr bool in_r() {
        return in_impl_r<type, result, args...>(nullptr);
    }

private:
    template <typename ...args>
    detector(args &&...) = delete;

    template <typename type, typename ...args>
    static constexpr bool in_impl(feature<type, args...>*) {
        return true;
    }

    template <typename type, typename ...args>
    static constexpr bool in_impl(...) {
        return false;
    }

    template <typename type, typename result, typename ...args>
    static constexpr bool in_impl_r(feature<type, args...>*) {
        return std::is_same_v<result, feature<type, args...>>;
    }

    template <typename type, typename result, typename ...args>
    static constexpr bool in_impl_r(...) {
        return false;
    }
};

}

#endif
