#include <stdio.h>

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
  reg_t x = {.x = (unsigned)uo};
  return x;
}

int32_t convert_r_i(reg_t uo){
  return ((int32_t)(uo.x));
}

// slice_module
unsigned slice(unsigned x, unsigned h, unsigned l) {
    unsigned mask = ((1 << h) - (1 << l)) | (1 << h);
    return (x & mask) >> l;
}
unsigned long slicel(unsigned long x, unsigned h, unsigned l) {
    unsigned long mask = ((1L << h) - (1L << l)) | (1L << h);
    return (x & mask) >> l;
}
unsigned fmt(unsigned x, unsigned w) {
    return slice(x, w - 1, 0);
}

// fadd_s
reg_t fadd_s(reg_t x1, reg_t x2) {
    // step 1
    unsigned s1 = x1.bits.sign;
    unsigned e1 = x1.bits.exp;
    unsigned m1 = x1.bits.mantissa;
    unsigned s2 = x2.bits.sign;
    unsigned e2 = x2.bits.exp;
    unsigned m2 = x2.bits.mantissa;
    // step 2
    unsigned m1a = ((e1 != 0) << 23) | m1;
    unsigned m2a = ((e2 != 0) << 23) | m2;
    // step 3
    unsigned e1a = e1 ? e1 : 1;
    unsigned e2a = e2 ? e2 : 1;
    // step 4
    unsigned e2ai = fmt(~e2a, 8);
    // step 5
    unsigned te = e1a + e2ai;
    // step 6
    unsigned ce = !(te & (1 << 8));
    unsigned tde_tmp[2] = {te + 1, ~te};
    unsigned tde = fmt(tde_tmp[ce], 8);
    // step 7
    unsigned de = slice(tde, 7, 5) ? 0x1F : fmt(tde, 5);
    // step 8
    unsigned sel = de ? ce : !(m1a > m2a);
    // step 9
    unsigned ms = sel ? m2a : m1a;
    unsigned mi = sel ? m1a : m2a;
    unsigned es = sel ? e2a : e1a;
    unsigned ss = sel ? s2 : s1;
    // step 10
    unsigned long mie = (unsigned long)mi << 31;
    // step 11
    unsigned long mia = mie >> de;
    // step 12
    unsigned tstck = fmt(mia, 29) != 0;
    // step 13
    unsigned mye =(s1 == s2) ?
        fmt((ms << 2) + slicel(mia, 55, 29), 27) :
        fmt((ms << 2) - slicel(mia, 55, 29), 27);
    // step 14
    unsigned esi = fmt(es + 1, 8);
    // step 15
    unsigned eyd = (mye & (1 << 26)) ? esi : es;
    unsigned myd = (mye & (1 << 26)) ?
        (esi == 0xFF ? 1 << 25 : mye >> 1) : mye;
    unsigned stck = (mye & (1 << 26)) ?
        (esi != 0xFF) && (tstck || (mye & 1)) : tstck;
    // step 16
    unsigned se = fmt(__builtin_clz(myd) - 6, 5);
    // step 17
    unsigned eyf = fmt(eyd - se, 9);
    // step 18
    unsigned myf = !(eyf & (1 << 8)) && (fmt(eyf, 8) != 0) ?
        myd << se : myd << (fmt(eyd, 5) - 1);
    unsigned eyr = !(eyf & (1 << 8)) && (fmt(eyf, 8) != 0) ?
        fmt(eyf, 8) : 0;
    // step 19
    unsigned myr =
        (fmt(myf, 3) == 0b110 && !stck) ||
        (fmt(myf, 2) == 0b10 && s1 == s2 && stck) ||
        (fmt(myf, 2) == 0b11) ?
        fmt(slice(myf, 26, 2) + 1, 25) : slice(myf, 26, 2);
    // step 20
    unsigned eyri = fmt(eyr + 1, 8);
    // step 21
    unsigned ey = (myr & (1 << 24)) ? eyri :
                  fmt(myr, 24) ? eyr : 0;
    unsigned my = (myr & (1 << 24)) ? 0 :
                  fmt(myr, 24) ? fmt(myr, 23) : 0;
    // step 22
    unsigned sy = !ey && !my ? s1 & s2 : ss;
    // step 23
    unsigned nzm1 = !!fmt(m1, 23), nzm2 = !!fmt(m2, 23);
    reg_t y;
    y.bits.sign =
        (e1 == 0xFF && e2 != 0xFF) ? s1 :
        (e1 != 0xFF && e2 == 0xFF) ? s2 :
        (e1 == 0xFF && e2 == 0xFF && nzm2) ? s2 :
        (e1 == 0xFF && e2 == 0xFF && nzm1) ? s1 :
        (e1 == 0xFF && e2 == 0xFF && s1 == s2) ? s1 :
        (e1 == 0xFF && e2 == 0xFF) ? 1 :
        sy;
    y.bits.exp =
        (e1 == 0xFF || e2 == 0xFF) ? 0xFF : ey;
    y.bits.mantissa =
        (e1 == 0xFF && e2 != 0xFF) ? (nzm1 << 22) | fmt(m1, 22) :
        (e1 != 0xFF && e2 == 0xFF) ? (nzm2 << 22) | fmt(m2, 22) :
        (e1 == 0xFF && e2 == 0xFF && nzm2) ? (1 << 22) | fmt(m2, 22) :
        (e1 == 0xFF && e2 == 0xFF && nzm1) ? (1 << 22) | fmt(m1, 22) :
        (e1 == 0xFF && e2 == 0xFF && s1 == s2) ? 0 :
        (e1 == 0xFF && e2 == 0xFF) ? 1 << 22 :
        my;
    return y;
}

