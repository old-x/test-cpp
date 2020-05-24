#ifndef LIBRARY_FUNCTION_HPP
#define LIBRARY_FUNCTION_HPP

#include <utility>

namespace library {

template <typename...>
class function {
    template <typename ...args>
    function(args &&...) = delete;
};

template <typename result, typename ...args>
class function<result(args...)> {
public:
    result operator()(args &&...values) const {
        return _ptr(std::forward<args>(values)...);
    }

private:
    using ptr_t = result(*)(args...);
    friend class instance;

    function(void *ptr):
        _ptr{reinterpret_cast<ptr_t>(ptr)}
    {}

    ptr_t _ptr;
};

}

#endif
