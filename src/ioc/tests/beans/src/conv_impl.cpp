#include "conv_impl.h"

conv_impl::conv_impl(logger *log):
_log{log},
_calls_count{0} {
    _log->log("conv_impl initialized");
}

conv_impl::~conv_impl() {
    _log->log(__func__);
}

int conv_impl::convert(int a) {
    ++_calls_count;
    _log->log("convertion requested for value " + std::to_string(a));
    if (a < 10) {
        return a * 2;
    }
    if (a < 100) {
        return a * 3;
    }
    return a;
}

std::size_t conv_impl::calls_count() const {
    return _calls_count;
}
