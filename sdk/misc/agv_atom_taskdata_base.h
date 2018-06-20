/*!
 * file agv_atom_task_base.h
 * date 2017/08/03 16:52
 *
 * author chenkai
 *
 * brief
 *
 * note
 */

#ifndef __AGV_ATOM_TASKDATA_BASE_H_CK__
#define __AGV_ATOM_TASKDATA_BASE_H_CK__

#include "agv_task_define.h"
#include "vartypes.h"
#include "agv_taskdata_base.h"
#include <mutex>
#include <vector>
#include <memory>
#include <atomic>

class agv_atom_taskdata_base:public agv_taskdata_base
{
public:
    agv_atom_taskdata_base(agv_atom_taskdata_types dttype);
    ~agv_atom_taskdata_base();

    agv_atom_taskdata_types get_atomtaskdata_type();
    void set_atomtaskseq_idex(int idx);
    int get_atomtaskseq_idex();
private:
    agv_atom_taskdata_types __atomtaskdata_type;
    int __atomtaskseq_idex = -1;
};

class agv_atom_taskdata_header :public agv_atom_taskdata_base, public agv_taskdata_header
{
public:
    agv_atom_taskdata_header(agv_atom_task_types type);
    ~agv_atom_taskdata_header();

    uint64_t get_atomtask_id();
    void set_atomtask_id(uint64_t id);
    agv_atom_task_types get_atom_tasktype();
private:
    uint64_t __task_id;
    agv_atom_task_types __atom_task_type;
};



class agv_atom_taskdata_check_base :public  agv_atom_taskdata_base
{
public:
    explicit agv_atom_taskdata_check_base(agv_atom_task_types task_type, agv_taskstatus_check_types ck);
    ~agv_atom_taskdata_check_base();

    agv_atom_task_types get_atom_task_type();
    agv_taskstatus_check_types get_check_types();
    virtual int check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type) = 0;

    void set_check_timeout_thres_cnt(uint64_t thres_count = 1000);

    virtual bool check_timeout();
protected:
    agv_atom_task_types __task_type;
    agv_taskstatus_check_types __check_type;

    std::atomic<uint64_t> __timeout_counter{0};
    uint64_t __timeout_thres_counter = 10;
};     


class agv_atom_taskdata_checkstatus_available :
    public agv_atom_taskdata_check_base
{
public:
    explicit agv_atom_taskdata_checkstatus_available(agv_atom_task_types task_type);
    ~agv_atom_taskdata_checkstatus_available();
    virtual int check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type);

};

class agv_atom_taskdata_checkstatus_idle :
    public agv_atom_taskdata_check_base 
{
public:
    explicit agv_atom_taskdata_checkstatus_idle(agv_atom_task_types task_type);
    ~agv_atom_taskdata_checkstatus_idle();
    virtual int check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type);

};


class agv_atom_taskdata_checkstatus_final :
    public  agv_atom_taskdata_check_base  
{
public:
    explicit agv_atom_taskdata_checkstatus_final(agv_atom_task_types task_type);
    ~agv_atom_taskdata_checkstatus_final();
    virtual int check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type);

};


class agv_atom_taskdata_checkstatus_running :
    public  agv_atom_taskdata_check_base
{
public:
    explicit agv_atom_taskdata_checkstatus_running(agv_atom_task_types task_type);
    ~agv_atom_taskdata_checkstatus_running();
    virtual int check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type);

};


class agv_atom_taskdata_checkstatus_equalstatus :
    public  agv_atom_taskdata_check_base
{
public:
    explicit agv_atom_taskdata_checkstatus_equalstatus(agv_atom_task_types task_type, status_describe_t s);
    ~agv_atom_taskdata_checkstatus_equalstatus();
    virtual int check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type);
protected:
    status_describe_t __equal_condition;
};

class agv_atom_taskdata_checkstatus_multi_and :public  agv_atom_taskdata_check_base
{
public:
    agv_atom_taskdata_checkstatus_multi_and();
    ~agv_atom_taskdata_checkstatus_multi_and();
    virtual int check_status(status_describe_t s, bool task_id_matched, agv_atom_task_types type);

    void add_checkdata(std::shared_ptr<agv_atom_taskdata_check_base>);
private:
    std::mutex __mtx_vct_check_data;
    std::vector<std::shared_ptr<agv_atom_taskdata_check_base>> __vct_checkdata;
};



#endif

