#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "parser.h"
#include "execution.h"
#include "assembler.h"
#include "assembly.h"

#define size_memory 131072 // text,data,heap,stack領域それぞれのword数(int32_t)。

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

  if(argc==2){
    if(matomete()<0){
      printf("execution error\n");
      free_memory();
      return 0;
    }
  }
  else{
    int op = 0, output = 3;
    while(argv[2][op]!='\0'){
      if(op==0&&argv[2][op]!='-'){
        break;
      }
      else if(op!=0&&argv[2][op]=='b'&&output<argc){
        if(assemble(argv[output])<0){
          printf("text assemble error\n");
          free_memory();
          return 0;
        }
        printf("text assembled\n");
        printf("min_caml_start address : %d\n", pc);
        output = output+1;
      }
      else if(op!=0&&argv[2][op]=='d'&&output<argc){
        if(data_assemble(argv[output])<0){
          printf("data assemble error\n");
          free_memory();
          return 0;
        }
        printf("data assembled\n");
        printf("sorry for big-endian\n");
        output = output+1;
      }
      else if(op!=0&&argv[2][op]=='a'&&output<argc){
        if(post_parser(argv[output])<0){
          printf("assembly error\n");
          free_memory();
          return 0;
        }
        printf("min_caml_start index   : %d\n", pc/4);
      }
      else if(op!=0&&argv[2][op]=='s'){
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
      else{
      }
      op = op+1;
    }
  }

  free_memory();
  return 0;
}
