#ifndef PROMISE_EXCEPTION_HPP
#define PROMISE_EXCEPTION_HPP

#include <stdexcept>

namespace promise {

struct polymorphic_exception: public std::exception {
    virtual void rethrow() const = 0;
};

template <class exception_type>
class promise_exception: public polymorphic_exception {
    static_assert(
        std::is_base_of_v<std::exception, exception_type>, "exception must be a subclass of std::exception"
    );
public:
    explicit promise_exception():
        promise_exception(exception_type{}) {

    }

    explicit promise_exception(exception_type &&e):
        _exception{std::move(e)} {

    }

    const char *what() const noexcept override {
        return _exception.what();
    }

    void rethrow() const override {
        throw _exception;
    }

private:
    exception_type _exception;
};

struct canceled_exception: std::exception {
    const char *what() const noexcept override;
};

const char *canceled_exception::what() const noexcept {
    return "canceled_exception";
}

}

#endif
