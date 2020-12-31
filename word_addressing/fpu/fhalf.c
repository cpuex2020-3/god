#include <stdio.h>
#include "reg_t.h"

reg_t fhalf(reg_t x1) {
    unsigned s1 = x1.bits.sign;
    unsigned e1 = x1.bits.exp;
    unsigned m1 = x1.bits.mantissa;
    reg_t y;
    y.bits.sign = s1;
    y.bits.exp = e1-1;
    y.bits.mantissa = m1;
    return y;
}
