#include "agv_atom_taskdata_base.h"


agv_atom_taskdata_base::agv_atom_taskdata_base(agv_atom_taskdata_types type)
:__atomtaskdata_type(type), agv_taskdata_base(AgvTaskType_Atom)
{

}


agv_atom_taskdata_base::~agv_atom_taskdata_base()
{
}

agv_atom_taskdata_types agv_atom_taskdata_base::get_atomtaskdata_type()
{
    return __atomtaskdata_type;
}

void agv_atom_taskdata_base::set_atomtaskseq_idex(int idx)
{
    __atomtaskseq_idex = idx;
}

int agv_atom_taskdata_base::get_atomtaskseq_idex()
{
    return __atomtaskseq_idex;
}

uint64_t agv_atom_taskdata_header::get_atomtask_id()
{
    return __task_id;
}

agv_atom_taskdata_header::agv_atom_taskdata_header(agv_atom_task_types t)
:agv_atom_taskdata_base(AgvAtomTaskData_Header), __task_id(-1), agv_taskdata_header(AgvTaskType_Atom), __atom_task_type(t)
{

}

agv_atom_taskdata_header::~agv_atom_taskdata_header()
{

}

void agv_atom_taskdata_header::set_atomtask_id(uint64_t id)
{
    __task_id = id;
}

agv_atom_task_types agv_atom_taskdata_header::get_atom_tasktype()
{
    return __atom_task_type;
}

agv_atom_taskdata_check_base::agv_atom_taskdata_check_base(agv_atom_task_types task_type, agv_taskstatus_check_types ck)
:__task_type(task_type), agv_atom_taskdata_base(AgvAtomTaskData_CheckStatus), __check_type(ck)
{
}

agv_atom_taskdata_check_base::~agv_atom_taskdata_check_base()
{
}

agv_atom_task_types agv_atom_taskdata_check_base::get_atom_task_type()
{
    return __task_type;
}

agv_taskstatus_check_types agv_atom_taskdata_check_base::get_check_types()
{
    return __check_type;
}

void agv_atom_taskdata_check_base::set_check_timeout_thres_cnt(uint64_t thres_count /*= 1000*/)
{
    __timeout_thres_counter = thres_count;
}

bool agv_atom_taskdata_check_base::check_timeout()
{
    if (++__timeout_counter > __timeout_thres_counter)
    {
        __timeout_counter = 0;
        return true;
    }

    return false;
}


agv_atom_taskdata_checkstatus_available::agv_atom_taskdata_checkstatus_available(agv_atom_task_types task_type)
:agv_atom_taskdata_check_base(task_type, AgvTaskStatus_CheckStatus_Avaliable)
{
}


agv_atom_taskdata_checkstatus_available::~agv_atom_taskdata_checkstatus_available()
{
}

int agv_atom_taskdata_checkstatus_available::check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type)
{
    if (__task_type != type)
    {
        return -1;
    }
    if (s > kStatusDescribe_FinalFunction ||
        s < kStatusDescribe_PendingFunction)
    {
        return 0;
    }

    return -1;
}




agv_atom_taskdata_checkstatus_idle::agv_atom_taskdata_checkstatus_idle(agv_atom_task_types task_type)
:agv_atom_taskdata_check_base(task_type, AgvTaskStatus_CheckStatus_Idle)
{
}

agv_atom_taskdata_checkstatus_idle::~agv_atom_taskdata_checkstatus_idle()
{
}

int agv_atom_taskdata_checkstatus_idle::check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type)
{
    if (__task_type != type)
    {
        return -1;
    }
    if (s < kStatusDescribe_PendingFunction)
    {
        return 0;
    }

    return -1;
}

agv_atom_taskdata_checkstatus_final::agv_atom_taskdata_checkstatus_final(agv_atom_task_types task_type)
:agv_atom_taskdata_check_base(task_type, AgvTaskStatus_CheckStatus_Final)
{
}

agv_atom_taskdata_checkstatus_final::~agv_atom_taskdata_checkstatus_final()
{
}

int agv_atom_taskdata_checkstatus_final::check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type)
{
    if (__task_type != type)
    {
        return -1;
    }
    if (s > kStatusDescribe_FinalFunction
        && task_id_matched)
    {
        return 0;
    }

    return -1;
}

agv_atom_taskdata_checkstatus_running::agv_atom_taskdata_checkstatus_running(agv_atom_task_types task_type)
:agv_atom_taskdata_check_base(task_type, AgvTaskStatus_CheckStatus_Running)
{
}

agv_atom_taskdata_checkstatus_running::~agv_atom_taskdata_checkstatus_running()
{
}

int agv_atom_taskdata_checkstatus_running::check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type)
{
    if (__task_type != type)
    {
        return -1;
    }
    if (s < kStatusDescribe_FinalFunction
        && s >kStatusDescribe_PendingFunction
        && task_id_matched)
    {
        return 0;
    }

    return -1;
}



agv_atom_taskdata_checkstatus_equalstatus::agv_atom_taskdata_checkstatus_equalstatus(agv_atom_task_types task_type,status_describe_t s)
: agv_atom_taskdata_check_base(task_type, AgvTaskStatus_CheckStatus_EqualStatus), __equal_condition(s)
{
}

agv_atom_taskdata_checkstatus_equalstatus::~agv_atom_taskdata_checkstatus_equalstatus()
{
}

int agv_atom_taskdata_checkstatus_equalstatus::check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type)
{
    if (__task_type != type)
    {
        return -1;
    }
    if (s == __equal_condition && task_id_matched)
    {
        return 0;
    }

    return -1;
}


agv_atom_taskdata_checkstatus_multi_and::agv_atom_taskdata_checkstatus_multi_and()
:agv_atom_taskdata_check_base(AgvAtomTaskType_Multi,AgvTaskStatus_CheckStatus_MultiAnd)
{

}

agv_atom_taskdata_checkstatus_multi_and::~agv_atom_taskdata_checkstatus_multi_and()
{

}

int agv_atom_taskdata_checkstatus_multi_and::check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type)
{

    std::lock_guard<decltype(__mtx_vct_check_data)> lock(__mtx_vct_check_data);
    for (std::vector<std::shared_ptr<agv_atom_taskdata_check_base>>::iterator itr = __vct_checkdata.begin();
        itr != __vct_checkdata.end();++itr)
    {
        if ((*itr)->check_status(s,task_id_matched,type) == 0)
        {
            __vct_checkdata.erase(itr);
            break;
        }
    }

    if (__vct_checkdata.empty())
    {
        return 0;
    }

    return -1;
}

void agv_atom_taskdata_checkstatus_multi_and::add_checkdata(std::shared_ptr<agv_atom_taskdata_check_base> ck)
{
    std::lock_guard<decltype(__mtx_vct_check_data)> lock(__mtx_vct_check_data);
    __vct_checkdata.push_back(ck);
}
