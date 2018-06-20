/*!
 * file agv_atom_taskdata_opt.h
 * date 2017/08/17 14:48
 *
 * author chenkai
 *
 * brief 
 *
 * note
*/

#ifndef __AGV_ATOM_TASK_DATA_OPT_H__
#define __AGV_ATOM_TASK_DATA_OPT_H__
#include "agv_atom_taskdata_base.h"
#include "operation.h"
#include "net_impls.h"
#include <vector>

class agv_atom_opt_taskdata_header
    :public agv_atom_taskdata_header
{
public:
    agv_atom_opt_taskdata_header();
    ~agv_atom_opt_taskdata_header();

};

class agv_atom_taskdata_newopt :
    public agv_atom_taskdata_base
{
public:
    agv_atom_taskdata_newopt();
    ~agv_atom_taskdata_newopt();
    var__operation_t __opt_data;
};


class agv_atom_taskdata_modifyopt :
    public agv_atom_taskdata_base
{
public:
    agv_atom_taskdata_modifyopt();
    ~agv_atom_taskdata_modifyopt();

public:
    std::vector<mn::common_data_item> __vec_common;
};

class agv_atom_taskdata_checkopt :
    public agv_atom_taskdata_base
{
public:
    agv_atom_taskdata_checkopt(const var__operation_t& opt2check);
    virtual ~agv_atom_taskdata_checkopt();

    //return 0 is ok
    virtual int check(const var__operation_t& opt) = 0;

protected:
    var__operation_t __opt2check;
};

class agv_atom_taskdata_pauseopt :
    public agv_atom_taskdata_base
{
public:
    agv_atom_taskdata_pauseopt();
    ~agv_atom_taskdata_pauseopt();

};

class agv_atom_taskdata_resumeopt :
    public agv_atom_taskdata_base
{
public:
    agv_atom_taskdata_resumeopt();
    ~agv_atom_taskdata_resumeopt();

};

class agv_atom_taskdata_cancelopt :
    public agv_atom_taskdata_base
{
public:
    agv_atom_taskdata_cancelopt();
    ~agv_atom_taskdata_cancelopt();

};

#endif