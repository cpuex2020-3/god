#include <stdio.h>
#include "reg_t.h"

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
