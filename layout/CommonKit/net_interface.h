#pragma once
#include "net_impls.h"
#include "optpar.h"
#include "BasicDefine.h"
#include "AlgorithmDefine.hpp"
#include "os_util.hpp"
#include <atomic>
#include <mutex>
#include "agv_base.hpp"
#include "singleton.hpp"

#define QUERY_SUCCESS        0    //查询成功
#define QUERY_OUT_TIME       -1   //查询数据超时
#define READY_DATA_FAILED    -2   //准备数据失败，没有查询到数据完成标志
#define QUERY_DATA_FAILED    -3   //查询数据失败
#define POST_DATA_FAILED     -4   //发送数据失败

class net_interface:public agv_base
{
public:
	net_interface();
	virtual ~net_interface();
	/*public:
	static net_interface* Instance()
	{
		static net_interface net_instance;
		return &net_instance;
	}*/
	friend class nsp::toolkit::singleton<net_interface>;
	virtual int ready(bool& ready);
	virtual int get_voltage(double& vol) const
	{
		return 0;
	}
	int goto_charge(int dock_id, int charge_on, uint64_t& task_id, std::function<void(uint64_t taskid, status_describe_t status, int err, void* user)> fn, void* user = nullptr);
	int get_elongate_variable();
	void get_elongate_variable_ack(uint32_t id, const void *data);
	virtual int get_external_fault_describe(std::vector<std::string> & v) {
		return 0;
	}
public:
	int connect_net();
	int net_login(const std::string& ip, int port);
	void set_callback_notify();
	int get_opt_param(OPTINFO& opt_info);
	int get_vehicle_pos(POSXYA& posXya);
	int get_vehicle_pos_asyn();
	int get_vehicle_upl(UPL& upl);
	int get_nav_info(var__navigation_t& nav);
	int post_nav_task(const upl_t &dest_upl, const position_t &dest_pos, const std::vector<trail_t> &vec_trail);
	int post_cancel_nav_task();
	int post_suspend_nav_task();
	int post_resume_nav_task();
private:
	void callback_rec_data(int32_t robot_id, const void *data, int type);
	void callback_rev_pos();
	int query_opt_param_ready();
	int query_opt_param(OPTINFO& opt_info);
private:
	std::recursive_mutex mutex_vehicle_ip;
	std::string vehicle_ip;
	nsp::os::waitable_handle wait_param_complete;
	int robot_id_ = 1;
	std::atomic<uint32_t> _robot_id;
	std::shared_ptr<agv_base> agv_ptr_;
};

