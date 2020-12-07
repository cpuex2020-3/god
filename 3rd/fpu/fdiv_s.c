#include <stdio.h>
#include "reg_t.h"
#include "fmul_s.h"
#include "finv_table.h"

reg_t finv_s(reg_t x1) {
    // step 1
    unsigned s1 = x1.bits.sign;
    unsigned e1 = x1.bits.exp;
    unsigned m1 = x1.bits.mantissa;
    unsigned a0 = slice(m1, 22, 13);
    unsigned a1 = slice(m1, 12, 0);
    // step 2
    unsigned c1 = finv_table[a0*2];
    unsigned k1 = finv_table[a0*2+1];
    c1 = (c1 << 3)|(k1 >> 13);
    k1 = slice(k1, 12, 0);
    unsigned c2 = (1<<23)|c1;
    unsigned d1 = k1 * a1;
    unsigned d2 = slice(d1, 25, 12);
    unsigned m2 = c2 - d2;
    // conclude
    unsigned e2 = (m1 == 0) ? 254 - e1 : 253 - e1;
    unsigned m3 = (m1 == 0) ? 0 : slice(m2, 22, 0);
    reg_t y;
    y.bits.sign = s1;
    y.bits.exp = e2;
    y.bits.mantissa = m3;
    return y;
}

reg_t fdiv_s(reg_t x1, reg_t x2) {
    unsigned e1 = x1.bits.exp;
    unsigned e2 = x2.bits.exp;
    unsigned e3 = (e2 >= 254) ? 1 : 0;
    unsigned x1n = (x1.bits.sign << 31)|((e1-e3) << 23)|x1.bits.mantissa;
    unsigned x2n = (x2.bits.sign << 31)|((e2-e3) << 23)|x2.bits.mantissa;

    reg_t x1n_reg = {.x = x1n};
    reg_t x2n_reg = {.x = x2n};
    reg_t x2n_inv = finv_s(x2n_reg);
    reg_t y = fmul_s(x1n_reg, x2n_inv);
    return y;
}
