#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "parser.h"
#include "execution.h"
#include "assembler.h"
#include "assembly.h"

#define size_memory 10000 // text,data,heap,stack領域それぞれのword数(int32_t)。

int main(int argc, char *argv[]){
  if(init_data(size_memory)<0){
    printf("memory allocation error\n");
    free_memory();
    return 0;
  }

  if(parse(argv[1])<0){
    printf("parse error\n");
    free_memory();
    return 0;
  }

  if(argc>2){
    if(assemble(argv[2])<0){
      printf("assemble error\n");
      free_memory();
      return 0;
    }
    if(argc==4){
      if(post_parser(argv[3])<0){
        printf("assembly error\n");
        free_memory();
        return 0;
      }
    }
    printf("main : %d\n", pc);
  }

  else{
    char c = '\n';
    while(c=='\n'){
      c = getchar();
      if(step()<0){
        printf("execution error\n");
        free_memory();
        return 0;
      }
    }
  }

  free_memory();
  return 0;
}
