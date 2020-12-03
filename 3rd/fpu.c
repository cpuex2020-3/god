#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

typedef union myreg {
    unsigned x;
    float    f;
    struct {
        unsigned mantissa : 23;
        unsigned exp : 8;
        unsigned sign : 1;
    } __attribute((packed)) bits;
} reg_t;

reg_t convert_i_r(int32_t uo){
  reg_t x = {.x = (uint32_t)uo};
  return x;
}

int32_t convert_r_i(reg_t uo){
  return ((int32_t)(uo.x));
}

// fcvt_s_w
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

int32_t fcvt_s_w_wrap(int32_t rs1){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rd  = fcvt_s_w(r_rs1);
  return convert_r_i(r_rd);
}

// fle_s
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

int32_t fle_s_wrap(int32_t rs1, int32_t rs2){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rs2 = convert_i_r(rs2);
  reg_t r_rd  = fle_s(r_rs1, r_rs2);
  return convert_r_i(r_rd);
}

// flt_s
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

int32_t flt_s_wrap(int32_t rs1, int32_t rs2){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rs2 = convert_i_r(rs2);
  reg_t r_rd  = flt_s(r_rs1, r_rs2);
  return convert_r_i(r_rd);
}
