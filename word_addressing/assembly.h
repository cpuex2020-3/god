#ifndef ASSEMBLY_H_INCLUDED
#define ASSEMBLY_H_INCLUDED

signed char assembly(struct instruction instruction, FILE *fp);
signed char post_parser(char *output_file_s);

#endif