int32_t fadd_s_wrap(int32_t rs1, int32_t rs2){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rs2 = convert_i_r(rs2);
  reg_t r_rd  = fadd_s(r_rs1, r_rs2);
  return convert_r_i(r_rd);
}

// fsub_s
reg_t fsub_s(reg_t x1, reg_t x2) {
    // step 1
    unsigned s1 = x1.bits.sign;
    unsigned e1 = x1.bits.exp;
    unsigned m1 = x1.bits.mantissa;
    unsigned s2 = !x2.bits.sign;
    unsigned e2 = x2.bits.exp;
    unsigned m2 = x2.bits.mantissa;
    // step 2
    unsigned m1a = ((e1 != 0) << 23) | m1;
    unsigned m2a = ((e2 != 0) << 23) | m2;
    // step 3
    unsigned e1a = e1 ? e1 : 1;
    unsigned e2a = e2 ? e2 : 1;
    // step 4
    unsigned e2ai = fmt(~e2a, 8);
    // step 5
    unsigned te = e1a + e2ai;
    // step 6
    unsigned ce = !(te & (1 << 8));
    unsigned tde_tmp[2] = {te + 1, ~te};
    unsigned tde = fmt(tde_tmp[ce], 8);
    // step 7
    unsigned de = slice(tde, 7, 5) ? 0x1F : fmt(tde, 5);
    // step 8
    unsigned sel = de ? ce : !(m1a > m2a);
    // step 9
    unsigned ms = sel ? m2a : m1a;
    unsigned mi = sel ? m1a : m2a;
    unsigned es = sel ? e2a : e1a;
    unsigned ss = sel ? s2 : s1;
    // step 10
    unsigned long mie = (unsigned long)mi << 31;
    // step 11
    unsigned long mia = mie >> de;
    // step 12
    unsigned tstck = fmt(mia, 29) != 0;
    // step 13
    unsigned mye =(s1 == s2) ?
        fmt((ms << 2) + slicel(mia, 55, 29), 27) :
        fmt((ms << 2) - slicel(mia, 55, 29), 27);
    // step 14
    unsigned esi = fmt(es + 1, 8);
    // step 15
    unsigned eyd = (mye & (1 << 26)) ? esi : es;
    unsigned myd = (mye & (1 << 26)) ?
        (esi == 0xFF ? 1 << 25 : mye >> 1) : mye;
    unsigned stck = (mye & (1 << 26)) ?
        (esi != 0xFF) && (tstck || (mye & 1)) : tstck;
    // step 16
    unsigned se = fmt(__builtin_clz(myd) - 6, 5);
    // step 17
    unsigned eyf = fmt(eyd - se, 9);
    // step 18
    unsigned myf = !(eyf & (1 << 8)) && (fmt(eyf, 8) != 0) ?
        myd << se : myd << (fmt(eyd, 5) - 1);
    unsigned eyr = !(eyf & (1 << 8)) && (fmt(eyf, 8) != 0) ?
        fmt(eyf, 8) : 0;
    // step 19
    unsigned myr =
        (fmt(myf, 3) == 0b110 && !stck) ||
        (fmt(myf, 2) == 0b10 && s1 == s2 && stck) ||
        (fmt(myf, 2) == 0b11) ?
        fmt(slice(myf, 26, 2) + 1, 25) : slice(myf, 26, 2);
    // step 20
    unsigned eyri = fmt(eyr + 1, 8);
    // step 21
    unsigned ey = (myr & (1 << 24)) ? eyri :
                  fmt(myr, 24) ? eyr : 0;
    unsigned my = (myr & (1 << 24)) ? 0 :
                  fmt(myr, 24) ? fmt(myr, 23) : 0;
    // step 22
    unsigned sy = !ey && !my ? s1 & s2 : ss;
    // step 23
    unsigned nzm1 = !!fmt(m1, 23), nzm2 = !!fmt(m2, 23);
    reg_t y;
    y.bits.sign =
        (e1 == 0xFF && e2 != 0xFF) ? s1 :
        (e1 != 0xFF && e2 == 0xFF) ? s2 :
        (e1 == 0xFF && e2 == 0xFF && nzm2) ? s2 :
        (e1 == 0xFF && e2 == 0xFF && nzm1) ? s1 :
        (e1 == 0xFF && e2 == 0xFF && s1 == s2) ? s1 :
        (e1 == 0xFF && e2 == 0xFF) ? 1 :
        sy;
    y.bits.exp =
        (e1 == 0xFF || e2 == 0xFF) ? 0xFF : ey;
    y.bits.mantissa =
        (e1 == 0xFF && e2 != 0xFF) ? (nzm1 << 22) | fmt(m1, 22) :
        (e1 != 0xFF && e2 == 0xFF) ? (nzm2 << 22) | fmt(m2, 22) :
        (e1 == 0xFF && e2 == 0xFF && nzm2) ? (1 << 22) | fmt(m2, 22) :
        (e1 == 0xFF && e2 == 0xFF && nzm1) ? (1 << 22) | fmt(m1, 22) :
        (e1 == 0xFF && e2 == 0xFF && s1 == s2) ? 0 :
        (e1 == 0xFF && e2 == 0xFF) ? 1 << 22 :
        my;
    return y;
}

