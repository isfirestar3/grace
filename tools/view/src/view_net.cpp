#include "view_net.h"


agv_api::agv_api() :agv_base(kVehicleAgv_Type_CartAgv)
{

}
agv_api::~agv_api(){}

int agv_api::ready(bool& ready){ ready = true; return 0; }

int agv_api::get_elongate_variable(){ return 0; }

void agv_api::get_elongate_variable_ack(uint32_t id, const void *data){}

int agv_api::goto_charge(int dock_id, int charge_on, uint64_t& task_id, std::function<void(uint64_t taskid, status_describe_t status, int err, void* user)> fn, void* user){ return 0; }
