#ifndef FDATA_H_INCLUDED
#define FDATA_H_INCLUDED

// 将来もっと細かいレベルでコアを見たくなったときのために。
int32_t rm;
int32_t fcsr;

// 整数のときと大体同じ。
void f_show_registers();
int f_index_register(char *name);
float f_load_regster(int index);
void f_store_register(int index, float value);
// メモリ操作だけ少し変わってて、メモリとレジスタの間での受け渡しを関数内で行う形になってる。
void f_load_memory(int index_add, int index_f_reg);
void f_store_memory(int index_add, int index_f_reg);

#endif
