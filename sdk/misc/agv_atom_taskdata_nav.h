/*!
 * file agv_atom_taskdata_newnav.h
 * date 2017/08/03 17:18
 *
 * author chenkai
 *
 * brief 
 *
 * note
*/

#ifndef __AGV_ATOM_TASKDATA_NAV_H_CK__
#define __AGV_ATOM_TASKDATA_NAV_H_CK__

#include "agv_atom_taskdata_base.h"  
#include "navigation.h"
#include <vector>

class agv_atom_taskdata_pathsearch :
    public agv_atom_taskdata_base
{
public:
    agv_atom_taskdata_pathsearch();
    ~agv_atom_taskdata_pathsearch();

public://input
    upl_t destUpl;

};

class agv_atom_taskdata_newnav :
    public agv_atom_taskdata_base
{
public:
    agv_atom_taskdata_newnav();
    ~agv_atom_taskdata_newnav();

public://input
    uint64_t task_id;
    upl_t destUpl;
    position_t destPos;
    std::vector<trail_t> pathUpl;
    int path_is_whole;
};

class agv_atom_taskdata_addnav :
    public agv_atom_taskdata_base
{
public:
    agv_atom_taskdata_addnav();
    ~agv_atom_taskdata_addnav();
public://input
    int index;
    std::vector<trail_t> pathUpl;
    int path_is_whole;
};

class agv_atom_taskdata_pausenav :
    public agv_atom_taskdata_base
{
public:
    agv_atom_taskdata_pausenav();
    ~agv_atom_taskdata_pausenav();

};

class agv_atom_taskdata_resumenav :
    public agv_atom_taskdata_base
{
public:
    agv_atom_taskdata_resumenav();
    ~agv_atom_taskdata_resumenav();

};

class agv_atom_taskdata_cancelnav :
    public agv_atom_taskdata_base
{
public:
    agv_atom_taskdata_cancelnav();
    ~agv_atom_taskdata_cancelnav();

};

class agv_atom_nav_taskdata_header
    :public agv_atom_taskdata_header
{
public:
    agv_atom_nav_taskdata_header();
    ~agv_atom_nav_taskdata_header();

};


#endif


