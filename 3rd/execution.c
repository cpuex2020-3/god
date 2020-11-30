#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "instruction.h"
#include "data.h"
#include "fdata.h"
#include "assembly.h"

/* execute のなかから使う。長くなるから分離しただけ。*/
signed char f_execute(struct instruction instruction){
  // LOAD
  if(instruction.opcode==0b0000111){
    int32_t mem_address = load_regster(instruction.rs1_index)+instruction.imm;
    int mem_index = index_memory(mem_address);
    if(mem_index<0) return -1;
    // flw
    if(instruction.funct3==0b010){
      f_load_memory(mem_index, instruction.rd_index);
    }
    else return -1;
  }
  // STORE
  else if(instruction.opcode==0b0100111){
    int32_t mem_address = load_regster(instruction.rs1_index)+instruction.imm;
    int mem_index = index_memory(mem_address);
    if(mem_index<0) return -1;
    // fsw
    if(instruction.funct3==0b010){
      f_store_memory(mem_index, instruction.rs2_index);
    }
    else return -1;
  }
  // OTHERS
  else if(instruction.opcode==0b1010011){
    // fadd.s
    if(instruction.funct7==0b0000000){
      if(instruction.funct3==rm){
        float rs1 = f_load_regster(instruction.rs1_index);
        float rs2 = f_load_regster(instruction.rs2_index);
        f_store_register(instruction.rd_index, rs1+rs2);
      }
    }
    // fsub.s
    else if(instruction.funct7==0b0000100){
      if(instruction.funct3==rm){
        float rs1 = f_load_regster(instruction.rs1_index);
        float rs2 = f_load_regster(instruction.rs2_index);
        f_store_register(instruction.rd_index, rs1-rs2);
      }
    }
    // fmul.s
    else if(instruction.funct7==0b0001000){
      if(instruction.funct3==rm){
        float rs1 = f_load_regster(instruction.rs1_index);
        float rs2 = f_load_regster(instruction.rs2_index);
        f_store_register(instruction.rd_index, rs1*rs2);
      }
    }
    // fdiv.s
    else if(instruction.funct7==0b0001100){
      if(instruction.funct3==rm){
        float rs1 = f_load_regster(instruction.rs1_index);
        float rs2 = f_load_regster(instruction.rs2_index);
        f_store_register(instruction.rd_index, rs1/rs2);
      }
    }
    // fsqrt.s
    else if(instruction.funct7==0b0101100){
      if(instruction.funct3==rm&&instruction.rs2_index==0b00000){
        float rs1 = f_load_regster(instruction.rs1_index);
        f_store_register(instruction.rd_index, sqrtf(rs1));
      }
      else return -1;
    }
    else if(instruction.funct7==0b0010000){
      float rs1 = f_load_regster(instruction.rs1_index);
      float rs2 = f_load_regster(instruction.rs2_index);
      // fsgnj.s
      if(instruction.funct3==0b000){
        int32_t int1 = (*((int32_t *)&rs1))&0x7fffffff;
        int32_t int2 = (*((int32_t *)&rs2))&0x80000000;
        int32_t value = int1|int2;
        f_store_register(instruction.rd_index, *((float *)&value));
      }
      // fsgnjn.s
      else if(instruction.funct3==0b001){
        int32_t int1 = (*((int32_t *)&rs1))&0x80000000;
        int32_t int2 = (*((int32_t *)&rs2))&0x80000000;
        int32_t value = int1^int2;
        f_store_register(instruction.rd_index, *((float *)&value));
      }
      // fsgnjx.s
      else if(instruction.funct3==0b010){
        int32_t int1 = (*((int32_t *)&rs1));
        int32_t int2 = (*((int32_t *)&rs2))&0x80000000;
        int32_t value = int1^int2;
        f_store_register(instruction.rd_index, *((float *)&value));
      }
      else return -1;
    }
    // fcvt.s.w
    else if(instruction.funct7==0b1101000){
      if(instruction.funct3==rm&&instruction.rs2_index==0b00000){
        int32_t rs1 = load_regster(instruction.rs1_index);
        f_store_register(instruction.rd_index, (float)rs1);
      }
      else return -1;
    }
    // fmv.x.s
    else if(instruction.funct7==0b1110000){
      if(instruction.funct3==0b000&&instruction.rs2_index==0b00000){
        float rs1 = f_load_regster(instruction.rs1_index);
        store_register(instruction.rd_index, *((int32_t *)&rs1));
      }
      else return -1;
    }
    else if(instruction.funct7==0b1010000){
      int32_t value = 0;
      float rs1 = f_load_regster(instruction.rs1_index);
      float rs2 = f_load_regster(instruction.rs2_index);
      // feq.s
      if(instruction.funct3==0b010){
        if(rs1==rs2) value = 1;
        store_register(instruction.rd_index, value);
      }
      // flt.s
      else if(instruction.funct3==0b001){
        if(rs1<rs2) value = 1;
        store_register(instruction.rd_index, value);
      }
      // fle.s
      else if(instruction.funct3==0b000){
        if(rs1<=rs2) value = 1;
        store_register(instruction.rd_index, value);
      }
      else return -1;
    }
    else return -1;
  }
  else return -1;
  return 0;
}

