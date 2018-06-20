#include "agv_combine_taskdata_gotoaction.h"


agv_combine_taskdata_gotoaction::agv_combine_taskdata_gotoaction()
:agv_combine_taskdata_base(AgvCombineTaskData_NewGotoAction)
{
}


agv_combine_taskdata_gotoaction::~agv_combine_taskdata_gotoaction()
{
}

uint64_t agv_combine_taskdata_gotoaction::get_task_id()
{
    return __task_id;
}

void agv_combine_taskdata_gotoaction::set_task_id(uint64_t id)
{
    __task_id = id;
}

void agv_combine_taskdata_gotoaction::set_task_status_callback(std::function<void(uint64_t taskid, status_describe_t status, int err, void* user)> fn, void* user /*= nullptr*/)
{
    __fn_result = fn;
    __user = user;
}

void agv_combine_taskdata_gotoaction::callback_task_status(status_describe_t status, int err)
{
    __fn_result(__task_id, status, err, __user);
}

agv_combine_taskdata_pausegotoaction::agv_combine_taskdata_pausegotoaction()
:agv_combine_taskdata_base(AgvCombineTaskData_PauseGotoAction)
{

}

agv_combine_taskdata_pausegotoaction::~agv_combine_taskdata_pausegotoaction()
{

}

agv_combine_taskdata_resumegotoaction::agv_combine_taskdata_resumegotoaction()
:agv_combine_taskdata_base(AgvCombineTaskData_ResumeGotoAction)
{

}

agv_combine_taskdata_resumegotoaction::~agv_combine_taskdata_resumegotoaction()
{

}

agv_combine_taskdata_cancelgotoaction::agv_combine_taskdata_cancelgotoaction()
:agv_combine_taskdata_base(AgvCombineTaskData_CancelGotoAction)
{

}

agv_combine_taskdata_cancelgotoaction::~agv_combine_taskdata_cancelgotoaction()
{

}
