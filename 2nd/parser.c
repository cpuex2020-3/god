#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "data.h"

struct label_list {
  char name[256];
  int32_t address;
  struct label_list *next;
};

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

int32_t text_address = 0;
int32_t data_address = 0;
void init_parser(){
  struct instruction halt;
  halt.opcode = 0b0000000;
  /* pcとraの初期値は0です。*/
  store_text(index_register("ra"), halt);
  text_address = pc+4;
  data_address = load_regster(index_register("gp"));
  return;
}

char s[256];
int i_s = 0;
char mode = 0;  // .textは0, .dataは1, それ以外はエラー。
struct label_list *labels = NULL;

void white_skip(){
  while(s[i_s]==9||s[i_s]==32) i_s++;
  return;
}

// operand がなければ-1、','で区切れたら0, '\0'で区切れたら1を返す。
signed char operand(char (*rand)[256]){
  int i = 0;
  for (; s[i_s]!=','&&s[i_s]!='\0'; i++,i_s++) (*rand)[i]=s[i_s];
  if(i==0) return -1;
  (*rand)[i]='\0';
  if(s[i_s]=='\0') return 1;
  i_s++;
  white_skip();
  return 0;
}

signed char operands_ls(char (*rand0)[256], char (*rand1)[256], char (*rand2)[256]){
  int i = 0;
  for (; s[i_s]!=','&&s[i_s]!='\0'; i++,i_s++) (*rand0)[i]=s[i_s];
  (*rand0)[i]='\0';
  if(s[i_s]=='\0') return -1;
  i_s++;
  white_skip();
  for (i=0; s[i_s]!='('&&s[i_s]!='\0'; i++,i_s++) (*rand1)[i]=s[i_s];
  (*rand1)[i]='\0';
  if(s[i_s]=='\0') return -1;
  i_s++;
  white_skip();
  for (i=0; s[i_s]!=')'&&s[i_s]!='\0'; i++,i_s++) (*rand2)[i]=s[i_s];
  (*rand2)[i]='\0';
  return 0;
}

/* immidiate枠には次の6種類がある。
          imm[11:0]  (基本はこれ),
          shamt[4:0] (shift系),
          imm[31:12] (lui,aupci),
          imm[12:1]  (branch系),
          imm[20:1]  (jal),
          imm[31:0]  (li)
   次の関数では、bit = 11, 4, 19, 12, 20, 31 で場合分けしています。1048576 は 1<<20 の値。*/
int32_t immediate(char *imm, int32_t bit){
  int i = 0, j = 0;
  if(imm[0]=='\0') return 1048576;
  if(imm[0]=='-') j++;
  for (i = j; imm[i]!='\0'; i++){
    if(imm[i]<48||57<imm[i]) return 1048576;
  }
  if(i==j) return 1048576;
  int32_t value = atoi(imm);

  if(bit==4){
    if(value<0||32<=value) return 1048576;
  }
  else if(bit!=31){
    int32_t up_bd = 1 << bit;
    if(value<-1*up_bd||up_bd<=value) return 1048576;
    if((bit==12||bit==20)&&value%2==1) return 1048576;
  }
  return value;
}

signed char labeling(char t[256]){
  int i_t = 0;
  while(t[i_t]!=':') i_t++;
  t[i_t] = '\0';
  // indexのインクリメントは実際にメモリに書き込むとき。
  if(mode==0) add_list(&labels, t, text_address);
  else if(mode==1) add_list(&labels, t, data_address);
  return 0;
}

/* 増やそうな */
signed char directive(char t[256]){
  if(eqlstr(t,".data")==0) mode = 1;
  else if(eqlstr(t,".text")==0) mode = 0;
  else if(eqlstr(t,".align")==0){
    /* エラー処理だけして読み飛ばしてる。 */
    char imm[256];
    if(operand(&imm)!=1) return -1;
    int32_t i = immediate(imm,32);
    if(eqlstr(imm,"1048576")!=0&&i==1048576) return -1;
    if(i<=0) return -1;
  }
  else if(eqlstr(t,".balign")==0){
    /* エラー処理だけして読み飛ばしてる。 */
    char imm[256];
    if(operand(&imm)!=1) return -1;
    int32_t i = immediate(imm,31);
    if(eqlstr(imm,"1048576")!=0&&i==1048576) return -1;
    if(i<=0) return -1;
  }
  else if(eqlstr(t,".globl")==0){
    /* 外部関数実装時に書くか */
  }
  else if(eqlstr(t,".word")==0){
    /* 書くだけ。はよ書け。これ書かな la 実装しても使われへん。*/
    /*
    char imm[256];
    signed char i = operand(&imm);
    while()
    */
  }
  return 0;
}

