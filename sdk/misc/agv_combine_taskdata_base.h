/*!
 * file agv_combine_taskdata_base.h
 * date 2017/08/23 15:12
 *
 * author chenkai
 *
 * brief 
 *
 * note
*/

#ifndef __AGV_COMBINE_TASKDATA_BASE_H__
#define __AGV_COMBINE_TASKDATA_BASE_H__
#include "agv_task_define.h"
#include "agv_taskdata_base.h"

class agv_combine_taskdata_base :public agv_taskdata_base
{
public:
    agv_combine_taskdata_base(agv_combine_taskdata_types dttype);
    ~agv_combine_taskdata_base();

    agv_combine_taskdata_types get_combine_taskdata_type();
private:
    agv_combine_taskdata_types __combine_taskdata_type;
};

class agv_combine_taskdata_header :public agv_combine_taskdata_base, public agv_taskdata_header
{
public:
    agv_combine_taskdata_header(agv_combine_task_types type);
    ~agv_combine_taskdata_header();


    agv_combine_task_types get_combine_tasktype();
public:
    uint64_t get_goto_task_id();
    void set_goto_task_id(uint64_t id);

    uint64_t get_action_task_id();
    void set_action_task_id(uint64_t id);

    void set_has_goto_task(bool has);
    bool has_goto_task();

    void set_has_action_task(bool has);
    bool has_action_task();
private:
    uint64_t __goto_task_id = -1;
    uint64_t __action_task_id = -1;
    agv_combine_task_types __combine_task_type = AgvCombineTaskType_None;
    bool __has_goto_task = false;
    bool __has_action_task = false;
};

class agv_combine_taskdata_check_base :public  agv_combine_taskdata_base
{
public:
    explicit agv_combine_taskdata_check_base(agv_combine_task_types task_type, agv_taskstatus_check_types ck);
    ~agv_combine_taskdata_check_base();

    agv_combine_task_types get_combine_task_type();
    agv_taskstatus_check_types get_check_type();
    virtual int check_status(status_describe_t s, bool task_id_matched) = 0;
protected:
    agv_combine_task_types __task_type;
    agv_taskstatus_check_types __check_type;
};

class agv_combine_taskdata_checkstatus_final :
    public  agv_combine_taskdata_check_base
{
public:
    explicit agv_combine_taskdata_checkstatus_final(agv_combine_task_types task_type);
    ~agv_combine_taskdata_checkstatus_final();
    virtual int check_status(status_describe_t s,  bool task_id_matched);

};

#endif


