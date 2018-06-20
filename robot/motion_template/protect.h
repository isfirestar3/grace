#ifndef __SAFETY_H__
#define __SAFETY_H__

#include "var.h"

extern int safety__init();
extern int safety__proc();
extern int safety__enable(posix__boolean_t en);
extern int safety__set_manual_bank(int bank_id, int level);

#endif