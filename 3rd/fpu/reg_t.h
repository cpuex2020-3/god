#ifndef REG_T_H_INCLUDED
#define REG_T_H_INCLUDED

typedef union myreg {
    unsigned x;
    float    f;
    struct {
        unsigned mantissa : 23;
        unsigned exp : 8;
        unsigned sign : 1;
    } __attribute((packed)) bits;
} reg_t;

#endif