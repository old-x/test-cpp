#ifndef CALC_IMPL_H
#define CALC_IMPL_H

#include "conv.h"
#include "logger.h"
#include "calc.h"

#include "ioc/context.hpp"

class calc_impl: public calc {
public:
    template <class context>
    explicit calc_impl(context *ctx):
    calc_impl(ioc::get<logger>(ctx), ioc::get<conv>(ctx)) {

    }

    ~calc_impl();

    int sum(int a, int b) override;

    void init();
    void destroy();

private:
    calc_impl(logger *log, conv *cnv);

    logger *_log;
    conv *_cnv;
};

#endif
