/*!
 * file agv_taskdata_base.h
 * date 2017/08/14 11:14
 *
 * author chenkai
 *
 * brief
 *
 * note
 */
#ifndef __AGV_TASKDATA_BASE_H__
#define __AGV_TASKDATA_BASE_H__

#include "vartypes.h"
#include "agv_task_define.h"
#include <functional>

class agv_taskdata_base
{
public:
    agv_taskdata_base(agv_task_types type);
    ~agv_taskdata_base();

    agv_task_types get_taskdata_type();

    void set_task_phase(agv_task_phase p);
    agv_task_phase get_task_phase();
private:
    agv_task_types __taskdata_type;
    agv_task_phase __task_phase;
};

class agv_taskdata_header :public agv_taskdata_base
{
public:
    agv_taskdata_header(agv_task_types type);
    ~agv_taskdata_header();

    uint64_t get_task_id();
    void set_task_id(uint64_t id);

    void set_task_status_callback(std::function<void(uint64_t taskid, status_describe_t status, int err, void* user)> fn, void* user = nullptr);

public:

    void callback_task_status(status_describe_t status, int err);
private:
    std::function<void(uint64_t taskid, status_describe_t status, int err, void* user)>   __fn;
    void* __user;
    uint64_t __task_id;
};


#endif


