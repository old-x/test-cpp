#ifndef TEST_IOC_HPP
#define TEST_IOC_HPP

#include <cassert>

#include "calc_impl.h"
#include "conv_impl.h"
#include "logger_impl.h"

#include "ioc/ioc.hpp"

void test_context() {
    auto ctx = ioc::context{
        ioc::new_bean<calc, calc_impl>(),
        ioc::new_bean<conv, conv_impl>(),
        ioc::new_bean<logger, logger_impl>(std::string{"ioc"})
    };

    assert(ctx.get<calc>() == ctx.get<calc>());
    assert(ctx.get<conv>() == ctx.get<conv>());
    assert(ctx.get<logger>() == ctx.get<logger>());

    assert(ctx.get<calc>()->sum(5, 45) == 145);
    assert(ctx.get<conv>()->calls_count() == 2);
    assert(ctx.get<logger>()->calls_count() == 7);
}

#endif
