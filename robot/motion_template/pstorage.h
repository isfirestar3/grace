#if !defined PERIOD_STORAGE_H
#define PERIOD_STORAGE_H

#include "compiler.h"

#if !defined P_STORAGE_FILE_SIZE
#define P_STORAGE_FILE_SIZE  (64)
#endif

extern
int run__load_mapping();
extern
void run__release_mapping();
extern
int run__write_mapping(uint32_t len, const void *data);
extern
int run__read_mapping(uint32_t offset, uint32_t len, void *data);

#endif