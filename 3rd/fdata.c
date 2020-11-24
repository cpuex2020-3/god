#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "data.h"

const int32_t rm = 0b000;
int32_t fcsr = 0x00000000|(rm<<5);

int32_t f_registers[32] = {0};
char *f_abi_names[32] = {"ft0","ft1","ft2","ft3","ft4","ft5","ft6","ft7","fs0","fs1","fa0","fa1","fa2","fa3","fa4","fa5","fa6","fa7","fs2","fs3","fs4","fs5","fs6","fs7","fs8","fs9","fs10","fs11","ft8","ft9","ft10","ft11"};

void f_show_registers(){
  for (size_t i=0; i<32; i++){
    printf("%s ",f_abi_names[i]);
    if(i!=26&&i!=27&&i!=30&&i!=31){
      printf(" ");
    }
    printf(" : %f (0x%08x)\n", *((float *)(f_registers+i)), f_registers[i]);
  }
  return;
}

int f_index_register(char *name){
  for(int i=0; i<32; i++){
    if(eqlstr(name,f_abi_names[i])==0){
      return i;
    }
  }
  return -1;
}

float f_load_regster(int index){
  return *((float *)(f_registers+index));
}

void f_store_register(int index, float value){
  f_registers[index] = *((int *)&value);
  return;
}

void f_load_memory(int index_add, int index_f_reg){
  f_registers[index_f_reg] = load_memory(index_add);
  return;
}

void f_store_memory(int index_add, int index_f_reg){
  store_memory(index_add, f_registers[index_f_reg]);
  return;
}
