#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "instruction.h"
#include "data.h"
#include "fpu_wrap.h"
#include "statistics.h"
#include "assembly.h"

/* execute のなかから使う。長くなるから分離しただけ。*/
signed char f_execute(struct instruction instruction){
  // LOAD-FP
  if(instruction.opcode==0b0000111){
    signed char indrs1 = 1, indmem = 1;
    int32_t mem_address = load_register(instruction.rs1_index, &indrs1)+instruction.imm;
    if(indrs1>0) return -1;
    int mem_index = index_memory(mem_address);
    if(mem_index<0) return -1;
    int32_t value = load_memory(mem_index, &indmem);
    // flw
    if(instruction.funct3==0b010){
    }
    else return -1;
    f_store_register(instruction.rd_index, value, indmem);
  }
  // STORE-FP
  else if(instruction.opcode==0b0100111){
    signed char indrs1 = 1, indrs2 = 1;
    int32_t mem_address = load_register(instruction.rs1_index, &indrs1)+instruction.imm;
    if(indrs1>0) return -1;
    int mem_index = index_memory(mem_address);
    if(mem_index<0) return -1;
    int32_t value = f_load_register(instruction.rs2_index, &indrs2);
    // fsw
    if(instruction.funct3==0b010){
    }
    else return -1;
    store_memory(mem_index, value, indrs2);
  }
  // OP-FP
  else if(instruction.opcode==0b1010011){
    // fadd.s
    if(instruction.funct7==0b0000000){
      if(instruction.funct3==rm){
        signed char indrs1 = 1, indrs2 = 1;
        int32_t rs1 = f_load_register(instruction.rs1_index, &indrs1);
        int32_t rs2 = f_load_register(instruction.rs2_index, &indrs2);
        int32_t value = fadd_s_wrap(rs1, rs2);
        f_store_register(instruction.rd_index, value, indrs1|indrs2);
      }
      else return -1;
    }
    // fsub.s
    else if(instruction.funct7==0b0000100){
      if(instruction.funct3==rm){
        signed char indrs1 = 1, indrs2 = 1;
        int32_t rs1 = f_load_register(instruction.rs1_index, &indrs1);
        int32_t rs2 = f_load_register(instruction.rs2_index, &indrs2);
        int32_t value = fsub_s_wrap(rs1, rs2);
        f_store_register(instruction.rd_index, value, indrs1|indrs2);
      }
      else return -1;
    }
    // fmul.s
    else if(instruction.funct7==0b0001000){
      if(instruction.funct3==rm){
        signed char indrs1 = 1, indrs2 = 1;
        int32_t rs1 = f_load_register(instruction.rs1_index, &indrs1);
        int32_t rs2 = f_load_register(instruction.rs2_index, &indrs2);
        int32_t value = fmul_s_wrap(rs1, rs2);
        f_store_register(instruction.rd_index, value, indrs1|indrs2);
      }
      else return -1;
    }
    // fdiv.s
    else if(instruction.funct7==0b0001100){
      if(instruction.funct3==rm){
        signed char indrs1 = 1, indrs2 = 1;
        int32_t rs1 = f_load_register(instruction.rs1_index, &indrs1);
        int32_t rs2 = f_load_register(instruction.rs2_index, &indrs2);
        int32_t value = fdiv_s_wrap(rs1, rs2);
        f_store_register(instruction.rd_index, value, indrs1|indrs2);
      }
      else return -1;
    }
    // fsqrt.s
    else if(instruction.funct7==0b0101100){
      if(instruction.funct3==rm&&instruction.rs2_index==0b00000){
        signed char indrs1 = 1;
        int32_t rs1 = f_load_register(instruction.rs1_index, &indrs1);
        int32_t value = fsqrt_s_wrap(rs1);
        f_store_register(instruction.rd_index, value, indrs1);
      }
      else return -1;
    }
    else if(instruction.funct7==0b0010000){
      signed char indrs1 = 1, indrs2 = 1;
      int32_t rs1 = f_load_register(instruction.rs1_index, &indrs1);
      int32_t rs2 = f_load_register(instruction.rs2_index, &indrs2);
      int32_t value = 0;
      // fsgnj.s
      if(instruction.funct3==0b000){
        int32_t int1 = rs1&0x7fffffff;
        int32_t int2 = rs2&0x80000000;
        value = int1|int2;
      }
      // fsgnjn.s
      else if(instruction.funct3==0b001){
        int32_t int1 = rs1|0x80000000;
        int32_t int2 = rs2&0x80000000;
        value = int1^int2;
      }
      // fsgnjx.s
      else if(instruction.funct3==0b010){
        int32_t int1 = rs1;
        int32_t int2 = rs2&0x80000000;
        value = int1^int2;
      }
      else return -1;
      f_store_register(instruction.rd_index, value, indrs1|indrs2);
    }
    // fcvt.s.w
    else if(instruction.funct7==0b1101000){
      if(instruction.funct3==rm&&instruction.rs2_index==0b00000){
        signed char indrs1 = 1;
        int32_t rs1 = load_register(instruction.rs1_index, &indrs1);
        int32_t value = fcvt_s_w_wrap(rs1);
        f_store_register(instruction.rd_index, value, indrs1);
      }
      else return -1;
    }
    // fmv.s.w
    else if(instruction.funct7==0b1111000){
      if(instruction.funct3==0b000&&instruction.rs2_index==0b00000){
        signed char indrs1 = 1;
        int32_t rs1 = load_register(instruction.rs1_index, &indrs1);
        f_store_register(instruction.rd_index, rs1, indrs1);
      }
      else return -1;
    }
    // fmv.w.s
    else if(instruction.funct7==0b1110000){
      if(instruction.funct3==0b000&&instruction.rs2_index==0b00000){
        signed char indrs1 = 1;
        int32_t rs1 = f_load_register(instruction.rs1_index, &indrs1);
        store_register(instruction.rd_index, rs1, indrs1);
      }
      else return -1;
    }
    else if(instruction.funct7==0b1010000){
      signed char indrs1 = 1, indrs2 = 1;
      int32_t rs1 = f_load_register(instruction.rs1_index, &indrs1);
      int32_t rs2 = f_load_register(instruction.rs2_index, &indrs2);
      int32_t value = 0;
      // feq.s
      if(instruction.funct3==0b010){
        int32_t abs1 = rs1 & 0x7fffffff;
        int32_t abs2 = rs2 & 0x7fffffff;
        if(rs1==rs2||(abs1==0&&abs2==0)) value = 1;
      }
      // flt.s
      else if(instruction.funct3==0b001){
        value = flt_s_wrap(rs1, rs2);
      }
      // fle.s
      else if(instruction.funct3==0b000){
        value = fle_s_wrap(rs1, rs2);
      }
      else return -1;
      store_register(instruction.rd_index, value, indrs1|indrs2);
    }
    else return -1;
  }
  else return -1;
  return 0;
}

