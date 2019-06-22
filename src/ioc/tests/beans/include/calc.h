#ifndef CALC_H
#define CALC_H

struct calc {
    virtual ~calc() = default;
    virtual int sum(int a, int b) = 0;
};

#endif
