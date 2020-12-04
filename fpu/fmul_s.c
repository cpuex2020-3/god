#include <stdio.h>
#include "reg_t.h"

reg_t fmul_s(reg_t x1, reg_t x2) {
    // step 1
    unsigned s1 = x1.bits.sign;
    unsigned e1 = x1.bits.exp;
    unsigned m1 = x1.bits.mantissa;
    unsigned stingy_h1 = (m1 >> 10);
    unsigned l1 = m1 & 0x3FF;
    unsigned s2 = x2.bits.sign;
    unsigned e2 = x2.bits.exp;
    unsigned m2 = x2.bits.mantissa;
    unsigned stingy_h2 = (m2 >> 10);
    unsigned l2 = m2 & 0x3FF;
    // step 2
    unsigned h1 = (1 << 12) | stingy_h1;
    unsigned h2 = (1 << 12) | stingy_h2;
    unsigned hh = h1*h2;
    unsigned hl = h1*l2;
    unsigned lh = l1*h2;
    // step 5
    unsigned e3 = e1+e2+129;
    unsigned s3 = s1^s2;
    // step 3
    unsigned m3 = hh + (hl >> 11) + (lh >> 11) + 2;
    unsigned e4 = e3+1;
    unsigned e5 = ((e3 & (1 << 8)) == 0) ? 0 : ((m3 & (1 << 25)) != 0) ? (e4 & 0xFF) : (e3 & 0xFF);
    // step 4
    unsigned m4 = ((e3 & (1 << 8)) == 0) ? 0 : ((m3 & (1 << 25)) != 0) ? ((m3 >> 2) & 0x7FFFFF) : ((m3 >> 1) & 0x7FFFFF);
    reg_t y;
    y.bits.sign = s3 & 1;
    y.bits.exp = e5 & 0xFF;
    y.bits.mantissa = m4 & 0x7FFFFF;
    return y;
}
