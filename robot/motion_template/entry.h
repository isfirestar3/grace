#if !defined ENTRY_H
#define ENTRY_H

#if !defined STD_CALL
#if WIN32
#define STD_CALL __stdcall
#else
#define STD_CALL
#endif
#endif

#include <stdint.h>
#include "posix_wait.h"

extern
int run__interval_control(posix__waitable_handle_t *waiter, uint64_t begin_tick, uint32_t maximum_delay);
extern
int run__register_event_callback(void( *)(void *functional_object));

#endif