#ifndef TEST_IOC_HPP
#define TEST_IOC_HPP

#include "calc_impl.h"
#include "conv_impl.h"
#include "logger_impl.h"

#include "ioc/ioc.hpp"

TEST_CASE("Test context", "[ioc]") {
    auto ctx = ioc::context{
        ioc::new_bean<calc, calc_impl>(),
        ioc::new_bean<conv, conv_impl>(),
        ioc::new_bean<logger, logger_impl>(std::string{"ioc"})
    };

    REQUIRE(ctx.get<calc>() == ctx.get<calc>());
    REQUIRE(ctx.get<conv>() == ctx.get<conv>());
    REQUIRE(ctx.get<logger>() == ctx.get<logger>());

    REQUIRE(ctx.get<calc>()->sum(5, 45) == 145);
    REQUIRE(ctx.get<conv>()->calls_count() == 2);
    REQUIRE(ctx.get<logger>()->calls_count() == 7);
}

#endif
