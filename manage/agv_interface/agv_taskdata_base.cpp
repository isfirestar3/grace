#include "agv_taskdata_base.h"


agv_taskdata_base::agv_taskdata_base(agv_task_types type)
:__taskdata_type(type), __task_phase(AgvTaskPhase_None)
{

}



agv_taskdata_base::~agv_taskdata_base()
{
}

agv_task_phase agv_taskdata_base::get_task_phase()
{
    return __task_phase;
}

void agv_taskdata_base::set_task_phase(agv_task_phase p)
{
    __task_phase = p;
}

agv_task_types agv_taskdata_base::get_taskdata_type()
{
    return __taskdata_type;
}



agv_taskdata_header::agv_taskdata_header(agv_task_types type)
:agv_taskdata_base(type), __fn(nullptr)
{

}

void agv_taskdata_header::set_task_status_callback(std::function<void(uint64_t taskid, status_describe_t status, int err, void* user)> fn,void* user)
{
    __fn = fn;
    __user = user;
}

agv_taskdata_header::~agv_taskdata_header()
{

}

void agv_taskdata_header::callback_task_status(status_describe_t status, int err)
{
    __fn(__task_id, status, err, __user);
}

uint64_t agv_taskdata_header::get_task_id()
{
    return __task_id;
}

void agv_taskdata_header::set_task_id(uint64_t id)
{
    __task_id = id;
}