signed char execute(struct instruction instruction){
  if(instruction.opcode==0b0000000){
    // fprintf(stderr, "\nhalt\n");
    pc = pc+0;
  }
  // LOAD
  else if(instruction.opcode==0b0000011){
    signed char indrs1 = 1, indmem = 1;
    int32_t mem_address = load_register(instruction.rs1_index, &indrs1)+instruction.imm;
    if(indrs1>0) return -1;
    int mem_index = index_memory(mem_address);
    if(mem_index<0) return -1;
    int32_t value = load_memory(mem_index, &indmem);
    // lw
    if(instruction.funct3==0b010){
    }
    else return -1;
    store_register(instruction.rd_index, value, indmem);
    pc = pc+1;
  }
  // STORE
  else if(instruction.opcode==0b0100011){
    signed char indrs1 = 1, indrs2 = 1;
    int32_t mem_address = load_register(instruction.rs1_index, &indrs1)+instruction.imm;
    if(indrs1>0) return -1;
    int mem_index = index_memory(mem_address);
    if(mem_index<0) return -1;
    int32_t value = load_register(instruction.rs2_index, &indrs2);
    // sw
    if(instruction.funct3==0b010){
    }
    else return -1;
    store_memory(mem_index, value, indrs2);
    pc = pc+1;
  }
  // OP
  else if(instruction.opcode==0b0110011){
    signed char indrs1 = 1, indrs2 = 1;
    int32_t rs1 = load_register(instruction.rs1_index, &indrs1);
    int32_t rs2 = load_register(instruction.rs2_index, &indrs2);
    int32_t value = 0;
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
    store_register(instruction.rd_index, value, indrs1|indrs2);
    pc = pc+1;
  }
  // OP-IMM
  else if(instruction.opcode==0b0010011){
    signed char indrs1 = 1;
    int32_t rs1 = load_register(instruction.rs1_index, &indrs1);
    int32_t value = 0;
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
    store_register(instruction.rd_index, value, indrs1);
    pc = pc+1;
  }
  // BRANCH
  else if(instruction.opcode==0b1100011){
    signed char indrs1 = 1, indrs2 = 1;
    int32_t rs1 = load_register(instruction.rs1_index, &indrs1);
    int32_t rs2 = load_register(instruction.rs2_index, &indrs2);
    if(indrs1>0||indrs2>0) return -1;
    int32_t offset = 4;
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
    store_register(instruction.rd_index, value, 0);
    pc = pc+1;
  }
  // auipc
  else if(instruction.opcode==0b0010111){
    int32_t value = instruction.imm<<12;
    store_register(instruction.rd_index, pc+value, 0);
    pc = pc+1;
  }
  // jal
  else if(instruction.opcode==0b1101111){
    store_register(instruction.rd_index, pc+1, 0);
    pc = pc+instruction.imm;
  }
  // jalr
  else if(instruction.opcode==0b1100111){
    if(instruction.funct3==0b000){
      signed char indrs1 = 1;
      int32_t rd = load_register(instruction.rs1_index, &indrs1);
      if(indrs1>0) return -1;
      store_register(instruction.rd_index, pc+1, 0);
      pc = rd+instruction.imm;
    }
    else return -1;
  }
  else if(instruction.opcode==0b1011011){
    signed char indrs1 = 1;
    int32_t rs1 = load_register(instruction.rs1_index, &indrs1);
    int32_t value = 0;
    uint32_t urs1 = (uint32_t)rs1;
    if(instruction.funct7==0b0000000&&instruction.rs2_index==0b00000){
      // mul10
      if(instruction.funct3==0b000){
        value = (int32_t)(urs1*10);
      }
      // div10
      else if(instruction.funct3==0b001){
        value = (int32_t)(urs1/10);
      }
      else return -1;
    }
    else return -1;
    store_register(instruction.rd_index, value, indrs1);
    pc = pc+1;
  }
  // rxbu
  else if(instruction.opcode==0b0001011){
    // fprintf(stderr, "plz UART for rxbu : ");
    scanf("%d", &uart);
    if(uart<0||uart>=256) return -1;
    store_register(instruction.rd_index, uart, 0);
    pc = pc+1;
  }
  // txbu
  else if(instruction.opcode==0b0011011){
    signed char indrs1 = 1;
    uart = load_register(instruction.rs1_index, &indrs1)&0x000000ff;
    if(indrs1>0) return -1;
    pc = pc+1;
  }
  else{
    if(f_execute(instruction)<0) return -1;
    pc = pc+1;
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

signed char matomete(signed char stat_switch){
  for(int i = index_text(pc); ; uart = 0, i = index_text(pc)){
    // fprintf(stderr, "%d\n", i);
    if(i<0) return -1;
    struct instruction instruction = load_text(i);
    if(execute(instruction)<0) {
      assembly(instruction, stderr);
      show_registers();
      return -1;
    }
    count_exec();
    if(stat_switch>0){
      if(get_stat(instruction)<0){
        assembly(instruction, stderr);
        return -1;
      }
    }
    if(instruction.opcode==0b0000000){
      break;
    }
    else if(instruction.opcode==0b0011011){
      printf("%c", uart);
    }
  }
  if(stat_switch==0) print_count(stderr);
  return 0;
}
