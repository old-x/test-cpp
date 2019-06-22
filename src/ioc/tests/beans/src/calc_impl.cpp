#include "calc_impl.h"

calc_impl::calc_impl(logger *log, conv *cnv):
_log{log},
_cnv{cnv} {
    _log->log("calc_impl initialized");
}

calc_impl::~calc_impl() {
    _log->log(__func__);
}

int calc_impl::sum(int a, int b) {
    _log->log("summation requested with a=" + std::to_string(a) +
              ", b=" + std::to_string(b));
    return _cnv->convert(a) + _cnv->convert(b);
}

void calc_impl::init() {
    _log->log(__func__);
}

void calc_impl::destroy() {
    _log->log(__func__);
}
