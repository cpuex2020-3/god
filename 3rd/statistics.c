#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "data.h"

unsigned long long counter = 0;

unsigned long long stat_inst[52] = { 0 };
unsigned long long stat_reg_rd[64] = { 0 };
unsigned long long stat_reg_rs[64] = { 0 };

int32_t max_hp = 0;

char *inst[52] = {"halt    ","lw      ","sw      ","sll     ","srl     ","sra     ","add     ","sub     ","xor     ","or      ","and     ","slt     ","sltu    ","slli    ","srli    ","srai    ","addi    ","xori    ","ori     ","andi    ","slti    ","sltiu   ","beq     ","bne     ","blt     ","bge     ","bltu    ","bgeu    ","lui     ","auipc   ","jal     ","jalr    ","mul10   ","div10   ","rxbu    ","txbu    ","flw     ","fsw     ","fadd.s  ","fsub.s  ","fmul.s  ","fdiv.s  ","fsqrt.s ","fsgnj.s ","fsgnjn.s","fsgnjx.s","fcvt.s.w","fmv.s.w ","fmv.w.s ","feq.s   ","flt.s   ","fle.s   "};
char *regi[64] = {"zero ","ra   ","sp   ","gp   ","tp   ","t0/hp","t1   ","t2   ","s0/fp","s1   ","a0   ","a1   ","a2   ","a3   ","a4   ","a5   ","a6   ","a7   ","s2   ","s3   ","s4   ","s5   ","s6   ","s7   ","s8   ","s9   ","s10  ","s11  ","t3   ","t4   ","t5   ","t6   ","ft0  ","ft1  ","ft2  ","ft3  ","ft4  ","ft5  ","ft6  ","ft7  ","fs0  ","fs1  ","fa0  ","fa1  ","fa2  ","fa3  ","fa4  ","fa5  ","fa6  ","fa7  ","fs2  ","fs3  ","fs4  ","fs5  ","fs6  ","fs7  ","fs8  ","fs9  ","fs10 ","fs11 ","ft8  ","ft9  ","ft10 ","ft11 "};

void count_exec(){
  counter = counter + 1;
  return ;
};

void print_count(FILE *fp){
  fprintf(fp, "\nexecutions : %llu times\n", counter);
  return ;
};