int32_t fsub_s_wrap(int32_t rs1, int32_t rs2){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rs2 = convert_i_r(rs2);
  reg_t r_rd  = fsub_s(r_rs1, r_rs2);
  return convert_r_i(r_rd);
}

// fmul_s
reg_t fmul_s(reg_t x1, reg_t x2) {
    // step 1
    unsigned s1 = x1.bits.sign;
    unsigned e1 = x1.bits.exp;
    unsigned m1 = x1.bits.mantissa;
    unsigned stingy_h1 = slice(m1, 22, 11);
    unsigned l1 = slice(m1, 10, 0);
    unsigned s2 = x2.bits.sign;
    unsigned e2 = x2.bits.exp;
    unsigned m2 = x2.bits.mantissa;
    unsigned stingy_h2 = slice(m2, 22, 11);
    unsigned l2 = slice(m2, 10, 0);
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
    unsigned m4 = ((e3 & (1 << 8)) == 0) ? 0 : ((m3 & (1 << 25)) != 0) ? slice(m3, 24, 2) : slice(m3, 23, 1);
    reg_t y;
    y.bits.sign = s3 & 1;
    y.bits.exp = e5 & 0xFF;
    y.bits.mantissa = m4 & 0x7FFFFF;
    return y;
}

int32_t fmul_s_wrap(int32_t rs1, int32_t rs2){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rs2 = convert_i_r(rs2);
  reg_t r_rd  = fmul_s(r_rs1, r_rs2);
  return convert_r_i(r_rd);
}

// fdiv_s
int32_t fdiv_s_wrap(int32_t rs1, int32_t rs2){
  float value = (*((float *)&rs1))/(*((float *)&rs2));
  return (*((int32_t *)&value));
}

// fsqrt_s
#include <math.h>
int32_t fsqrt_s_wrap(int32_t rs1){
  float value = sqrtf(*((float *)&rs1));
  return (*((int32_t *)&value));
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
