/*!
 * file agv_combine_taskdata_gotoaction.h
 * date 2017/08/23 19:19
 *
 * author chenkai
 *
 * brief 
 *
 * note
*/

#ifndef __AGV_COMBINE_TASKDATA_GOTOACTION_H__
#define __AGV_COMBINE_TASKDATA_GOTOACTION_H__
#include "agv_combine_taskdata_base.h"
#include "agv_atom_taskdata_base.h"
#include <vector>
#include <functional>
#include <memory>

class agv_combine_taskdata_gotoaction :public   agv_combine_taskdata_base
{
public:
    agv_combine_taskdata_gotoaction();
    ~agv_combine_taskdata_gotoaction();

public:   //input
    int dock_id = 0;
    int opt_id = -1;
    std::function<void(int dock, int opt, std::vector<std::shared_ptr<agv_combine_taskdata_base>>& v,void* user)> fn_action_logic = nullptr;
    void* logic_user = nullptr;
    
public: //do not call outside
    uint64_t get_task_id();
    void set_task_id(uint64_t id);
    void set_task_status_callback(std::function<void(uint64_t taskid, status_describe_t status, int err, void* user)> fn, void* user = nullptr);
    void callback_task_status(status_describe_t status, int err);
private:
    std::function<void(uint64_t taskid, status_describe_t status, int err, void* user)>   __fn_result;
    void* __user;
    uint64_t __task_id;
};

class agv_combine_taskdata_pausegotoaction :
    public agv_combine_taskdata_base
{
public:
    agv_combine_taskdata_pausegotoaction();
    ~agv_combine_taskdata_pausegotoaction();

};

class agv_combine_taskdata_resumegotoaction :
    public agv_combine_taskdata_base
{
public:
    agv_combine_taskdata_resumegotoaction();
    ~agv_combine_taskdata_resumegotoaction();

};

class agv_combine_taskdata_cancelgotoaction :
    public agv_combine_taskdata_base
{
public:
    agv_combine_taskdata_cancelgotoaction();
    ~agv_combine_taskdata_cancelgotoaction();

};
#endif