signed char get_stat(struct instruction instruction){
  // halt
  if(instruction.opcode==0b0000000){
    stat_inst[0] = stat_inst[0] + 1;
  }
  // LOAD
  else if(instruction.opcode==0b0000011){
    // lw
    if(instruction.funct3==0b010){
      stat_reg_rd[instruction.rd_index] = stat_reg_rd[instruction.rd_index] + 1;
      stat_reg_rs[instruction.rs1_index] = stat_reg_rs[instruction.rs1_index] + 1;
      stat_inst[1] = stat_inst[1] + 1;
    }
    else return -1;
  }
  // STORE
  else if(instruction.opcode==0b0100011){
    // sw
    if(instruction.funct3==0b010){
      stat_reg_rs[instruction.rs1_index] = stat_reg_rs[instruction.rs1_index] + 1;
      stat_reg_rs[instruction.rs2_index] = stat_reg_rs[instruction.rs2_index] + 1;
      stat_inst[2] = stat_inst[2] + 1;
    }
    else return -1;
  }
  // OP
  else if(instruction.opcode==0b0110011){
    stat_reg_rd[instruction.rd_index] = stat_reg_rd[instruction.rd_index] + 1;
    stat_reg_rs[instruction.rs1_index] = stat_reg_rs[instruction.rs1_index] + 1;
    stat_reg_rs[instruction.rs2_index] = stat_reg_rs[instruction.rs2_index] + 1;
    // sll
    if(instruction.funct3==0b001){
      stat_inst[3] = stat_inst[3] + 1;
    }
    else if(instruction.funct3==0b101){
      // srl
      if(instruction.funct7==0b0000000){
        stat_inst[4] = stat_inst[4] + 1;
      }
      // sra
      else if(instruction.funct7==0b0100000){
        stat_inst[5] = stat_inst[5] + 1;
      }
      else return -1;
    }
    else if(instruction.funct3==0b000){
      // add
      if(instruction.funct7==0b0000000){
        stat_inst[6] = stat_inst[6] + 1;
      }
      // sub
      else if(instruction.funct7==0b0100000){
        stat_inst[7] = stat_inst[7] + 1;
      }
      else return -1;
    }
    // xor
    else if(instruction.funct3==0b100){
      stat_inst[8] = stat_inst[8] + 1;
    }
    // or
    else if(instruction.funct3==0b110){
      stat_inst[9] = stat_inst[9] + 1;
    }
    // and
    else if(instruction.funct3==0b111){
      stat_inst[10] = stat_inst[10] + 1;
    }
    // slt
    else if(instruction.funct3==0b010){
      stat_inst[11] = stat_inst[11] + 1;
    }
    // sltu
    else if(instruction.funct3==0b011){
      stat_inst[12] = stat_inst[12] + 1;
    }
    else return -1;
  }
  // OP-IMM
  else if(instruction.opcode==0b0010011){
    stat_reg_rd[instruction.rd_index] = stat_reg_rd[instruction.rd_index] + 1;
    stat_reg_rs[instruction.rs1_index] = stat_reg_rs[instruction.rs1_index] + 1;
    // slli
    if(instruction.funct3==0b001){
      stat_inst[13] = stat_inst[13] + 1;
    }
    else if(instruction.funct3==0b101){
      // srli
      if(instruction.funct7==0b0000000){
        stat_inst[14] = stat_inst[14] + 1;
      }
      // srai
      else if(instruction.funct7==0b0100000){
        stat_inst[15] = stat_inst[15] + 1;
      }
      else return -1;
    }
    // addi
    else if(instruction.funct3==0b000){
      stat_inst[16] = stat_inst[16] + 1;
    }
    // xori
    else if(instruction.funct3==0b100){
      stat_inst[17] = stat_inst[17] + 1;
    }
    // ori
    else if(instruction.funct3==0b110){
      stat_inst[18] = stat_inst[18] + 1;
    }
    // andi
    else if(instruction.funct3==0b111){
      stat_inst[19] = stat_inst[19] + 1;
    }
    // slti
    else if(instruction.funct3==0b010){
      stat_inst[20] = stat_inst[20] + 1;
    }
    // sltiu
    else if(instruction.funct3==0b011){
      stat_inst[21] = stat_inst[21] + 1;
    }
    else return -1;
  }
  // BRANCH
  else if(instruction.opcode==0b1100011){
    stat_reg_rs[instruction.rs1_index] = stat_reg_rs[instruction.rs1_index] + 1;
    stat_reg_rs[instruction.rs2_index] = stat_reg_rs[instruction.rs2_index] + 1;
    // beq
    if(instruction.funct3==0b000){
      stat_inst[22] = stat_inst[22] + 1;
    }
    // bne
    else if(instruction.funct3==0b001){
      stat_inst[23] = stat_inst[23] + 1;
    }
    // blt
    else if(instruction.funct3==0b100){
      stat_inst[24] = stat_inst[24] + 1;
    }
    // bge
    else if(instruction.funct3==0b101){
      stat_inst[25] = stat_inst[25] + 1;
    }
    // bltu
    else if(instruction.funct3==0b110){
      stat_inst[26] = stat_inst[26] + 1;
    }
    // bgeu
    else if(instruction.funct3==0b111){
      stat_inst[27] = stat_inst[27] + 1;
    }
    else return -1;
  }
  // lui
  else if(instruction.opcode==0b0110111){
    stat_inst[28] = stat_inst[28] + 1;
    stat_reg_rd[instruction.rd_index] = stat_reg_rd[instruction.rd_index] + 1;
  }
  // auipc
  else if(instruction.opcode==0b0010111){
    stat_inst[29] = stat_inst[29] + 1;
    stat_reg_rd[instruction.rd_index] = stat_reg_rd[instruction.rd_index] + 1;
  }
  // jal
  else if(instruction.opcode==0b1101111){
    stat_inst[30] = stat_inst[30] + 1;
    stat_reg_rd[instruction.rd_index] = stat_reg_rd[instruction.rd_index] + 1;
  }
  // jalr
  else if(instruction.opcode==0b1100111){
    stat_reg_rd[instruction.rd_index] = stat_reg_rd[instruction.rd_index] + 1;
    stat_reg_rs[instruction.rs1_index] = stat_reg_rs[instruction.rs1_index] + 1;
    stat_inst[31] = stat_inst[31] + 1;
  }
  else if(instruction.opcode==0b1011011){
    stat_reg_rd[instruction.rd_index] = stat_reg_rd[instruction.rd_index] + 1;
    stat_reg_rs[instruction.rs1_index] = stat_reg_rs[instruction.rs1_index] + 1;
    if(instruction.funct7==0b0000000&&instruction.rs2_index==0b00000){
      // mul10
      if(instruction.funct3==0b000){
        stat_inst[32] = stat_inst[32] + 1;
      }
      // div10
      else if(instruction.funct3==0b001){
        stat_inst[33] = stat_inst[33] + 1;
      }
      else return -1;
    }
    else return -1;
  }
  // rxbu
  else if(instruction.opcode==0b0001011){
    stat_inst[34] = stat_inst[34] + 1;
    stat_reg_rd[instruction.rd_index] = stat_reg_rd[instruction.rd_index] + 1;
  }
  // txbu
  else if(instruction.opcode==0b0011011){
    stat_inst[35] = stat_inst[35] + 1;
    stat_reg_rs[instruction.rs1_index] = stat_reg_rs[instruction.rs1_index] + 1;
  }
  // LOAD-FP
  else if(instruction.opcode==0b0000111){
    // flw
    if(instruction.funct3==0b010){
      stat_inst[36] = stat_inst[36] + 1;
      stat_reg_rd[32+instruction.rd_index] = stat_reg_rd[32+instruction.rd_index] + 1;
      stat_reg_rs[instruction.rs1_index] = stat_reg_rs[instruction.rs1_index] + 1;
    }
    else return -1;
  }
  // STORE-FP
  else if(instruction.opcode==0b0100111){
    // fsw
    if(instruction.funct3==0b010){
      stat_inst[37] = stat_inst[37] + 1;
      stat_reg_rs[instruction.rs1_index] = stat_reg_rs[instruction.rs1_index] + 1;
      stat_reg_rs[32+instruction.rs2_index] = stat_reg_rs[32+instruction.rs2_index] + 1;
    }
    else return -1;
  }
  // OP-FP
  else if(instruction.opcode==0b1010011){
    // fadd.s
    if(instruction.funct7==0b0000000){
      if(instruction.funct3==rm){
        stat_inst[38] = stat_inst[38] + 1;
        stat_reg_rd[32+instruction.rd_index] = stat_reg_rd[32+instruction.rd_index] + 1;
        stat_reg_rs[32+instruction.rs1_index] = stat_reg_rs[32+instruction.rs1_index] + 1;
        stat_reg_rs[32+instruction.rs2_index] = stat_reg_rs[32+instruction.rs2_index] + 1;
      }
      else return -1;
    }
    // fsub.s
    else if(instruction.funct7==0b0000100){
      if(instruction.funct3==rm){
        stat_inst[39] = stat_inst[39] + 1;
        stat_reg_rd[32+instruction.rd_index] = stat_reg_rd[32+instruction.rd_index] + 1;
        stat_reg_rs[32+instruction.rs1_index] = stat_reg_rs[32+instruction.rs1_index] + 1;
        stat_reg_rs[32+instruction.rs2_index] = stat_reg_rs[32+instruction.rs2_index] + 1;
      }
      else return -1;
    }
    // fmul.s
    else if(instruction.funct7==0b0001000){
      if(instruction.funct3==rm){
        stat_inst[40] = stat_inst[40] + 1;
        stat_reg_rd[32+instruction.rd_index] = stat_reg_rd[32+instruction.rd_index] + 1;
        stat_reg_rs[32+instruction.rs1_index] = stat_reg_rs[32+instruction.rs1_index] + 1;
        stat_reg_rs[32+instruction.rs2_index] = stat_reg_rs[32+instruction.rs2_index] + 1;
      }
      else return -1;
    }
    // fdiv.s
    else if(instruction.funct7==0b0001100){
      if(instruction.funct3==rm){
        stat_inst[41] = stat_inst[41] + 1;
        stat_reg_rd[32+instruction.rd_index] = stat_reg_rd[32+instruction.rd_index] + 1;
        stat_reg_rs[32+instruction.rs1_index] = stat_reg_rs[32+instruction.rs1_index] + 1;
        stat_reg_rs[32+instruction.rs2_index] = stat_reg_rs[32+instruction.rs2_index] + 1;
      }
      else return -1;
    }
    // fsqrt.s
    else if(instruction.funct7==0b0101100){
      if(instruction.funct3==rm&&instruction.rs2_index==0b00000){
        stat_inst[42] = stat_inst[42] + 1;
        stat_reg_rd[32+instruction.rd_index] = stat_reg_rd[32+instruction.rd_index] + 1;
        stat_reg_rs[32+instruction.rs1_index] = stat_reg_rs[32+instruction.rs1_index] + 1;
      }
      else return -1;
    }
    else if(instruction.funct7==0b0010000){
      stat_reg_rd[32+instruction.rd_index] = stat_reg_rd[32+instruction.rd_index] + 1;
      stat_reg_rs[32+instruction.rs1_index] = stat_reg_rs[32+instruction.rs1_index] + 1;
      stat_reg_rs[32+instruction.rs2_index] = stat_reg_rs[32+instruction.rs2_index] + 1;
      // fsgnj.s
      if(instruction.funct3==0b000){
        stat_inst[43] = stat_inst[43] + 1;
      }
      // fsgnjn.s
      else if(instruction.funct3==0b001){
        stat_inst[44] = stat_inst[44] + 1;
      }
      // fsgnjx.s
      else if(instruction.funct3==0b010){
        stat_inst[45] = stat_inst[45] + 1;
      }
      else return -1;
    }
    // fcvt.s.w
    else if(instruction.funct7==0b1101000){
      if(instruction.funct3==rm&&instruction.rs2_index==0b00000){
        stat_inst[46] = stat_inst[46] + 1;
        stat_reg_rd[32+instruction.rd_index] = stat_reg_rd[32+instruction.rd_index] + 1;
        stat_reg_rs[instruction.rs1_index] = stat_reg_rs[instruction.rs1_index] + 1;
      }
      else return -1;
    }
    // fmv.s.w
    else if(instruction.funct7==0b1111000){
      if(instruction.funct3==0b000&&instruction.rs2_index==0b00000){
        stat_inst[47] = stat_inst[47] + 1;
        stat_reg_rd[32+instruction.rd_index] = stat_reg_rd[32+instruction.rd_index] + 1;
        stat_reg_rs[instruction.rs1_index] = stat_reg_rs[instruction.rs1_index] + 1;
      }
      else return -1;
    }
    // fmv.w.s
    else if(instruction.funct7==0b1110000){
      if(instruction.funct3==0b000&&instruction.rs2_index==0b00000){
        stat_inst[48] = stat_inst[48] + 1;
        stat_reg_rd[instruction.rd_index] = stat_reg_rd[instruction.rd_index] + 1;
        stat_reg_rs[32+instruction.rs1_index] = stat_reg_rs[32+instruction.rs1_index] + 1;
      }
      else return -1;
    }
    else if(instruction.funct7==0b1010000){
      stat_reg_rd[instruction.rd_index] = stat_reg_rd[instruction.rd_index] + 1;
      stat_reg_rs[32+instruction.rs1_index] = stat_reg_rs[32+instruction.rs1_index] + 1;
      stat_reg_rs[32+instruction.rs2_index] = stat_reg_rs[32+instruction.rs2_index] + 1;
      // feq.s
      if(instruction.funct3==0b010){
        stat_inst[49] = stat_inst[49] + 1;
      }
      // flt.s
      else if(instruction.funct3==0b001){
        stat_inst[50] = stat_inst[50] + 1;
      }
      // fle.s
      else if(instruction.funct3==0b000){
        stat_inst[51] = stat_inst[51] + 1;
      }
      else return -1;
    }
    else return -1;
  }
  else return -1;
  signed char uo = 1;
  int32_t now_hp = load_register(index_register("hp"), &uo);
  if(uo==0&&now_hp>max_hp) max_hp = now_hp;
  return 0;
};

signed char show_statistics(char *output_file_s){
  FILE *fp = fopen(output_file_s, "w");
  if(fp==NULL) return -1;

  fprintf(fp, "\nstatistics\n");
  fprintf(fp, "\nmax of hp : %d\n", max_hp);
  print_count(fp);
  fprintf(fp, "\ninstructions : itemized excutions\n");
  for(int i=0; i<52; i++){
    fprintf(fp, "%s : ", inst[i]);
    fprintf(fp, "%llu\n", stat_inst[i]);
  }
  fprintf(fp, "\nregisters : accessed as rd, rs\n");
  for(size_t i=0; i<64; i++){
    fprintf(fp, "%s : ", regi[i]);
    fprintf(fp, "%llu, %llu\n", stat_reg_rd[i], stat_reg_rs[i]);
  }

  fclose(fp);
  return 0;
};
