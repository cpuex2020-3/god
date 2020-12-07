#include <stdio.h>
#include "reg_t.h"

#include "math.h"

reg_t fsqrt_s(reg_t x1){
  float value = sqrtf(x1.f);
  reg_t y = {.f = value};
  return y;
}
