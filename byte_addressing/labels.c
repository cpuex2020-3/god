#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "data.h"
#include "labels.h"

void add_list(struct label_list** li, char name[256], int32_t address){
  struct label_list* st;
  st = (struct label_list *)malloc(sizeof(struct label_list));
  int i = 0;
  for (; name[i]!='\0'; i++){
    (st->name)[i] = name[i];
  }
  (st->name)[i] = '\0';
  st->address = address;
  st->next = *li;
  *li = st;
  return;
}

int32_t search_list(struct label_list *li, char *name){
  if(li==NULL) return -1;
  else if(eqlstr(li->name,name)==0) return li->address;
  else return search_list(li->next,name);
}

void delete_list(struct label_list *li){
  if(li!=NULL){
    delete_list(li->next);
    free(li);
  }
  return;
}

/* 今回はlabel_textのみ集めたい。label_dataはめんどくさいので。*/
struct label_list* get_text_labels(char *file_name){

  int32_t text_address = pc+4;
  struct label_list *labels = NULL;
  char mode = 0; // .textは0, .dataは1。

  char s[256];
  int i_s = 0;

  char *name[2] = {"external.s", file_name};
  FILE *fp;

  for (size_t uo = 0; uo<2; uo++){
    fp = fopen(name[uo], "r");
    if(fp==NULL) return NULL;
    while(fgets(s,256,fp)!=NULL){

      /*　コメントアウト部分、ついでに末尾の改行も消しとこ */
      while(s[i_s]!='\0'){
        if(s[i_s]=='\n'||s[i_s]=='#'){
          i_s--;
          while(0<=i_s&&(s[i_s]==9||s[i_s]==32)) i_s--;
          s[i_s+1]='\0';
          break;
        }
        i_s++;
      }
      i_s = 0;

      /* 先頭の切り出し */
      char t[256];
      int i_t = 0;
      while(s[i_s]==9||s[i_s]==32) i_s++;
      while(s[i_s]!=9&&s[i_s]!=32&&s[i_s]!='\n'&&s[i_s]!='\0'){
        t[i_t] = s[i_s];
        i_s++; i_t++;
      }
      t[i_t] = '\0';
      while(s[i_s]==9||s[i_s]==32) i_s++;

      /* 切り出した先頭を見て、label:, directive, instruction に場合分け */
      if(t[0]=='\0'){
      }
      else if(t[i_t-1]==':'){
        t[i_t-1] = '\0';
        if(mode==0) add_list(&labels, t, text_address);
      }
      else if(t[0]=='.'){
        if(eqlstr(t,".data")==0) mode = 1;
        else if(eqlstr(t,".text")==0) mode = 0;
      }
      else if(mode==0){
        if(eqlstr(t,"mod10")==0){
          text_address = text_address+12;
        }
        else if(t[0]=='b'||eqlstr(t,"jal")==0||eqlstr(t,"j")==0){
          text_address = text_address+8;
        }
        else if(eqlstr(t,"li")==0||eqlstr(t,"la")==0){
          text_address = text_address+8;
        }
        else{
          text_address = text_address+4;
        }
      }

      i_s = 0;
    }
    fclose(fp);
  }

  return labels;
}
