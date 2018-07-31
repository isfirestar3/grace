#if !defined GESN_H
#define GESN_H

#include <stdarg.h>

#include "compiler.h"
#include "logger.h"

extern
int gesn__config_server(const char *ipv4, uint16_t port);
extern
void gesn__stop();
extern
int gesn__publish(const char *context, int size);
extern
int gesn__write_event(const char *module, enum log__levels level, uint32_t code, const char *format,...);

#endif