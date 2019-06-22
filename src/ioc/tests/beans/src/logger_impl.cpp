#include <iostream>

#include "logger_impl.h"

logger_impl::logger_impl(std::string prefix):
_prefix{std::move(prefix)},
_calls_count{0} {
    log("logger initialized with prefix " + _prefix);
}

logger_impl::~logger_impl() {
    log(__func__);
}

void logger_impl::log(std::string msg) {
    ++_calls_count;
    std::cout << _prefix << ": " << msg << std::endl;
}

std::size_t logger_impl::calls_count() const {
    return _calls_count;
}
