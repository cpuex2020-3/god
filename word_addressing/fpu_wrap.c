#include <stdio.h>
#include <stdint.h>
#include "fpu/reg_t.h"
#include "fpu/fadd_s.h"
#include "fpu/fsub_s.h"
#include "fpu/fmul_s.h"
#include "fpu/fdiv_s.h"
#include "fpu/fsqrt_s.h"
#include "fpu/fcvt_s_w.h"
#include "fpu/flt_s.h"
#include "fpu/fle_s.h"
#include "fpu/fhalf.h"

reg_t convert_i_r(int32_t uo){
  reg_t x = {.x = (unsigned)uo};
  return x;
}

int32_t convert_r_i(reg_t uo){
  return ((int32_t)(uo.x));
}

int32_t fadd_s_wrap(int32_t rs1, int32_t rs2){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rs2 = convert_i_r(rs2);
  reg_t r_rd  = fadd_s(r_rs1, r_rs2);
  return convert_r_i(r_rd);
}

int32_t fsub_s_wrap(int32_t rs1, int32_t rs2){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rs2 = convert_i_r(rs2);
  reg_t r_rd  = fsub_s(r_rs1, r_rs2);
  return convert_r_i(r_rd);
}

int32_t fmul_s_wrap(int32_t rs1, int32_t rs2){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rs2 = convert_i_r(rs2);
  reg_t r_rd  = fmul_s(r_rs1, r_rs2);
  return convert_r_i(r_rd);
}

int32_t fdiv_s_wrap(int32_t rs1, int32_t rs2){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rs2 = convert_i_r(rs2);
  reg_t r_rd  = fdiv_s(r_rs1, r_rs2);
  return convert_r_i(r_rd);
}

int32_t fsqrt_s_wrap(int32_t rs1){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rd  = fsqrt_s(r_rs1);
  return convert_r_i(r_rd);
}

int32_t fcvt_s_w_wrap(int32_t rs1){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rd  = fcvt_s_w(r_rs1);
  return convert_r_i(r_rd);
}

int32_t flt_s_wrap(int32_t rs1, int32_t rs2){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rs2 = convert_i_r(rs2);
  reg_t r_rd  = flt_s(r_rs1, r_rs2);
  return convert_r_i(r_rd);
}

int32_t fle_s_wrap(int32_t rs1, int32_t rs2){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rs2 = convert_i_r(rs2);
  reg_t r_rd  = fle_s(r_rs1, r_rs2);
  return convert_r_i(r_rd);
}

int32_t fhalf_wrap(int32_t rs1){
  reg_t r_rs1 = convert_i_r(rs1);
  reg_t r_rd  = fhalf(r_rs1);
  return convert_r_i(r_rd);
}
