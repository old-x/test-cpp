#ifndef FEATURE_DETECTOR_HPP
#define FEATURE_DETECTOR_HPP

namespace feature {

template <template<typename> typename feature>
class detector {
public:
    template <typename type>
    static constexpr bool in() {
        return in_impl<type>(nullptr);
    }

private:
    template <typename ...args>
    detector(args &&...) = delete;

    template <typename type>
    static constexpr bool in_impl(feature<type>*) {
        return true;
    }

    template <typename type>
    static constexpr bool in_impl(...) {
        return false;
    }
};

}

#endif
