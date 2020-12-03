#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "reg_t.h"

reg_t srai(reg_t x, int shamt) {
    for (int i = 0; i < shamt; ++i) {
        int sgn = x.bits.sign;
        x.x = x.x >> 1;
        x.bits.sign = sgn;
    }
    return x;
}

reg_t itof_small(reg_t x, int isneg) {
    reg_t a;
    reg_t c23 = {.f = 8388608.f}, c24 = {.f = 16777216.f};
    a.bits.sign = c23.bits.sign;
    a.bits.exp = c23.bits.exp;
    a.bits.mantissa = x.bits.mantissa;
    reg_t b = isneg ? c24 : c23;
    reg_t ret;
    ret.f = a.f - b.f;
    return ret;
}

reg_t itof_large(reg_t x, int isneg) {
    reg_t y = itof_small(x, isneg);
    y.bits.exp += 23;
    return y;
}

reg_t fcvt_s_w(reg_t x) {
    int isneg = x.bits.sign;
    reg_t xd = {.x = x.bits.mantissa}, xu = srai(x, 23);
    reg_t yd = itof_small(xd, 0), yu = itof_large(xu, isneg);
    return (reg_t){.f = yd.f + yu.f};
}
