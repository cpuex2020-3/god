#include <stdio.h>
#include "reg_t.h"
#include "fsqrt_table.h"

reg_t fsqrt_s(reg_t x1) {
    // step 1
    unsigned s1 = x1.bits.sign;
    unsigned e1 = x1.bits.exp;
    unsigned m1 = x1.bits.mantissa;
    unsigned a0 = slice(m1, 22, 14);
    unsigned a1 = slice(m1, 13, 0);
    unsigned key = ((e1&1) << 9)|a0;
    // step 2
    unsigned c1 = fsqrt_table[key*2];
    unsigned k1 = fsqrt_table[key*2+1];
    c1 = (c1 << 3)|(k1 >> 13);
    k1 = slice(k1, 12, 0);
    unsigned c2 = (1<<23)|c1;
    unsigned k2 = (1<<13)|k1;
    unsigned d1 = k2 * a1;
    unsigned d2 = ((e1&1) != 0) ? slice(d1, 27, 15) : slice(d1, 27, 14);
    unsigned m2 = (c2 + d2)&0x00ffffff;
    // conclude
    unsigned e2 = (e1 >> 1) + (((e1&1) != 0) ? 64 : 63);
    unsigned m3 = (((e1&1) != 0) && (m1 == 0)) ? 0 : slice(m2, 22, 0);
    reg_t y;
    if(x1.x == 0){
      y.bits.sign = 0;
      y.bits.exp = 0;
      y.bits.mantissa = 0;
    }
    else{
      y.bits.sign = s1;
      y.bits.exp = e2;
      y.bits.mantissa = m3;
    }
    return y;
}
