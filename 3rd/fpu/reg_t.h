#ifndef REG_T_H_INCLUDED
#define REG_T_H_INCLUDED

typedef union myreg {
    unsigned x;
    struct {
        unsigned mantissa : 23;
        unsigned exp : 8;
        unsigned sign : 1;
    } __attribute((packed)) bits;
} reg_t;

unsigned slice(unsigned x, unsigned h, unsigned l);
unsigned long slicel(unsigned long x, unsigned h, unsigned l);
unsigned fmt(unsigned x, unsigned w);

#endif
