#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "reg_t.h"

reg_t fle_s(reg_t x1, reg_t x2) {
    reg_t x1d = x1, x2d = x2;
    x1d.bits.sign = 0;
    x2d.bits.sign = 0;
    int bothzero = (x1d.x == 0) && (x2d.x == 0);
    int le = x1d.x <= x2d.x, ge = x1d.x >= x2d.x;
    reg_t y;
    if (!x1.bits.sign && !x2.bits.sign) y.x = le; else
    if (!x1.bits.sign &&  x2.bits.sign) y.x = bothzero; else
    if ( x1.bits.sign && !x2.bits.sign) y.x = 1; else
    y.x = ge;
    return y;
}
