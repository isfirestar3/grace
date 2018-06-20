#ifndef __OBJECT_SWITCH_H__
#define __OBJECT_SWITCH_H__

#include "netdriver_base.h"
#include "netdriver_omron_plc.h"
#include "netdriver_modbus.h"
#include <memory>

std::shared_ptr<netdriver_base> get_netdriver_ptr(const var__functional_object_t *d) {
    switch (d->type_) {
    case kVarType_OmronPLC:
        return std::shared_ptr<netdriver_base>(new netdriver_omron_plc());
    case  kVarType_ModBus_TCP:
        return std::shared_ptr<netdriver_modbus>(new netdriver_modbus());
    default:
        return nullptr;
    }
}

#endif