#if !defined PERIOD_STORAGE_H
#define PERIOD_STORAGE_H

#include "compiler.h"

extern
int mm__load_mapping();
extern
void mm__release_mapping();
extern
int mm__read_mapping(uint32_t offset, uint32_t len, void *data);
extern
int mm__write_mapping(uint32_t len, const void *data);

extern
int mm__getupl(void *upl);
extern
int mm__setupl(const void *upl);

#endif