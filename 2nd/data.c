#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"

int32_t pc = 0;

int32_t registers[32] = {0};
char *abi_names[36] = {"zero","ra","sp","gp","tp","t0/hp","t1","t2","s0/fp","s1","a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6","t0","hp","s0","fp"};

int size_memory = 0; // 各領域のword数 = 配列のサイズ
struct instruction *text_memory = NULL;  // text領域, 実行時はread only。
int32_t *rest_memory[3] = {NULL};  // {data,heap,stack} のイメージ。

void show_registers(){
  for (size_t i=0; i<32; i++){
    printf("%s",abi_names[i]);
    if(i==0) printf(" ");
    else if(i==26||i==27) printf("  ");
    else if(i!=5&&i!=8) printf("   ");
    printf(" : %d\n",registers[i]);
  }
  return;
}

signed char init_data(int32_t size){
  size_memory = size;
  text_memory = (struct instruction *)malloc(sizeof(struct instruction)*size);
  if(text_memory==NULL){
    return -1;
  }
  for (size_t i=0; i<3; i++){
    rest_memory[i] = (int32_t *)malloc(sizeof(int32_t)*size);
  }
  for (size_t i=0; i<3; i++){
    if(rest_memory[i]==NULL){
      return -1;
    }
  }
  registers[3] = (size_memory*1)*4; // gp = data領域の先頭。本来は真ん中。
  registers[5] = (size_memory*2)*4; // hp = heap領域の先頭。
  registers[2] = (size_memory*3)*4; // sp = stack領域の先頭。本来は末尾。
  return 0;
}

void free_memory(){
  free(text_memory);
  for (size_t i=0; i<3; i++){
    free(rest_memory[i]);
  }
  return;
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

int32_t load_regster(int index){
  return registers[index];
}

void store_register(int index, int32_t value){
  registers[index] = value;
  return;
}

int index_text(int32_t program_counter){
  if(program_counter<0||size_memory*4<=program_counter||program_counter%4!=0){
    return -1;
  }
  return program_counter/4;
}

struct instruction load_text(int index){
  return text_memory[index];
}

void store_text(int index, struct instruction instruction){
  text_memory[index] = instruction;
  return;
}

int index_memory(int32_t address){
  if(address<size_memory*4||size_memory*4*4<=address||address%4!=0){
    return -1;
  }
  return address/4;
};

int32_t load_memory(int index){
  int32_t value = 0;
  for (size_t i=0; i<3; i++){
    if(index<size_memory*(i+2)){
      value = rest_memory[i][index-size_memory*(i+1)];
      break;
    }
  }
  return value;
}

void store_memory(int index, int32_t value){
  for (size_t i=0; i<3; i++){
    if(index<size_memory*(i+2)){
      rest_memory[i][index-size_memory*(i+1)] = value;
      break;
    }
  }
  return;
}
