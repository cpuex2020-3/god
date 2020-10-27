#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "data.h"

struct label_list {
  char name[256];
  int32_t offset;
  struct label_list *next;
};

void add_list(struct label_list** li, char name[256], int32_t offset){
  struct label_list* st;
  st = (struct label_list *)malloc(sizeof(struct label_list));
  int i = 0;
  for (; name[i]!='\0'; i++){
    (st->name)[i] = name[i];
  }
  (st->name)[i] = '\0';
  st->offset = offset;
  st->next = *li;
  *li = st;
  return;
}

int32_t search_list(struct label_list *li, char *name){
  if(li==NULL) return -1;
  else if(eqlstr(li->name,name)==0) return li->offset;
  else return search_list(li->next,name);
}

void delete_list(struct label_list *li){
  if(li!=NULL){
    delete_list(li->next);
    free(li);
  }
  return;
}

int32_t text_offset = 0;  // pcからの相対位置
int32_t data_offset = 0;  // gpからの相対位置
int32_t gp = 0;
void init_parser(){
  struct instruction halt;
  halt.opcode = 0b0000000;
  store_text(index_register("ra"), halt);  // pcとraの初期値は0です。
  text_offset = 4;  // 命令はpcの初期値の次の場所から順に書き込んでいく。
  data_offset = 0;
  gp = load_regster(index_register("gp"));
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

/* immidiate枠にはimm[11:0](基本はこれ),shamt[5:0](shift系),
                 imm[31:12](lui,aupci),
                 imm[12:1](branch系),imm[20:1](jal)の5種類があるらしい。
   次の関数では、bit = 11, 5, 19, 12, 20 で場合分けしています。1048576 は 1<<20 の値。*/
int32_t immediate(char *imm, int32_t bit){
  int32_t up_bd = 1 << bit;
  int i = 0, j = 0;
  if(imm[0]=='\0') return 1048576;
  if(imm[0]=='-') j++;
  for (i = j; imm[i]!='\0'; i++){
    if(imm[i]<48||57<imm[i]) return 1048576;
  }
  if(i==j) return 1048576;
  int32_t value = atoi(imm);
  if(value<-1*up_bd||up_bd<=value) return 1048576;
  if((bit==12||bit==20)&&value%2==1) return 1048576;
  return value;
}

signed char labeling(char t[256]){
  int i_t = 0;
  while(t[i_t]!=':') i_t++;
  t[i_t] = '\0';
  // indexのインクリメントは実際にメモリに書き込むとき。
  if(mode==0) add_list(&labels, t, text_offset);
  else if(mode==1) add_list(&labels, t, data_offset);
  return 0;
}

/* 増やそうな */
signed char directive(char t[256]){
  if(eqlstr(t,".data")==0) mode = 1;
  else if(eqlstr(t,".text")==0) mode = 0;
  else if(eqlstr(t,".align")==0){
    /* 4-alignment以外想定してないんだけど */
  }
  else if(eqlstr(t,".globl")==0){
    /* 外部関数実装時に書くか */
  }
  else if(eqlstr(t,".word")==0){
    /* 書くだけ。はよ書け。 */
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
    int i = index_text(pc+text_offset);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_offset = text_offset+4;
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
    int i = index_text(pc+text_offset);
    if(i<0||type_S.rs1_index<0||type_S.rs2_index<0||type_S.imm==1048576) return -1;
    store_text(i,type_S);
    text_offset = text_offset+4;
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
    int i = index_text(pc+text_offset);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_offset = text_offset+4;
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
    int i = index_text(pc+text_offset);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_offset = text_offset+4;
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
    int i = index_text(pc+text_offset);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_offset = text_offset+4;
  }
  else if(eqlstr(t,"lui")==0){
    char rd[256],imm[256];
    if(operand(&rd)!=0||operand(&imm)!=1) return -1;
    struct instruction type_U;
    type_U.opcode = 0b0110111;
    type_U.rd_index = index_register(rd);
    type_U.imm = immediate(imm,19);
    int i = index_text(pc+text_offset);
    if(i<0||type_U.rd_index<0||type_U.imm==1048576) return -1;
    store_text(i,type_U);
    text_offset = text_offset+4;
  }
  else if(eqlstr(t,"auipc")==0){
    char rd[256],imm[256];
    if(operand(&rd)!=0||operand(&imm)!=1) return -1;
    struct instruction type_U;
    type_U.opcode = 0b0010111;
    type_U.rd_index = index_register(rd);
    type_U.imm = immediate(imm,19);
    int i = index_text(pc+text_offset);
    if(i<0||type_U.rd_index<0||type_U.imm==1048576) return -1;
    store_text(i,type_U);
    text_offset = text_offset+4;
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
    int i = index_text(pc+text_offset);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_offset = text_offset+4;
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
    int i = index_text(pc+text_offset);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_offset = text_offset+4;
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
    int i = index_text(pc+text_offset);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_offset = text_offset+4;
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
    int i = index_text(pc+text_offset);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_offset = text_offset+4;
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
    int i = index_text(pc+text_offset);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_offset = text_offset+4;
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
    int i = index_text(pc+text_offset);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_offset = text_offset+4;
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
    int i = index_text(pc+text_offset);
    if(i<0||type_R.rd_index<0||type_R.rs1_index<0||type_R.rs2_index<0) return -1;
    store_text(i,type_R);
    text_offset = text_offset+4;
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
    int i = index_text(pc+text_offset);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_offset = text_offset+4;
  }
  else if(eqlstr(t,"beq")==0){
    char rs1[256],rs2[256],imm[256];
    if(operand(&rs1)!=0||operand(&rs2)!=0||operand(&imm)!=1) return -1;
    struct instruction type_B;
    type_B.opcode = 0b1100011;
    type_B.funct3 = 0b000;
    type_B.rs1_index = index_register(rs1);
    type_B.rs2_index = index_register(rs2);
    type_B.imm = immediate(imm,12);
    int i = index_text(pc+text_offset);
    if(i<0||type_B.rs1_index<0||type_B.rs2_index<0||type_B.imm==1048576) return -1;
    store_text(i,type_B);
    text_offset = text_offset+4;
  }
  else if(eqlstr(t,"bne")==0){
    char rs1[256],rs2[256],imm[256];
    if(operand(&rs1)!=0||operand(&rs2)!=0||operand(&imm)!=1) return -1;
    struct instruction type_B;
    type_B.opcode = 0b1100011;
    type_B.funct3 = 0b001;
    type_B.rs1_index = index_register(rs1);
    type_B.rs2_index = index_register(rs2);
    type_B.imm = immediate(imm,12);
    int i = index_text(pc+text_offset);
    if(i<0||type_B.rs1_index<0||type_B.rs2_index<0||type_B.imm==1048576) return -1;
    store_text(i,type_B);
    text_offset = text_offset+4;
  }
  else if(eqlstr(t,"blt")==0){
    char rs1[256],rs2[256],imm[256];
    if(operand(&rs1)!=0||operand(&rs2)!=0||operand(&imm)!=1) return -1;
    struct instruction type_B;
    type_B.opcode = 0b1100011;
    type_B.funct3 = 0b100;
    type_B.rs1_index = index_register(rs1);
    type_B.rs2_index = index_register(rs2);
    type_B.imm = immediate(imm,12);
    int i = index_text(pc+text_offset);
    if(i<0||type_B.rs1_index<0||type_B.rs2_index<0||type_B.imm==1048576) return -1;
    store_text(i,type_B);
    text_offset = text_offset+4;
  }
  else if(eqlstr(t,"bge")==0){
    char rs1[256],rs2[256],imm[256];
    if(operand(&rs1)!=0||operand(&rs2)!=0||operand(&imm)!=1) return -1;
    struct instruction type_B;
    type_B.opcode = 0b1100011;
    type_B.funct3 = 0b101;
    type_B.rs1_index = index_register(rs1);
    type_B.rs2_index = index_register(rs2);
    type_B.imm = immediate(imm,12);
    int i = index_text(pc+text_offset);
    if(i<0||type_B.rs1_index<0||type_B.rs2_index<0||type_B.imm==1048576) return -1;
    store_text(i,type_B);
    text_offset = text_offset+4;
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
      int i = index_text(pc+text_offset);
      if(i<0||type_J.rd_index<0||type_J.imm==1048576) return -1;
      store_text(i,type_J);
      text_offset = text_offset+4;
    }
    // ラベルの場合
    else{
      int32_t offset = search_list(labels,rd);
      if(offset<0) return -1;
      int32_t distance = offset - text_offset;
      // jalで飛べる場合は jal ra, distance。
      if(-1048576<=distance&&distance<1048576){
        struct instruction type_J;
        type_J.opcode = 0b1101111;
        type_J.rd_index = index_register("ra");
        type_J.imm = distance;
        int i = index_text(pc+text_offset);
        if(i<0) return -1;
        store_text(i,type_J);
        text_offset = text_offset+4;
      }
      // 飛べない場合は auipc t1, uo -> jal ra, t1, uouo。本来は jal ではなく call を使う。
      else{
        distance = distance-4;
        int32_t dis_lui = distance>>12;
        struct instruction type_U;
        type_U.opcode = 0b0010111;
        type_U.rd_index = index_register("t1");
        type_U.imm = dis_lui;
        int i = index_text(pc+text_offset);
        if(i<0) return -1;
        store_text(i,type_U);
        text_offset = text_offset+4;
        struct instruction type_I;
        type_I.opcode = 0b1100111;
        type_I.funct3 = 0b000;
        type_I.rd_index = index_register("ra");
        type_I.rs1_index = index_register("t1");
        type_I.imm = distance-(dis_lui<<12);
        i = index_text(pc+text_offset);
        store_text(i,type_I);
        text_offset = text_offset+4;
      }
    }
  }
  else if(eqlstr(t,"jalr")==0){
    char rd[256],rs1[256],imm[256];
    if(operand(&rd)!=0||operand(&rs1)!=0||operand(&imm)!=1) return -1;
    struct instruction type_I;
    type_I.opcode = 0b1100111;
    type_I.funct3 = 0b000;
    type_I.rd_index = index_register(rd);
    type_I.rs1_index = index_register(rs1);
    type_I.imm = immediate(imm,11);
    int i = index_text(pc+text_offset);
    if(i<0||type_I.rd_index<0||type_I.rs1_index<0||type_I.imm==1048576) return -1;
    store_text(i,type_I);
    text_offset = text_offset+4;
  }
  else if(eqlstr(t,"halt")==0){
    struct instruction halt;
    halt.opcode = 0b0000000;
    int i = index_text(pc+text_offset);
    if(i<0) return -1;
    store_text(i, halt);
    text_offset = text_offset+4;
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

    /* 切り出した先頭を見て、label:, .uouo, instruction に場合分け */
    if(t[0]=='\0'){
    }
    else if(t[i_t-1]==':'){
      if(labeling(t)<0){
        fclose(fp);
        return -1;
      }
    }
    else if(t[0]=='.'){
      if(directive(t)<0){
        fclose(fp);
        return -1;
      }
    }
    else{
      if(instruction(t)<0){
        fclose(fp);
        return -1;
      }
    }

    i_s = 0;
  }

  fclose(fp);
  /* 末尾にもhaltを足します。 */
  struct instruction halt;
  halt.opcode = 0b0000000;
  int i = index_text(pc+text_offset);
  if(i<0) return -1;
  /* mainから実行を始めます。 */
  int32_t main = search_list(labels,"main");
  if(main<0) return -1;
  delete_list(labels);
  pc = pc + main;
  return 0;
}
