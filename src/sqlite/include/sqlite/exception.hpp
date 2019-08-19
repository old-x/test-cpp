#ifndef SQLITE_EXCEPTION_HPP
#define SQLITE_EXCEPTION_HPP

#include "sqlite3.h"

#include <exception>
#include <ostream>
#include <string>

namespace sqlite {

class exception: public std::exception {
public:
    static constexpr bool is_ok(const int code) noexcept {
        return code == SQLITE_OK;
    }

    static void check(const int code) {
        if (!is_ok(code)) {
            throw exception{code};
        }
    }

    exception(const int code):
    exception{code, sqlite3_errstr(code)} {

    }

    exception(const int code, std::string &&message):
    _code{code},
    _message{std::move(message)} {

    }

    exception(const exception&) = default;
    exception(exception&&) = default;
    exception &operator=(const exception&) = default;
    exception &operator=(exception&&) = default;

    const char *what() const noexcept override;

    int code() const noexcept {
        return _code;
    }

    const std::string &message() const noexcept {
        return _message;
    }

    operator bool() const noexcept {
        return is_ok(_code);
    }

private:
    int _code;
    std::string _message;
};

const char *exception::what() const noexcept {
    return _message.c_str();
}

std::ostream &operator<<(std::ostream &out, const exception &e) {
    return out << '[' << e.code() << "] " << e.message();
}

}

#endif
