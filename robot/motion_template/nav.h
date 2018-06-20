#ifndef __NAVIGATION_ENTRY_H__
#define __NAVIGATION_ENTRY_H__

#include "compiler.h"
#include "clist.h"

#include "navigation.h"
#include "vehicle.h"

/* simflag > 0 则仿真 */
extern
int nav__traj_control(var__navigation_t *nav, var__vehicle_t *veh, const struct list_head *drive_unit_entry, posix__boolean_t simflag);

#endif