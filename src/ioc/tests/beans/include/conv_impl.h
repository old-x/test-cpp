#ifndef CONV_IMPL_H
#define CONV_IMPL_H

#include "logger.h"
#include "conv.h"

#include "ioc/context.hpp"

class conv_impl: public conv {
public:
    template <class context>
    explicit conv_impl(context *ctx):
    conv_impl(ioc::get<logger>(ctx)) {

    }

    ~conv_impl();

    int convert(int a) override;
    std::size_t calls_count() const override;

private:
    explicit conv_impl(logger *log);

    logger *_log;
    std::size_t _calls_count;
};

#endif
