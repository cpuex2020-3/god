#ifndef INSTRUCTION_H_INCLUDED
#define INSTRUCTION_H_INCLUDED

struct instruction {
    int opcode;
    int funct3;
    int funct7;
    int rd_index;
    int rs1_index;
    int rs2_index;
    int32_t imm;
};

signed char eqlstr(char *a, char *b);

#endif
