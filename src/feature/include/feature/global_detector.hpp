#ifndef FEATURE_GLOBAL_DETECTOR_HPP
#define FEATURE_GLOBAL_DETECTOR_HPP

#include <type_traits>

namespace feature {

template <template<typename...> typename feature>
class global_detector {
public:
    template <typename ...types>
    static constexpr bool in() {
        return in_impl<types...>(nullptr);
    }

    template <typename result, typename ...types>
    static constexpr bool in_r() {
        return in_impl_r<result, types...>(nullptr);
    }

private:
    template <typename ...args>
    global_detector(args &&...) = delete;

    template <typename ...types>
    static constexpr bool in_impl(feature<types...>*) {
        return true;
    }

    template <typename ...types>
    static constexpr bool in_impl(...) {
        return false;
    }

    template <typename result, typename ...types>
    static constexpr bool in_impl_r(feature<types...>*) {
        return std::is_same_v<result, feature<types...>>;
    }

    template <typename result, typename ...types>
    static constexpr bool in_impl_r(...) {
        return false;
    }
};

}

#endif
