#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "parser.h"
#include "statistics.h"
#include "execution.h"
#include "assembler.h"
#include "assembly.h"

#define size_memory 131072 // text,data,heap,stack領域それぞれのword数(int32_t)。

int main(int argc, char *argv[]){
  if(init_data(size_memory)<0){
    fprintf(stderr, "memory allocation error\n");
    free_memory();
    return 0;
  }
  if(parse(argv[1])<0){
    fprintf(stderr, "parse error\n");
    free_memory();
    return 0;
  }
  fprintf(stderr, "parse complete\n");

  if(argc==2){
    if(matomete(0)<0){
      fprintf(stderr, "execution error\n");
      free_memory();
      return 0;
    }
  }
  else if(argc==3&&argv[2][0]!='-'){
    if(matomete(1)<0){
      fprintf(stderr, "execution error\n");
      free_memory();
      return 0;
    }
    if(show_statistics(argv[2])<0){
      fprintf(stderr, "failure in writing statistics data out\n");
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
          fprintf(stderr, "text assemble error\n");
          free_memory();
          return 0;
        }
        fprintf(stderr, "text assembled\n");
        fprintf(stderr, "min_caml_start address : %d\n", pc);
        output = output+1;
      }
      else if(op!=0&&argv[2][op]=='d'&&output<argc){
        if(data_assemble(argv[output])<0){
          fprintf(stderr, "data assemble error\n");
          free_memory();
          return 0;
        }
        fprintf(stderr, "data assembled\n");
        fprintf(stderr, "sorry for big-endian\n");
        output = output+1;
      }
      else if(op!=0&&argv[2][op]=='a'&&output<argc){
        if(post_parser(argv[output])<0){
          fprintf(stderr, "assembly error\n");
          free_memory();
          return 0;
        }
        fprintf(stderr, "reverse assembled\n");
        fprintf(stderr, "min_caml_start index   : %d\n", pc);
        output = output+1;
      }
      else if(op!=0&&argv[2][op]=='s'){
        char c = '\n';
        while(c=='\n'){
          c = getchar();
          if(step()<0){
            fprintf(stderr, "execution error\n");
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
