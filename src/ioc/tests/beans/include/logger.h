#ifndef TEST_IOC_LOGGER_H
#define TEST_IOC_LOGGER_H

#include <string>

struct logger {
    virtual ~logger() = default;
    virtual void log(std::string msg) = 0;
    virtual std::size_t calls_count() const = 0;
};

#endif
