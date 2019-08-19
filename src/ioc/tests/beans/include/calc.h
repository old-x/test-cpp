#ifndef TEST_IOC_CALC_H
#define TEST_IOC_CALC_H

struct calc {
    virtual ~calc() = default;
    virtual int sum(int a, int b) = 0;
};

#endif
