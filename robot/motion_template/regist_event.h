#ifndef __MT_REGIST_EVENT_H__
#define __MT_REGIST_EVENT_H__

#include "compiler.h"
#include "nisdef.h"
#include "posix_wait.h"

__extern__
int nspi__init_regist_cycle_event();
__extern__
void nspi__uninit_regist_cycle_event();
__extern__
int nspi__regist_cycle_event(HTCPLINK link, const char *data, int cb);
__extern__
int nspi__unregist_cycle_event(HTCPLINK link, const char *data, int cb);

#endif //__MT_REGIST_EVENT_H__
