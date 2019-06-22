#ifndef LOGGER_IMPL_H
#define LOGGER_IMPL_H

#include "logger.h"

class logger_impl: public logger {
public:
    logger_impl(std::string prefix);
    ~logger_impl();

    void log(std::string msg) override;
    std::size_t calls_count() const override;

private:
    std::string _prefix;
    std::size_t _calls_count;
};

#endif