signed char instruction(char t[256]){
  if(mode==1) return -1;
  else if(eqlstr(t,"lw")==0){
    char rd[256],rs1[256],imm[256];
    if(operands_ls(&rd, &imm, &rs1)<0) return -1;
    struct instruction type_I;
    type_I.opcode = 0b0000011;
    type_I.funct3 = 0b010;
    type_I.rd_index = index_register(rd);
    type_I.rs1_index = index_register(rs1);
    type_I.imm = immediate(imm,11);
    int i = index_text(text_address);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"sw")==0){
    char rs1[256],rs2[256],imm[256];
    if(operands_ls(&rs2, &imm, &rs1)<0) return -1;
    struct instruction type_S;
    type_S.opcode = 0b0100011;
    type_S.funct3 = 0b010;
    type_S.rs1_index = index_register(rs1);
    type_S.rs2_index = index_register(rs2);
    type_S.imm = immediate(imm,11);
    int i = index_text(text_address);
    if(i<0||type_S.rs1_index<0||type_S.rs2_index<0||type_S.imm==1048576) return -1;
    store_text(i,type_S);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"sll")==0){
    char rd[256],rs1[256],rs2[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&rs2)!=1) return -1;
    struct instruction type_R;
    type_R.opcode = 0b0110011;
    type_R.funct3 = 0b001;
    type_R.funct7 = 0b0000000;
    type_R.rd_index = index_register(rd);
    type_R.rs1_index = index_register(rs1);
    type_R.rs2_index = index_register(rs2);
    int i = index_text(text_address);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"slli")==0){
    char rd[256],rs1[256],imm[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&imm)!=1) return -1;
    struct instruction type_I;
    type_I.opcode = 0b0010011;
    type_I.funct3 = 0b001;
    type_I.funct7 = 0b0000000;
    type_I.rd_index = index_register(rd);
    type_I.rs1_index = index_register(rs1);
    type_I.imm = immediate(imm,4);
    int i = index_text(text_address);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"srl")==0){
    char rd[256],rs1[256],rs2[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&rs2)!=1) return -1;
    struct instruction type_R;
    type_R.opcode = 0b0110011;
    type_R.funct3 = 0b101;
    type_R.funct7 = 0b0000000;
    type_R.rd_index = index_register(rd);
    type_R.rs1_index = index_register(rs1);
    type_R.rs2_index = index_register(rs2);
    int i = index_text(text_address);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"srli")==0){
    char rd[256],rs1[256],imm[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&imm)!=1) return -1;
    struct instruction type_I;
    type_I.opcode = 0b0010011;
    type_I.funct3 = 0b101;
    type_I.funct7 = 0b0000000;
    type_I.rd_index = index_register(rd);
    type_I.rs1_index = index_register(rs1);
    type_I.imm = immediate(imm,4);
    int i = index_text(text_address);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"sra")==0){
    char rd[256],rs1[256],rs2[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&rs2)!=1) return -1;
    struct instruction type_R;
    type_R.opcode = 0b0110011;
    type_R.funct3 = 0b101;
    type_R.funct7 = 0b0100000;
    type_R.rd_index = index_register(rd);
    type_R.rs1_index = index_register(rs1);
    type_R.rs2_index = index_register(rs2);
    int i = index_text(text_address);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"srai")==0){
    char rd[256],rs1[256],imm[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&imm)!=1) return -1;
    struct instruction type_I;
    type_I.opcode = 0b0010011;
    type_I.funct3 = 0b101;
    type_I.funct7 = 0b0100000;
    type_I.rd_index = index_register(rd);
    type_I.rs1_index = index_register(rs1);
    type_I.imm = immediate(imm,4);
    int i = index_text(text_address);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"add")==0){
    char rd[256],rs1[256],rs2[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&rs2)!=1) return -1;
    struct instruction type_R;
    type_R.opcode = 0b0110011;
    type_R.funct3 = 0b000;
    type_R.funct7 = 0b0000000;
    type_R.rd_index = index_register(rd);
    type_R.rs1_index = index_register(rs1);
    type_R.rs2_index = index_register(rs2);
    int i = index_text(text_address);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"addi")==0){
    char rd[256],rs1[256],imm[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&imm)!=1) return -1;
    struct instruction type_I;
    type_I.opcode = 0b0010011;
    type_I.funct3 = 0b000;
    type_I.rd_index = index_register(rd);
    type_I.rs1_index = index_register(rs1);
    type_I.imm = immediate(imm,11);
    int i = index_text(text_address);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"sub")==0){
    char rd[256],rs1[256],rs2[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&rs2)!=1) return -1;
    struct instruction type_R;
    type_R.opcode = 0b0110011;
    type_R.funct3 = 0b000;
    type_R.funct7 = 0b0100000;
    type_R.rd_index = index_register(rd);
    type_R.rs1_index = index_register(rs1);
    type_R.rs2_index = index_register(rs2);
    int i = index_text(text_address);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"lui")==0){
    char rd[256],imm[256];
    if(operand(&rd)!=0||operand(&imm)!=1) return -1;
    struct instruction type_U;
    type_U.opcode = 0b0110111;
    type_U.rd_index = index_register(rd);
    type_U.imm = immediate(imm,19);
    int i = index_text(text_address);
    if(i<0||type_U.rd_index<0||type_U.imm==1048576) return -1;
    store_text(i,type_U);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"auipc")==0){
    char rd[256],imm[256];
    if(operand(&rd)!=0||operand(&imm)!=1) return -1;
    struct instruction type_U;
    type_U.opcode = 0b0010111;
    type_U.rd_index = index_register(rd);
    type_U.imm = immediate(imm,19);
    int i = index_text(text_address);
    if(i<0||type_U.rd_index<0||type_U.imm==1048576) return -1;
    store_text(i,type_U);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"xor")==0){
    char rd[256],rs1[256],rs2[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&rs2)!=1) return -1;
    struct instruction type_R;
    type_R.opcode = 0b0110011;
    type_R.funct3 = 0b100;
    type_R.funct7 = 0b0000000;
    type_R.rd_index = index_register(rd);
    type_R.rs1_index = index_register(rs1);
    type_R.rs2_index = index_register(rs2);
    int i = index_text(text_address);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"xori")==0){
    char rd[256],rs1[256],imm[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&imm)!=1) return -1;
    struct instruction type_I;
    type_I.opcode = 0b0010011;
    type_I.funct3 = 0b100;
    type_I.rd_index = index_register(rd);
    type_I.rs1_index = index_register(rs1);
    type_I.imm = immediate(imm,11);
    int i = index_text(text_address);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"or")==0){
    char rd[256],rs1[256],rs2[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&rs2)!=1) return -1;
    struct instruction type_R;
    type_R.opcode = 0b0110011;
    type_R.funct3 = 0b110;
    type_R.funct7 = 0b0000000;
    type_R.rd_index = index_register(rd);
    type_R.rs1_index = index_register(rs1);
    type_R.rs2_index = index_register(rs2);
    int i = index_text(text_address);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"ori")==0){
    char rd[256],rs1[256],imm[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&imm)!=1) return -1;
    struct instruction type_I;
    type_I.opcode = 0b0010011;
    type_I.funct3 = 0b110;
    type_I.rd_index = index_register(rd);
    type_I.rs1_index = index_register(rs1);
    type_I.imm = immediate(imm,11);
    int i = index_text(text_address);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"and")==0){
    char rd[256],rs1[256],rs2[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&rs2)!=1) return -1;
    struct instruction type_R;
    type_R.opcode = 0b0110011;
    type_R.funct3 = 0b111;
    type_R.funct7 = 0b0000000;
    type_R.rd_index = index_register(rd);
    type_R.rs1_index = index_register(rs1);
    type_R.rs2_index = index_register(rs2);
    int i = index_text(text_address);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"andi")==0){
    char rd[256],rs1[256],imm[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&imm)!=1) return -1;
    struct instruction type_I;
    type_I.opcode = 0b0010011;
    type_I.funct3 = 0b111;
    type_I.rd_index = index_register(rd);
    type_I.rs1_index = index_register(rs1);
    type_I.imm = immediate(imm,11);
    int i = index_text(text_address);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"slt")==0){
    char rd[256],rs1[256],rs2[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&rs2)!=1) return -1;
    struct instruction type_R;
    type_R.opcode = 0b0110011;
    type_R.funct3 = 0b010;
    type_R.funct7 = 0b0000000;
    type_R.rd_index = index_register(rd);
    type_R.rs1_index = index_register(rs1);
    type_R.rs2_index = index_register(rs2);
    int i = index_text(text_address);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"slti")==0){
    char rd[256],rs1[256],imm[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&imm)!=1) return -1;
    struct instruction type_I;
    type_I.opcode = 0b0010011;
    type_I.funct3 = 0b010;
    type_I.rd_index = index_register(rd);
    type_I.rs1_index = index_register(rs1);
    type_I.imm = immediate(imm,11);
    int i = index_text(text_address);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"sltu")==0){
    char rd[256],rs1[256],rs2[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&rs2)!=1) return -1;
    struct instruction type_R;
    type_R.opcode = 0b0110011;
    type_R.funct3 = 0b011;
    type_R.funct7 = 0b0000000;
    type_R.rd_index = index_register(rd);
    type_R.rs1_index = index_register(rs1);
    type_R.rs2_index = index_register(rs2);
    int i = index_text(text_address);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"sltiu")==0){
    char rd[256],rs1[256],imm[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&imm)!=1) return -1;
    struct instruction type_I;
    type_I.opcode = 0b0010011;
    type_I.funct3 = 0b011;
    type_I.rd_index = index_register(rd);
    type_I.rs1_index = index_register(rs1);
    type_I.imm = immediate(imm,11);
    int i = index_text(text_address);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"beq")==0){
    char rs1[256],rs2[256],imm[256];
    if(operand(&rs1)!=0||operand(&rs2)!=0||operand(&imm)!=1) return -1;
    int32_t im = immediate(imm,12);
    if(im!=1048576){
      struct instruction type_B;
      type_B.opcode = 0b1100011;
      type_B.funct3 = 0b000;
      type_B.rs1_index = index_register(rs1);
      type_B.rs2_index = index_register(rs2);
      type_B.imm = im;
      int i = index_text(text_address);
      if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
      store_text(i,type_B);
      text_address = text_address+4;
    }
    else{
      int32_t address = search_list(labels,imm);
      if(address<0) return -1;
      int32_t distance = address - text_address;
      // beqで飛べる場合は beq rs1, rs2, distance。
      if(-4096<=distance&&distance<4096){
        struct instruction type_B;
        type_B.opcode = 0b1100011;
        type_B.funct3 = 0b000;
        type_B.rs1_index = index_register(rs1);
        type_B.rs2_index = index_register(rs2);
        type_B.imm = distance;
        int i = index_text(text_address);
        if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
        store_text(i,type_B);
        text_address = text_address+4;
      }
      // 飛べない場合は auipc t1, uo -> beq rs1, rs2, uouo。
      else{
        distance = distance-4;
        int32_t dis_lui = distance>>12;
        struct instruction type_U;
        type_U.opcode = 0b0010111;
        type_U.rd_index = index_register("t1");
        type_U.imm = dis_lui;
        int i = index_text(text_address);
        if(i<0) return -1;
        store_text(i,type_U);
        text_address = text_address+4;
        struct instruction type_B;
        type_B.opcode = 0b1100011;
        type_B.funct3 = 0b000;
        type_B.rs1_index = index_register(rs1);
        type_B.rs2_index = index_register(rs2);
        type_B.imm = distance-(dis_lui<<12);
        i = index_text(text_address);
        if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
        store_text(i,type_B);
        text_address = text_address+4;
      }
    }
  }
  else if(eqlstr(t,"bne")==0){
    char rs1[256],rs2[256],imm[256];
    if(operand(&rs1)!=0||operand(&rs2)!=0||operand(&imm)!=1) return -1;
    int32_t im = immediate(imm,12);
    if(im!=1048576){
      struct instruction type_B;
      type_B.opcode = 0b1100011;
      type_B.funct3 = 0b001;
      type_B.rs1_index = index_register(rs1);
      type_B.rs2_index = index_register(rs2);
      type_B.imm = im;
      int i = index_text(text_address);
      if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
      store_text(i,type_B);
      text_address = text_address+4;
    }
    else{
      int32_t address = search_list(labels,imm);
      if(address<0) return -1;
      int32_t distance = address - text_address;
      // beqで飛べる場合は bne rs1, rs2, distance。
      if(-4096<=distance&&distance<4096){
        struct instruction type_B;
        type_B.opcode = 0b1100011;
        type_B.funct3 = 0b001;
        type_B.rs1_index = index_register(rs1);
        type_B.rs2_index = index_register(rs2);
        type_B.imm = distance;
        int i = index_text(text_address);
        if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
        store_text(i,type_B);
        text_address = text_address+4;
      }
      // 飛べない場合は auipc t1, uo -> bne rs1, rs2, uouo。
      else{
        distance = distance-4;
        int32_t dis_lui = distance>>12;
        struct instruction type_U;
        type_U.opcode = 0b0010111;
        type_U.rd_index = index_register("t1");
        type_U.imm = dis_lui;
        int i = index_text(text_address);
        if(i<0) return -1;
        store_text(i,type_U);
        text_address = text_address+4;
        struct instruction type_B;
        type_B.opcode = 0b1100011;
        type_B.funct3 = 0b001;
        type_B.rs1_index = index_register(rs1);
        type_B.rs2_index = index_register(rs2);
        type_B.imm = distance-(dis_lui<<12);
        i = index_text(text_address);
        if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
        store_text(i,type_B);
        text_address = text_address+4;
      }
    }
  }
  else if(eqlstr(t,"blt")==0){
    char rs1[256],rs2[256],imm[256];
    if(operand(&rs1)!=0||operand(&rs2)!=0||operand(&imm)!=1) return -1;
    int32_t im = immediate(imm,12);
    if(im!=1048576){
      struct instruction type_B;
      type_B.opcode = 0b1100011;
      type_B.funct3 = 0b100;
      type_B.rs1_index = index_register(rs1);
      type_B.rs2_index = index_register(rs2);
      type_B.imm = im;
      int i = index_text(text_address);
      if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
      store_text(i,type_B);
      text_address = text_address+4;
    }
    else{
      int32_t address = search_list(labels,imm);
      if(address<0) return -1;
      int32_t distance = address - text_address;
      // beqで飛べる場合は blt rs1, rs2, distance。
      if(-4096<=distance&&distance<4096){
        struct instruction type_B;
        type_B.opcode = 0b1100011;
        type_B.funct3 = 0b100;
        type_B.rs1_index = index_register(rs1);
        type_B.rs2_index = index_register(rs2);
        type_B.imm = distance;
        int i = index_text(text_address);
        if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
        store_text(i,type_B);
        text_address = text_address+4;
      }
      // 飛べない場合は auipc t1, uo -> blt rs1, rs2, uouo。
      else{
        distance = distance-4;
        int32_t dis_lui = distance>>12;
        struct instruction type_U;
        type_U.opcode = 0b0010111;
        type_U.rd_index = index_register("t1");
        type_U.imm = dis_lui;
        int i = index_text(text_address);
        if(i<0) return -1;
        store_text(i,type_U);
        text_address = text_address+4;
        struct instruction type_B;
        type_B.opcode = 0b1100011;
        type_B.funct3 = 0b100;
        type_B.rs1_index = index_register(rs1);
        type_B.rs2_index = index_register(rs2);
        type_B.imm = distance-(dis_lui<<12);
        i = index_text(text_address);
        if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
        store_text(i,type_B);
        text_address = text_address+4;
      }
    }
  }
  else if(eqlstr(t,"bge")==0){
    char rs1[256],rs2[256],imm[256];
    if(operand(&rs1)!=0||operand(&rs2)!=0||operand(&imm)!=1) return -1;
    int32_t im = immediate(imm,12);
    if(im!=1048576){
      struct instruction type_B;
      type_B.opcode = 0b1100011;
      type_B.funct3 = 0b101;
      type_B.rs1_index = index_register(rs1);
      type_B.rs2_index = index_register(rs2);
      type_B.imm = im;
      int i = index_text(text_address);
      if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
      store_text(i,type_B);
      text_address = text_address+4;
    }
    else{
      int32_t address = search_list(labels,imm);
      if(address<0) return -1;
      int32_t distance = address - text_address;
      // beqで飛べる場合は bge rs1, rs2, distance。
      if(-4096<=distance&&distance<4096){
        struct instruction type_B;
        type_B.opcode = 0b1100011;
        type_B.funct3 = 0b101;
        type_B.rs1_index = index_register(rs1);
        type_B.rs2_index = index_register(rs2);
        type_B.imm = distance;
        int i = index_text(text_address);
        if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
        store_text(i,type_B);
        text_address = text_address+4;
      }
      // 飛べない場合は auipc t1, uo -> bge rs1, rs2, uouo。
      else{
        distance = distance-4;
        int32_t dis_lui = distance>>12;
        struct instruction type_U;
        type_U.opcode = 0b0010111;
        type_U.rd_index = index_register("t1");
        type_U.imm = dis_lui;
        int i = index_text(text_address);
        if(i<0) return -1;
        store_text(i,type_U);
        text_address = text_address+4;
        struct instruction type_B;
        type_B.opcode = 0b1100011;
        type_B.funct3 = 0b101;
        type_B.rs1_index = index_register(rs1);
        type_B.rs2_index = index_register(rs2);
        type_B.imm = distance-(dis_lui<<12);
        i = index_text(text_address);
        if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
        store_text(i,type_B);
        text_address = text_address+4;
      }
    }
  }
  else if(eqlstr(t,"bltu")==0){
    char rs1[256],rs2[256],imm[256];
    if(operand(&rs1)!=0||operand(&rs2)!=0||operand(&imm)!=1) return -1;
    int32_t im = immediate(imm,12);
    if(im!=1048576){
      struct instruction type_B;
      type_B.opcode = 0b1100011;
      type_B.funct3 = 0b110;
      type_B.rs1_index = index_register(rs1);
      type_B.rs2_index = index_register(rs2);
      type_B.imm = im;
      int i = index_text(text_address);
      if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
      store_text(i,type_B);
      text_address = text_address+4;
    }
    else{
      int32_t address = search_list(labels,imm);
      if(address<0) return -1;
      int32_t distance = address - text_address;
      // beqで飛べる場合は bltu rs1, rs2, distance。
      if(-4096<=distance&&distance<4096){
        struct instruction type_B;
        type_B.opcode = 0b1100011;
        type_B.funct3 = 0b110;
        type_B.rs1_index = index_register(rs1);
        type_B.rs2_index = index_register(rs2);
        type_B.imm = distance;
        int i = index_text(text_address);
        if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
        store_text(i,type_B);
        text_address = text_address+4;
      }
      // 飛べない場合は auipc t1, uo -> bltu rs1, rs2, uouo。
      else{
        distance = distance-4;
        int32_t dis_lui = distance>>12;
        struct instruction type_U;
        type_U.opcode = 0b0010111;
        type_U.rd_index = index_register("t1");
        type_U.imm = dis_lui;
        int i = index_text(text_address);
        if(i<0) return -1;
        store_text(i,type_U);
        text_address = text_address+4;
        struct instruction type_B;
        type_B.opcode = 0b1100011;
        type_B.funct3 = 0b110;
        type_B.rs1_index = index_register(rs1);
        type_B.rs2_index = index_register(rs2);
        type_B.imm = distance-(dis_lui<<12);
        i = index_text(text_address);
        if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
        store_text(i,type_B);
        text_address = text_address+4;
      }
    }
  }
  else if(eqlstr(t,"bgeu")==0){
    char rs1[256],rs2[256],imm[256];
    if(operand(&rs1)!=0||operand(&rs2)!=0||operand(&imm)!=1) return -1;
    int32_t im = immediate(imm,12);
    if(im!=1048576){
      struct instruction type_B;
      type_B.opcode = 0b1100011;
      type_B.funct3 = 0b111;
      type_B.rs1_index = index_register(rs1);
      type_B.rs2_index = index_register(rs2);
      type_B.imm = im;
      int i = index_text(text_address);
      if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
      store_text(i,type_B);
      text_address = text_address+4;
    }
    else{
      int32_t address = search_list(labels,imm);
      if(address<0) return -1;
      int32_t distance = address - text_address;
      // beqで飛べる場合は bgeu rs1, rs2, distance。
      if(-4096<=distance&&distance<4096){
        struct instruction type_B;
        type_B.opcode = 0b1100011;
        type_B.funct3 = 0b111;
        type_B.rs1_index = index_register(rs1);
        type_B.rs2_index = index_register(rs2);
        type_B.imm = distance;
        int i = index_text(text_address);
        if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
        store_text(i,type_B);
        text_address = text_address+4;
      }
      // 飛べない場合は auipc t1, uo -> bgeu rs1, rs2, uouo。
      else{
        distance = distance-4;
        int32_t dis_lui = distance>>12;
        struct instruction type_U;
        type_U.opcode = 0b0010111;
        type_U.rd_index = index_register("t1");
        type_U.imm = dis_lui;
        int i = index_text(text_address);
        if(i<0) return -1;
        store_text(i,type_U);
        text_address = text_address+4;
        struct instruction type_B;
        type_B.opcode = 0b1100011;
        type_B.funct3 = 0b111;
        type_B.rs1_index = index_register(rs1);
        type_B.rs2_index = index_register(rs2);
        type_B.imm = distance-(dis_lui<<12);
        i = index_text(text_address);
        if(i<0||type_B.rs1_index<0||type_B.rs2_index<0) return -1;
        store_text(i,type_B);
        text_address = text_address+4;
      }
    }
  }
  else if(eqlstr(t,"jal")==0){
    char rd[256],imm[256];
    signed char branch = operand(&rd);
    if(branch==-1) return -1;
    else if(branch==0){
      if(operand(&imm)!=1) return -1;
      struct instruction type_J;
      type_J.opcode = 0b1101111;
      type_J.rd_index = index_register(rd);
      type_J.imm = immediate(imm,20);
      int i = index_text(text_address);
      if(i<0||type_J.rd_index<0||type_J.imm==1048576) return -1;
      store_text(i,type_J);
      text_address = text_address+4;
    }
    // ラベルの場合
    else{
      int32_t address = search_list(labels,rd);
      if(address<0) return -1;
      int32_t distance = address - text_address;
      // jalで飛べる場合は jal ra, distance。
      if(-1048576<=distance&&distance<1048576){
        struct instruction type_J;
        type_J.opcode = 0b1101111;
        type_J.rd_index = index_register("ra");
        type_J.imm = distance;
        int i = index_text(text_address);
        if(i<0) return -1;
        store_text(i,type_J);
        text_address = text_address+4;
      }
      // 飛べない場合は auipc t1, uo -> jal ra, t1, uouo。本来は jal ではなく call を使う。
      else{
        distance = distance-4;
        int32_t dis_lui = distance>>12;
        struct instruction type_U;
        type_U.opcode = 0b0010111;
        type_U.rd_index = index_register("t1");
        type_U.imm = dis_lui;
        int i = index_text(text_address);
        if(i<0) return -1;
        store_text(i,type_U);
        text_address = text_address+4;
        struct instruction type_I;
        type_I.opcode = 0b1100111;
        type_I.funct3 = 0b000;
        type_I.rd_index = index_register("ra");
        type_I.rs1_index = index_register("t1");
        type_I.imm = distance-(dis_lui<<12);
        i = index_text(text_address);
        store_text(i,type_I);
        text_address = text_address+4;
      }
    }
  }
  else if(eqlstr(t,"jalr")==0){
    char rd[256],rs1[256],imm[256];
    struct instruction type_I;
    type_I.opcode = 0b1100111;
    type_I.funct3 = 0b000;
    signed char branch = operand(&rd);
    if(branch==-1) return -1;
    else if(branch==0){
      if(operand(&rs1)!=0||operand(&imm)!=1) return -1;
      type_I.rd_index = index_register(rd);
      type_I.rs1_index = index_register(rs1);
      type_I.imm = immediate(imm,11);
    }
    // jalr rs1 -> jalr ra, rs1, 0
    else if(branch==1){
      type_I.rd_index = index_register("ra");
      type_I.rs1_index = index_register(rd);
      type_I.imm = 0;
    }
    else return -1;
    int i = index_text(text_address);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"li")==0){
    char rd[256],imm[256];
    if(operand(&rd)!=0||operand(&imm)!=1) return -1;
    int32_t imm_li = immediate(imm,31);
    // 関数immidiateのエラーコードは1048576であることに注意。
    if(eqlstr(imm,"1048576")!=0&&imm_li==1048576) return -1;
    // imm_liが12bitで収まるなら addi rd, zero, imm_li。
    if(-2048<=imm_li&&imm_li<2048){
      struct instruction type_I;
      type_I.opcode = 0b0010011;
      type_I.funct3 = 0b000;
      type_I.rd_index = index_register(rd);
      type_I.rs1_index = index_register("zero");
      type_I.imm = imm_li;
      int i = index_text(text_address);
      if(i<0||type_I.rd_index<0) return -1;
      store_text(i,type_I);
      text_address = text_address+4;
    }
    // 収まらないならlui rd, (imm_li>>12) -> addi rd, rd, imm_li-((imm_li>>12)<<12)。
    else{
      struct instruction type_U;
      type_U.opcode = 0b0110111;
      type_U.rd_index = index_register(rd);
      type_U.imm = imm_li>>12;
      int i = index_text(text_address);
      if(i<0||type_U.rd_index<0) return -1;
      store_text(i,type_U);
      text_address = text_address+4;
      struct instruction type_I;
      type_I.opcode = 0b0010011;
      type_I.funct3 = 0b000;
      type_I.rd_index = index_register(rd);
      type_I.rs1_index = index_register(rd);
      type_I.imm = imm_li-((imm_li>>12)<<12);
      i = index_text(text_address);
      if(i<0||type_I.rd_index<0) return -1;
      store_text(i,type_I);
      text_address = text_address+4;
    }
  }
  else if(eqlstr(t,"la")==0){
    char rd[256],label[256];
    if(operand(&rd)!=0||operand(&label)!=1) return -1;
    int32_t imm_li = search_list(labels,label);
    if(imm_li<0) return -1;
    // imm_liが12bitで収まるなら addi rd, zero, imm_li。
    if(-2048<=imm_li&&imm_li<2048){
      struct instruction type_I;
      type_I.opcode = 0b0010011;
      type_I.funct3 = 0b000;
      type_I.rd_index = index_register(rd);
      type_I.rs1_index = index_register("zero");
      type_I.imm = imm_li;
      int i = index_text(text_address);
      if(i<0||type_I.rd_index<0) return -1;
      store_text(i,type_I);
      text_address = text_address+4;
    }
    // 収まらないならlui rd, (imm_li>>12) -> addi rd, rd, imm_li-((imm_li>>12)<<12)。
    else{
      struct instruction type_U;
      type_U.opcode = 0b0110111;
      type_U.rd_index = index_register(rd);
      type_U.imm = imm_li>>12;
      int i = index_text(text_address);
      if(i<0||type_U.rd_index<0) return -1;
      store_text(i,type_U);
      text_address = text_address+4;
      struct instruction type_I;
      type_I.opcode = 0b0010011;
      type_I.funct3 = 0b000;
      type_I.rd_index = index_register(rd);
      type_I.rs1_index = index_register(rd);
      type_I.imm = imm_li-((imm_li>>12)<<12);
      i = index_text(text_address);
      if(i<0||type_I.rd_index<0) return -1;
      store_text(i,type_I);
      text_address = text_address+4;
    }
  }
  // mv rd, rs1 -> addi rd, rs1, 0
  else if(eqlstr(t,"mv")==0){
    char rd[256],rs1[256];
    if(operand(&rd)!=0||operand(&rs1)!=1) return -1;
    struct instruction type_I;
    type_I.opcode = 0b0010011;
    type_I.funct3 = 0b000;
    type_I.rd_index = index_register(rd);
    type_I.rs1_index = index_register(rs1);
    type_I.imm = 0;
    int i = index_text(text_address);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  // ret -> jalr zero, ra, 0
  else if(eqlstr(t,"ret")==0){
    if(s[i_s]!='\0') return -1;
    struct instruction type_I;
    type_I.opcode = 0b1100111;
    type_I.funct3 = 0b000;
    type_I.rd_index = index_register("zero");
    type_I.rs1_index = index_register("ra");
    type_I.imm = 0;
    int i = index_text(text_address);
    if(i<0) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  // nop -> addi zero, zero, 0
  else if(eqlstr(t,"nop")==0){
    struct instruction type_I;
    type_I.opcode = 0b0010011;
    type_I.funct3 = 0b000;
    type_I.rd_index = index_register("zero");
    type_I.rs1_index = index_register("zero");
    type_I.imm = 0;
    int i = index_text(text_address);
    if(i<0) return -1;
    store_text(i,type_I);
    text_address = text_address+4;
  }
  else if(eqlstr(t,"halt")==0){
    if(s[i_s]!='\0') return -1;
    struct instruction halt;
    halt.opcode = 0b0000000;
    int i = index_text(text_address);
    if(i<0) return -1;
    store_text(i, halt);
    text_address = text_address+4;
  }
  else return -1;
  return 0;
}

signed char parse(char *file_name){

  init_parser();

  FILE *fp;
  fp = fopen(file_name, "r");
  if(fp==NULL) return -1;

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
    white_skip();
    while(s[i_s]!=9&&s[i_s]!=32&&s[i_s]!='\n'&&s[i_s]!='\0'){
      t[i_t] = s[i_s];
      i_s++; i_t++;
    }
    t[i_t] = '\0';
    white_skip();

    /* parse errorが出たときはこれを使ってどこでerror吐いてるか確認すると幸せになれるかも。*/
     printf("%s\n", t);
    /* 切り出した先頭を見て、label:, .uouo, instruction に場合分け */
    if(t[0]=='\0'){
    }
    else if(t[i_t-1]==':'){
      if(labeling(t)<0){
        fclose(fp);
        delete_list(labels);
        return -1;
      }
    }
    else if(t[0]=='.'){
      if(directive(t)<0){
        fclose(fp);
        delete_list(labels);
        return -1;
      }
    }
    else{
      if(instruction(t)<0){
        fclose(fp);
        delete_list(labels);
        return -1;
      }
    }

    i_s = 0;
  }

  /* 末尾にもhaltを足します。*/
  /* jalr zero, ra, 0にした方が行儀が良さそうだけど、こっちの方が便利なので。*/
  struct instruction halt;
  halt.opcode = 0b0000000;
  int i = index_text(text_address);
  if(i<0){
    fclose(fp);
    delete_list(labels);
    return -1;
  }

  /* min_caml_startから実行を始めます。 */
  int32_t main = search_list(labels,"min_caml_start");
  if(main<0){
    fclose(fp);
    delete_list(labels);
    return -1;
  }
  pc = pc + main;

  fclose(fp);
  delete_list(labels);
  return 0;
}
