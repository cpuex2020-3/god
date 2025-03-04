#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"

typedef struct int32_or_indeterminate {
  int32_t value;
  signed char indeterminate;
} int32_ind;

int32_t pc = 0;
int32_t uart = 0;

int32_ind registers[32] = { {0,0}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1} };
char *abi_names[36] = {"zero","ra","sp","gp","tp","t0/hp","t1","t2","s0/fp","s1","a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6","t0","hp","s0","fp"};

int size_memory = 0; // 各領域のword数 = 配列のサイズ。
struct instruction *text_memory = NULL;  // text領域, 実行時はread only。
int32_ind *rest_memory[3] = { NULL };  // {data,heap,stack} のイメージ。

void show_registers(){
  for (size_t i=0; i<32; i++){
    printf("%s",abi_names[i]);
    if(i==0) printf(" ");
    else if(i==26||i==27) printf("  ");
    else if(i!=5&&i!=8) printf("   ");
    if(registers[i].indeterminate==1) printf(" : XXXX\n");
    else printf(" : %d\n", registers[i].value);
  }
  return ;
}

signed char init_data(int32_t size){
  size_memory = size;
  text_memory = (struct instruction *)malloc(sizeof(struct instruction)*size);
  if(text_memory==NULL){
    return -1;
  }
  for(size_t j=0; j<size; j++){
    text_memory[j].opcode = 0b1111111;
  }
  for (size_t i=0; i<3; i++){
    rest_memory[i] = (int32_ind *)malloc(sizeof(int32_ind)*size);
    if(rest_memory[i]==NULL){
      return -1;
    }
    for(size_t j=0; j<size; j++){
      rest_memory[i][j].value = 0;
      rest_memory[i][j].indeterminate = 1;
    }
  }
  registers[1].value = 0;
  registers[3].value = size_memory*0; // gp = data領域の先頭。本来は真ん中。
  registers[5].value = size_memory*1; // hp = heap領域の先頭。
  registers[2].value = size_memory*2; // sp = stack領域の先頭。本来は末尾。
  registers[1].indeterminate = 0;
  registers[3].indeterminate = 0;
  registers[5].indeterminate = 0;
  registers[2].indeterminate = 0;
  return 0;
}

void free_memory(){
  free(text_memory);
  for (size_t i=0; i<3; i++){
    free(rest_memory[i]);
  }
  return ;
}

int index_register(char *name){
  int i = 0;
  while(i<36&&eqlstr(name,abi_names[i])<0){
    i++;
  }
  if(i==36){
    return -1;
  }
  else if(i==32||i==33){
    i = 5;
  }
  else if(i==34||i==35){
    i = 8;
  }
  return i;
}

char *reverse_register(int index){
  if(index==5){
    return abi_names[32];
  }
  else if(index==8){
    return abi_names[34];
  }
  else{
    return abi_names[index];
  }
}

int32_t load_register(int index, signed char *indeterminate){
  *indeterminate = registers[index].indeterminate;
  return registers[index].value;
}

void store_register(int index, int32_t value, signed char indeterminate){
  if(index!=0){
    registers[index].value = value;
    registers[index].indeterminate = indeterminate;
  }
  return ;
}

int index_text(int32_t program_counter){
  if(program_counter<0||size_memory<=program_counter){
    return -1;
  }
  return program_counter;
}

struct instruction load_text(int index){
  return text_memory[index];
}

void store_text(int index, struct instruction instruction){
  text_memory[index] = instruction;
  return ;
}

int index_memory(int32_t address){
  if(address<0||size_memory*3<=address){
    return -1;
  }
  return address;
};

int32_t load_memory(int index, signed char *indeterminate){
  int32_t value = 0;
  for (size_t i=0; i<3; i++){
    if(index<size_memory*(i+1)){
      value = rest_memory[i][index-size_memory*i].value;
      *indeterminate = rest_memory[i][index-size_memory*i].indeterminate;
      break;
    }
  }
  return value;
}

void store_memory(int index, int32_t value, signed char indeterminate){
  for (size_t i=0; i<3; i++){
    if(index<size_memory*(i+1)){
      rest_memory[i][index-size_memory*i].value = value;
      rest_memory[i][index-size_memory*i].indeterminate = indeterminate;
      break;
    }
  }
  return ;
}

// ここから浮動小数点数。

const int32_t rm = 0b000;
int32_t fcsr = 0x00000000|(rm<<5);

int32_ind f_registers[32] = { {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,0}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1} };
char *f_abi_names[33] = {"ft0","ft1","ft2","ft3","ft4","ft5","ft6","ft7","fs0","fs1","fa0","fa1","fa2","fa3","fa4","fa5","fa6","fa7","fzero","fs3","fs4","fs5","fs6","fs7","fs8","fs9","fs10","fs11","ft8","ft9","ft10","ft11", "fs2"};

void f_show_registers(){
  for (size_t i=0; i<32; i++){
    printf("%s ",f_abi_names[i]);
    if(i!=18){
      printf(" ");
    }
    if(i!=26&&i!=27&&i!=30&&i!=31){
      printf(" ");
    }
    if(f_registers[i].indeterminate==1) printf("XXXX\n");
    else printf(" : %f (0x%08x)\n", *((float *)(&(f_registers[i].value))), f_registers[i].value);
  }
  return ;
}

int f_index_register(char *name){
  for(int i=0; i<33; i++){
    if(eqlstr(name,f_abi_names[i])==0){
      if(i==32) i = 18;
      return i;
    }
  }
  return -1;
}

char *f_reverse_register(int index){
  return f_abi_names[index];
}

int32_t f_load_register(int index, signed char *indeterminate){
  *indeterminate = f_registers[index].indeterminate;
  return f_registers[index].value;
}

void f_store_register(int index, int32_t value, signed char indeterminate){
  if(index!=18){
    f_registers[index].value = value;
    f_registers[index].indeterminate = indeterminate;
  }
  return ;
}
