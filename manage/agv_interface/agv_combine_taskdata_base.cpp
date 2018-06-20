#include "agv_combine_taskdata_base.h"


agv_combine_taskdata_base::agv_combine_taskdata_base(agv_combine_taskdata_types dttype)
:agv_taskdata_base(AgvTaskType_Combine), __combine_taskdata_type(dttype)
{
}


agv_combine_taskdata_base::~agv_combine_taskdata_base()
{
}

agv_combine_taskdata_types agv_combine_taskdata_base::get_combine_taskdata_type()
{
    return __combine_taskdata_type;
}

agv_combine_taskdata_header::agv_combine_taskdata_header(agv_combine_task_types type)
:agv_combine_taskdata_base(AgvCombineTaskData_Header), agv_taskdata_header(AgvTaskType_Combine)
{

}

agv_combine_taskdata_header::~agv_combine_taskdata_header()
{

}
 agv_combine_task_types agv_combine_taskdata_header::get_combine_tasktype()
{
    return __combine_task_type;
}
uint64_t agv_combine_taskdata_header::get_goto_task_id()
{
    return __goto_task_id;
}

void agv_combine_taskdata_header::set_goto_task_id(uint64_t id)
{
    __goto_task_id = id;
}

uint64_t agv_combine_taskdata_header::get_action_task_id()
{
    return __action_task_id;
}


void agv_combine_taskdata_header::set_action_task_id(uint64_t id)
{
    __action_task_id = id;
}

void agv_combine_taskdata_header::set_has_goto_task(bool has)
{
    __has_goto_task = has;
}

bool agv_combine_taskdata_header::has_goto_task()
{
    return __has_goto_task;
}

void agv_combine_taskdata_header::set_has_action_task(bool has)
{
    __has_action_task = has;
}

bool agv_combine_taskdata_header::has_action_task()
{
    return __has_action_task;
}

agv_combine_taskdata_check_base::agv_combine_taskdata_check_base(agv_combine_task_types task_type, agv_taskstatus_check_types ck)
:agv_combine_taskdata_base(AgvCombineTaskData_CheckStatus), __check_type(ck), __task_type(task_type)
{

}

agv_combine_taskdata_check_base::~agv_combine_taskdata_check_base()
{

}

agv_combine_task_types agv_combine_taskdata_check_base::get_combine_task_type()
{
    return __task_type;
}

agv_taskstatus_check_types agv_combine_taskdata_check_base::get_check_type()
{
    return __check_type;
}

agv_combine_taskdata_checkstatus_final::agv_combine_taskdata_checkstatus_final(agv_combine_task_types task_type)
:agv_combine_taskdata_check_base(task_type, AgvTaskStatus_CheckStatus_Final)
{

}

agv_combine_taskdata_checkstatus_final::~agv_combine_taskdata_checkstatus_final()
{

}

int agv_combine_taskdata_checkstatus_final::check_status(status_describe_t s, bool task_id_matched)
{
    if (s > kStatusDescribe_FinalFunction&& task_id_matched)
    {
        return 0;
    }

    return -1;
}
