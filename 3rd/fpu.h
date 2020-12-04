#ifndef FPU_H_INCLUDED
#define FPU_H_INCLUDED

int32_t fadd_s_wrap(int32_t rs1, int32_t rs2);
int32_t fsub_s_wrap(int32_t rs1, int32_t rs2);
int32_t fcvt_s_w_wrap(int32_t rs1);
int32_t fle_s_wrap(int32_t rs1, int32_t rs2);
int32_t flt_s_wrap(int32_t rs1, int32_t rs2);

#endif
