#ifndef TEST_IOC_CONV_H
#define TEST_IOC_CONV_H

#include <cstdint>

struct conv {
    virtual ~conv() = default;
    virtual int convert(int a) = 0;
    virtual std::size_t calls_count() const = 0;
};

#endif
