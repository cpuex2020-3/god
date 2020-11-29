#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "data.h"

signed char assembly(struct instruction instruction, FILE *fp){
  // HALT Fmt:X
  if(instruction.opcode==0b0000000){
    fprintf(fp, "halt\n");
    return 1;
  }
  // LOAD Fmt:I
  else if(instruction.opcode==0b0000011){
    if(instruction.funct3==0b010){
      fprintf(fp, "lw   ");
    }
    else return -10;
    fprintf(fp, " x%d, %d(x%d)\n", instruction.rd_index, instruction.imm, instruction.rs1_index);
  }
  // STORE Fmt:S
  else if(instruction.opcode==0b0100011){
    if(instruction.funct3==0b010){
      fprintf(fp, "sw   ");
    }
    else return -10;
    fprintf(fp, " x%d, %d(x%d)\n", instruction.rs2_index, instruction.imm, instruction.rs1_index);
  }
  // OP Fmt:R
  else if(instruction.opcode==0b0110011){
    if(instruction.funct3==0b001){
      fprintf(fp, "sll  ");
    }
    else if(instruction.funct3==0b101){
      if(instruction.funct7==0b0000000){
        fprintf(fp, "srl  ");
      }
      else if(instruction.funct7==0b0100000){
        fprintf(fp, "sra  ");
      }
      else return -10;
    }
    else if(instruction.funct3==0b000){
      if(instruction.funct7==0b0000000){
        fprintf(fp, "add  ");
      }
      else if(instruction.funct7==0b0100000){
        fprintf(fp, "sub  ");
      }
      else return -10;
    }
    else if(instruction.funct3==0b100){
      fprintf(fp, "xor  ");
    }
    // or
    else if(instruction.funct3==0b110){
      fprintf(fp, "or   ");
    }
    // and
    else if(instruction.funct3==0b111){
      fprintf(fp, "and  ");
    }
    // slt
    else if(instruction.funct3==0b010){
      fprintf(fp, "slt  ");
    }
    // sltu
    else if(instruction.funct3==0b011){
      fprintf(fp, "sltu ");
    }
    else return -10;
    fprintf(fp, " x%d, x%d, x%d\n", instruction.rd_index, instruction.rs1_index, instruction.rs2_index);
  }
  // OP-IMM Fmt:I
  else if(instruction.opcode==0b0010011){
    if(instruction.funct3==0b001){
      fprintf(fp, "slli ");
    }
    else if(instruction.funct3==0b101){
      if(instruction.funct7==0b0000000){
        fprintf(fp, "srli ");
      }
      else if(instruction.funct7==0b0100000){
        fprintf(fp, "srai ");
      }
      else return -10;
    }
    else if(instruction.funct3==0b000){
      fprintf(fp, "addi ");
    }
    else if(instruction.funct3==0b100){
      fprintf(fp, "xori ");
    }
    // or
    else if(instruction.funct3==0b110){
      fprintf(fp, "ori  ");
    }
    // and
    else if(instruction.funct3==0b111){
      fprintf(fp, "andi ");
    }
    // slt
    else if(instruction.funct3==0b010){
      fprintf(fp, "slti ");
    }
    // sltiu
    else if(instruction.funct3==0b011){
      fprintf(fp, "sltiu");
    }
    else return -10;
    fprintf(fp, " x%d, x%d, %d\n", instruction.rd_index, instruction.rs1_index, instruction.imm);
  }
  // BRANCH Fmt:SB
  else if(instruction.opcode==0b1100011){
    if(instruction.funct3==0b000){
      fprintf(fp, "beq  ");
    }
    // bne
    else if(instruction.funct3==0b001){
      fprintf(fp, "bne  ");
    }
    // blt
    else if(instruction.funct3==0b100){
      fprintf(fp, "blt  ");
    }
    // bge
    else if(instruction.funct3==0b101){
      fprintf(fp, "bge  ");
    }
    // bltu
    else if(instruction.funct3==0b110){
      fprintf(fp, "bltu ");
    }
    // bgeu
    else if(instruction.funct3==0b111){
      fprintf(fp, "bgeu ");
    }
    else return -10;
    fprintf(fp, " x%d, x%d, %d\n", instruction.rs1_index, instruction.rs2_index, instruction.imm);
  }
  // lui Fmt:U
  else if(instruction.opcode==0b0110111){
    fprintf(fp, "lui  ");
    fprintf(fp, " x%d, %d\n", instruction.rd_index, instruction.imm);
  }
  // auipc Fmt:U
  else if(instruction.opcode==0b0010111){
    fprintf(fp, "auipc");
    fprintf(fp, " x%d, %d\n", instruction.rd_index, instruction.imm);
  }
  // jal Fmt:UJ
  else if(instruction.opcode==0b1101111){
    fprintf(fp, "jal  ");
    fprintf(fp, " x%d, %d\n", instruction.rd_index, instruction.imm);
  }
  // jalr Fmt:I
  else if(instruction.opcode==0b1100111){
    fprintf(fp, "jalr ");
    fprintf(fp, " x%d, x%d, %d\n", instruction.rd_index, instruction.rs1_index, instruction.imm);
  }
  // 10 Fmt:X
  else if(instruction.opcode==0b1011011){
    // mul10
    if(instruction.funct3==0b000){
      fprintf(fp, "mul10");
    }
    // div10
    else if(instruction.funct3==0b001){
      fprintf(fp, "div10");
    }
    else return -1;
    fprintf(fp, " x%d, x%d\n", instruction.rd_index, instruction.rs1_index);
  }
  // rxbu Fmt:X
  else if(instruction.opcode==0b0001011){
    fprintf(fp, "rxbu ");
    fprintf(fp, " x%d\n", instruction.rd_index);
  }
  // txbu Fmt:X
  else if(instruction.opcode==0b0011011){
    fprintf(fp, "txbu ");
    fprintf(fp, " x%d\n", instruction.rs1_index);
  }
  // LOAD-FP Fmt:I
  else if(instruction.opcode==0b0000111){
    if(instruction.funct3==0b010){
      fprintf(fp, "flw  ");
      fprintf(fp, " f%d, %d(x%d)\n", instruction.rd_index, instruction.imm, instruction.rs1_index);
    }
    else return -10;
  }
  // STORE-FP Fmt:S
  else if(instruction.opcode==0b0100111){
    if(instruction.funct3==0b010){
      fprintf(fp, "fsw  ");
      fprintf(fp, " f%d, %d(x%d)\n", instruction.rs2_index, instruction.imm, instruction.rs1_index);
    }
    else return -10;
  }
  // OP-FP Fmt:R
  else if(instruction.opcode==0b1010011){
    if(instruction.funct7==0b0000000){
      fprintf(fp, "fadd.s");
      fprintf(fp, " f%d, f%d, f%d\n", instruction.rd_index, instruction.rs1_index, instruction.rs2_index);
    }
    else if(instruction.funct7==0b0000100){
      fprintf(fp, "fsub.s");
      fprintf(fp, " f%d, f%d, f%d\n", instruction.rd_index, instruction.rs1_index, instruction.rs2_index);
    }
    else if(instruction.funct7==0b0001000){
      fprintf(fp, "fmul.s");
      fprintf(fp, " f%d, f%d, f%d\n", instruction.rd_index, instruction.rs1_index, instruction.rs2_index);
    }
    else if(instruction.funct7==0b0001100){
      fprintf(fp, "fdiv.s");
      fprintf(fp, " f%d, f%d, f%d\n", instruction.rd_index, instruction.rs1_index, instruction.rs2_index);
    }
    else if(instruction.funct7==0b0101100){
      fprintf(fp, "fsqrt.s");
      fprintf(fp, " f%d, f%d\n", instruction.rd_index, instruction.rs1_index);
    }
    else if(instruction.funct7==0b0010000){
      if(instruction.funct3==0b000){
        fprintf(fp, "fsgnj.s");
      }
      else if(instruction.funct3==0b001){
        fprintf(fp, "fsgnjn.s");
      }
      else if(instruction.funct3==0b010){
        fprintf(fp, "fsgnjx.s");
      }
      else return -10;
      fprintf(fp, " f%d, f%d, f%d\n", instruction.rd_index, instruction.rs1_index, instruction.rs2_index);
    }
    else if(instruction.funct7==0b1101000){
      fprintf(fp, "fcvt.s.w");
      fprintf(fp, " x%d, f%d\n", instruction.rd_index, instruction.rs1_index);
    }
    else if(instruction.funct7==0b1110000){
      fprintf(fp, "fmv.w.s");
      fprintf(fp, " x%d, f%d\n", instruction.rd_index, instruction.rs1_index);
    }
    else if(instruction.funct7==0b1010000){
      if(instruction.funct3==0b010){
        fprintf(fp, "feq.s");
      }
      else if(instruction.funct3==0b001){
        fprintf(fp, "flt.s");
      }
      else if(instruction.funct3==0b000){
        fprintf(fp, "fle.s");
      }
      else return -10;
      fprintf(fp, " x%d, f%d, f%d\n", instruction.rd_index, instruction.rs1_index, instruction.rs2_index);
    }
    else return -10;
  }
  else return -10;
  return 0;
};

signed char post_parser(char *output_file_s){
  FILE *fp = fopen(output_file_s, "w");
  if(fp==NULL) return -1;
  signed char j = 0;
  for(size_t i = 0 ; j<2; i++){
    struct instruction instruction = load_text(i);
    j = j+assembly(instruction, fp);
    if(j<0){
      fclose(fp);
      return -1;
    }
  }
  fclose(fp);
  return 0;
}
