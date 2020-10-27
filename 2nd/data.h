#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED
#include "instruction.h"

int32_t pc;
void show_registers();  // x0-x31を出力。

// simulatorの最初と最後に実行。
signed char init_data(int32_t size_memory_quarter);  // memoryの確保, sp,gpの初期化。mallocミスで -1。
void free_memory();  // mallocしたmemoryの解放。

// name/addressをindex_uoに渡す。例外処理をしなさーい。 -> indexをload_uo,store_uoに渡す。
int index_register(char *name);  // abi_nameからindexに変換。ただし存在しないregisterを指している場合は -1。
int32_t load_regster(int index);
void store_register(int index, int32_t value);
int index_text(int32_t program_counter);  // pcからindexに変換。text領域外参照や4 alignmentされていないを場合は -1。
struct instruction load_text(int index);
void store_text(int index, struct instruction instruction);
int index_memory(int32_t address);  // addressからindexに変換。範囲外参照や4 alignmentされていないを場合は -1。
int32_t load_memory(int index);
void store_memory(int index, int32_t value);

#endif
