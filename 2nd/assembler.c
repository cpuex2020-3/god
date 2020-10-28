#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "data.h"

FILE *fp;

void print_uo(int uouo, int length){
  for(int i=length-1; i>=0; i--){
    int c = (uouo>>i)&1;
    fprintf(fp, "%d", c);
  }
}

signed char binary(struct instruction instruction){
  // HALT Fmt:X
  if(instruction.opcode==0b0000000){
    print_uo(0, 32);
    return 1;
  }
  // LOAD Fmt:I
  else if(instruction.opcode==0b0000011){
    print_uo(instruction.imm, 12);
    print_uo(instruction.rs1_index, 5);
    print_uo(instruction.funct3, 3);
    print_uo(instruction.rd_index, 5);
    print_uo(instruction.opcode, 7);
  }
  // STORE Fmt:S
  else if(instruction.opcode==0b0100011){
    print_uo(instruction.imm>>5, 7);
    print_uo(instruction.rs2_index, 5);
    print_uo(instruction.rd_index, 5);
    print_uo(instruction.funct3, 3);
    print_uo(instruction.imm, 5);
    print_uo(instruction.opcode, 7);
  }
  // OP Fmt:R
  else if(instruction.opcode==0b0110011){
    print_uo(instruction.funct7, 7);
    print_uo(instruction.rs2_index, 5);
    print_uo(instruction.rs1_index, 5);
    print_uo(instruction.funct3, 3);
    print_uo(instruction.rd_index, 5);
    print_uo(instruction.opcode, 7);
  }
  // OP-IMM Fmt:I
  else if(instruction.opcode==0b0010011){
    if(instruction.funct3==001||instruction.funct3==101){
      print_uo(instruction.funct7, 7);
      print_uo(instruction.imm, 5);
    }
    else{
      print_uo(instruction.imm, 12);
    }
    print_uo(instruction.rs1_index, 5);
    print_uo(instruction.funct3, 3);
    print_uo(instruction.rd_index,5);
    print_uo(instruction.opcode,7);
  }
  // BRANCH Fmt:SB
  else if(instruction.opcode==0b1100011){
    // immの最下位bitは必ず0なので不要。
    print_uo(instruction.imm>>12, 1);
    print_uo(instruction.imm>>5, 6);
    print_uo(instruction.rs2_index, 5);
    print_uo(instruction.rs1_index, 5);
    print_uo(instruction.funct3, 3);
    print_uo(instruction.imm>>1, 4);
    print_uo(instruction.imm>>11, 1);
    print_uo(instruction.opcode, 7);
  }
  // lui Fmt:U
  else if(instruction.opcode==0b0110111){
    print_uo(instruction.imm, 20);
    print_uo(instruction.rd_index, 5);
    print_uo(instruction.opcode, 7);
  }
  // auipc Fmt:U
  else if(instruction.opcode==0b0010111){
    print_uo(instruction.imm, 20);
    print_uo(instruction.rd_index, 5);
    print_uo(instruction.opcode, 7);
  }
  // jal Fmt:UJ
  else if(instruction.opcode==0b1101111){
    // immの最下位bitは必ず0なので不要。
    print_uo(instruction.imm>>20, 1);
    print_uo(instruction.imm>>1, 10);
    print_uo(instruction.imm>>11, 1);
    print_uo(instruction.imm>>12, 8);
    print_uo(instruction.rd_index, 5);
    print_uo(instruction.opcode, 7);
  }
  // jalr Fmt:I
  else if(instruction.opcode==0b1100111){
    print_uo(instruction.imm, 12);
    print_uo(instruction.rs1_index, 5);
    print_uo(instruction.funct3, 3);
    print_uo(instruction.rd_index,5);
    print_uo(instruction.opcode,7);
  }
  else return -10;
  return 0;
};

signed char assemble(char *output_file){
  fp = fopen(output_file, "w");
  if(fp==NULL) return -1;
  signed char j = 0;
  for(size_t i = 0 ; j<2; i++){
    struct instruction instruction = load_text(i);
    j = j+binary(instruction);
    if(j<0){
      fclose(fp);
      return -1;
    }
  }
  fclose(fp);
  return 0;
}