signed char execute(struct instruction instruction){
  if(instruction.opcode==0b0000000){
    printf("\nhalt\n");
    pc = pc+0;
  }
  // LOAD
  else if(instruction.opcode==0b0000011){
    int32_t mem_address = load_regster(instruction.rs1_index)+instruction.imm;
    int mem_index = index_memory(mem_address);
    if(mem_index<0) return -1;
    int32_t value = load_memory(mem_index);
    // lw
    if(instruction.funct3==0b010){
    }
    else return -1;
    if(instruction.rd_index!=0) store_register(instruction.rd_index, value);
    pc = pc+4;
  }
  // STORE
  else if(instruction.opcode==0b0100011){
    int32_t mem_address = load_regster(instruction.rs1_index)+instruction.imm;
    int mem_index = index_memory(mem_address);
    if(mem_index<0) return -1;
    int32_t value = load_regster(instruction.rs2_index);
    // sw
    if(instruction.funct3==0b010){
    }
    else return -1;
    store_memory(mem_index, value);
    pc = pc+4;
  }
  // OP
  else if(instruction.opcode==0b0110011){
    int32_t value = 0;
    int32_t rs1 = load_regster(instruction.rs1_index);
    int32_t rs2 = load_regster(instruction.rs2_index);
    // sll
    if(instruction.funct3==0b001){
      rs2 = rs2&31;
      value = rs1<<rs2;
    }
    else if(instruction.funct3==0b101){
      // srl
      if(instruction.funct7==0b0000000){
        rs2 = rs2&31;
        value = ((uint32_t)rs1)>>rs2;
      }
      // sra
      else if(instruction.funct7==0b0100000){
        rs2 = rs2&31;
        value = rs1>>rs2;
      }
      else return -1;
    }
    else if(instruction.funct3==0b000){
      // add
      if(instruction.funct7==0b0000000){
        value = rs1+rs2;
      }
      // sub
      else if(instruction.funct7==0b0100000){
        value = rs1-rs2;
      }
      else return -1;
    }
    // xor
    else if(instruction.funct3==0b100){
      value = rs1^rs2;
    }
    // or
    else if(instruction.funct3==0b110){
      value = rs1|rs2;
    }
    // and
    else if(instruction.funct3==0b111){
      value = rs1&rs2;
    }
    // slt
    else if(instruction.funct3==0b010){
      if(rs1<rs2) value = 1;
    }
    // sltu
    else if(instruction.funct3==0b011){
      if((uint32_t)rs1<(uint32_t)rs2) value = 1;
    }
    else return -1;
    if(instruction.rd_index!=0) store_register(instruction.rd_index, value);
    pc = pc+4;
  }
  // OP-IMM
  else if(instruction.opcode==0b0010011){
    int32_t value = 0;
    int32_t rs1 = load_regster(instruction.rs1_index);
    // slli
    if(instruction.funct3==0b001){
      value = rs1<<instruction.imm;
    }
    else if(instruction.funct3==0b101){
      // srli
      if(instruction.funct7==0b0000000){
        value = ((uint32_t)rs1)>>instruction.imm;
      }
      // srai
      else if(instruction.funct7==0b0100000){
        value = rs1>>instruction.imm;
      }
      else return -1;
    }
    // addi
    else if(instruction.funct3==0b000){
      value = rs1+instruction.imm;
    }
    // xori
    else if(instruction.funct3==0b100){
      value = rs1^instruction.imm;
    }
    // ori
    else if(instruction.funct3==0b110){
      value = rs1|instruction.imm;
    }
    // andi
    else if(instruction.funct3==0b111){
      value = rs1&instruction.imm;
    }
    // slti
    else if(instruction.funct3==0b010){
      if(rs1<instruction.imm) value = 1;
    }
    // sltiu
    else if(instruction.funct3==0b011){
      if((uint32_t)rs1<(uint32_t)instruction.imm) value = 1;
    }
    else return -1;
    if(instruction.rd_index!=0) store_register(instruction.rd_index, value);
    pc = pc+4;
  }
  // BRANCH
  else if(instruction.opcode==0b1100011){
    int32_t offset = 4;
    int32_t rs1 = load_regster(instruction.rs1_index);
    int32_t rs2 = load_regster(instruction.rs2_index);
    // beq
    if(instruction.funct3==0b000){
      if(rs1==rs2) offset = instruction.imm;
    }
    // bne
    else if(instruction.funct3==0b001){
      if(rs1!=rs2) offset = instruction.imm;
    }
    // blt
    else if(instruction.funct3==0b100){
      if(rs1<rs2) offset = instruction.imm;
    }
    // bge
    else if(instruction.funct3==0b101){
      if(rs1>=rs2) offset = instruction.imm;
    }
    // bltu
    else if(instruction.funct3==0b110){
      if((uint32_t)rs1<(uint32_t)rs2) offset = instruction.imm;
    }
    // bgeu
    else if(instruction.funct3==0b111){
      if((uint32_t)rs1>=(uint32_t)rs2) offset = instruction.imm;
    }
    else return -1;
    pc = pc+offset;
  }
  // lui
  else if(instruction.opcode==0b0110111){
    int32_t value = instruction.imm<<12;
    if(instruction.rd_index!=0) store_register(instruction.rd_index, value);
    pc = pc+4;
  }
  // auipc
  else if(instruction.opcode==0b0010111){
    int32_t value = instruction.imm<<12;
    if(instruction.rd_index!=0) store_register(instruction.rd_index, pc+value);
    pc = pc+4;
  }
  // jal
  else if(instruction.opcode==0b1101111){
    if(instruction.rd_index!=0) store_register(instruction.rd_index, pc+4);
    pc = pc+instruction.imm;
  }
  // jalr
  else if(instruction.opcode==0b1100111){
    if(instruction.funct3==0b000){
      if(instruction.rd_index!=0) store_register(instruction.rd_index, pc+4);
      pc = load_regster(instruction.rs1_index)+instruction.imm;
    }
    else return -1;
  }
  else if(instruction.opcode==0b1011011){
    int32_t value = 0;
    int32_t rs1 = load_regster(instruction.rs1_index);
    if(instruction.funct7==0b0000000&&instruction.rs2_index==0b00000){
      // mul10
      if(instruction.funct3==0b000){
        value = rs1*10;
      }
      // div10
      else if(instruction.funct3==0b001){
        value = rs1/10;
      }
      else return -1;
    }
    else return -1;
    if(instruction.rd_index!=0) store_register(instruction.rd_index, value);
    pc = pc+4;
  }
  // rxbu
  else if(instruction.opcode==0b0001011){
    printf("plz UART for rxbu : ");
    scanf("%d\n", &uart);
    if(uart<0||uart>=256) return -1;
    int32_t rd = load_regster(instruction.rd_index)&0xffffff00;
    if(instruction.rd_index!=0) store_register(instruction.rd_index, rd+uart);
    pc = pc+4;
  }
  // txbu
  else if(instruction.opcode==0b0011011){
    uart = load_regster(instruction.rs1_index)&0x000000ff;
    pc = pc+4;
  }
  else{
    if(f_execute(instruction)<0) return -1;
    pc = pc+4;
  }
  return 0;
}

signed char step(){
  printf("pc    : %d\n\n", pc);
  show_registers();
  printf("\nUART  : %d\n\n", uart);
  f_show_registers();

  uart = 0;

  int i = index_text(pc);
  if(i<0) return -1;
  struct instruction instruction = load_text(i);

  printf("\nnext execution : ");
  if(assembly(instruction, stdout)<0) return -1;
  if(execute(instruction)<0) return -1;

  return 0;
}

signed char matomete(){
  for(int i = index_text(pc); ; uart = 0, i = index_text(pc)){
    uart = 0;
    if(i<0) return -1;
    struct instruction instruction = load_text(i);
    if(execute(instruction)<0) return -1;
    if(instruction.opcode==0b0000000){
      break;
    }
    else if(instruction.opcode==0b0011011){
      printf("%c", uart);
    }
  }
  return 0;
}
