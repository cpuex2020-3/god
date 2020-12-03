#ifndef FDATA_H_INCLUDED
#define FDATA_H_INCLUDED

// 将来もっと細かいレベルでコアを見たくなったときのために。
int32_t rm;
int32_t fcsr;

// 整数のときと同じ。参照する配列が違うだけ。
void f_show_registers();
int f_index_register(char *name);
int32_t f_load_regster(int index);
void f_store_register(int index, int32_t value);

#endif
