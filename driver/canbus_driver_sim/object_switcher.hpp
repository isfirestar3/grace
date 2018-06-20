#ifndef __OBJECT_SWITCHER_H__
#define __OBJECT_SWITCHER_H__

#include <memory>
#include "canbus_kvaser.h"
#include "driver_elmo.h"
#include "driver_moons.h"
#include "driver_angle.h"
#include "var.h"
#include "canio.h"
#include "driver_dio.h"
#include "driver_cutis.h"

std::shared_ptr<canbus_base> get_canbus_ptr(const _canbus_argv *can, int baud) {
	switch (can->type_) {
	case kCanDrvType_Kvaser:
		return std::shared_ptr<canbus_base>(new canbus_kvaser(can->port_, can->type_, baud));
	default:
		return nullptr;
	}
}

std::shared_ptr<driver_base> get_driver_ptr(const var__functional_object_t *d) {
	switch (d->type_) {
	case kVarType_Elmo:
		return std::shared_ptr<driver_base>(new driver_elmo());
	case kVarType_Moons:
		return std::shared_ptr<driver_base>(new driver_moons());
	case kVarType_AngleEncoder:
		return std::shared_ptr<driver_base>(new driver_angle());
	case kVarType_DIO:
		return std::shared_ptr<driver_base>(new driver_dio());
	case kVarType_Curtis:
		return std::shared_ptr<driver_base>( new driver_cutis() );
		
	default:
		return nullptr;
	}
}

#endif