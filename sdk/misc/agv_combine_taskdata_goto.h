/*!
 * file agv_combine_taskdata_goto.h
 * date 2017/08/23 15:11
 *
 * author chenkai
 *
 * brief 
 *
 * note
*/

#ifndef __AGV_COMBINE_TASKDATA_GOTO_H__
#define __AGV_COMBINE_TASKDATA_GOTO_H__

#include "agv_combine_taskdata_base.h"
class agv_combine_taskdata_goto  :public   agv_combine_taskdata_base
{
public:
    agv_combine_taskdata_goto();
    ~agv_combine_taskdata_goto();

public:
    int dock_id;
};



#endif


