#include <stdio.h>
#include "reg_t.h"
#include "fadd_s.h"
#include "fsub_s.h"

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
    reg_t c23 = {.x = 0x4B000000}, c24 = {.x = 0x4B800000};
    a.bits.sign = c23.bits.sign;
    a.bits.exp = c23.bits.exp;
    a.bits.mantissa = x.bits.mantissa;
    reg_t b = isneg ? c24 : c23;
    reg_t ret = fsub_s(a, b);
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
    reg_t y = fadd_s(yd, yu);
    return y;
}
