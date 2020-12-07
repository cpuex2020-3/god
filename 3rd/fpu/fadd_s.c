#include <stdio.h>
#include "reg_t.h"

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
