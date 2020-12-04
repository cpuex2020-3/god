#include <stdio.h>
#include "reg_t.h"

reg_t flt_s(reg_t x1, reg_t x2) {
    reg_t x1d = x1, x2d = x2;
    x1d.bits.sign = 0;
    x2d.bits.sign = 0;
    int bothzero = (x1d.x == 0) && (x2d.x == 0);
    int lt = x1d.x < x2d.x, gt = x1d.x > x2d.x;
    reg_t y;
    if (!x1.bits.sign && !x2.bits.sign) y.x = lt; else
    if (!x1.bits.sign &&  x2.bits.sign) y.x = 0; else
    if ( x1.bits.sign && !x2.bits.sign) y.x = !bothzero; else
    y.x = gt;
    return y;
}
