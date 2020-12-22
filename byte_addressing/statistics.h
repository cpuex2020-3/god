#ifndef STATISTICS_H_INCLUDED
#define STATISTICS_H_INCLUDED

#include "instruction.h"

void count_exec();
void print_count(FILE *fp);

signed char get_stat(struct instruction instruction);
signed char show_statistics(char *output_file_s);

#endif
