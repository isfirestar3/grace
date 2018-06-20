/*!
 * file agv_combine_taskdata_action.h
 * date 2017/08/23 15:39
 *
 * author chenkai
 *
 * brief 
 *
 * note
*/

#ifndef __AGV_COMBINE_TASKDATA_ACTION_H__
#define __AGV_COMBINE_TASKDATA_ACTION_H__

#include "agv_combine_taskdata_base.h"
#include "agv_atom_taskdata_base.h"
#include <vector>
#include <functional>
#include <memory>

class agv_combine_taskdata_action:public agv_combine_taskdata_base
{
public:
    agv_combine_taskdata_action();
    ~agv_combine_taskdata_action();

public:
    int dock_id;
    int opt_id;
    std::function<int(int dock,int opt,std::vector<std::shared_ptr<agv_atom_taskdata_base>>& v,void* user)> fn_opt_logic;
    void* user;
};


#endif

