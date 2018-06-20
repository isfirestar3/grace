#ifndef __OBJECT_SWITCHER_H__
#define __OBJECT_SWITCHER_H__
#include <memory>
#include "canbus_kvaser.h"
#include "driver_elmo.h"
#include "driver_moons.h"
#include "driver_angle.h"
#include "driver_dio.h"
#include "driver_curtis.h"
#include "var.h"
#include "canio.h"

/*
如果增加新类型，则需要继承对应基类编写对应的子类即可

然后根据对应类型在本文件中创建对应的对象

!!其他文件请勿修改
*/


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
		return std::shared_ptr<driver_curtis>(new driver_curtis());
	default:
		return nullptr;
	}
}

#endif