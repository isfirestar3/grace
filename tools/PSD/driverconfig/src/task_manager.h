#pragma once
#include "singleton.hpp"
#include <memory>
#include "task_scheduler.hpp"
#include "net_task.h"

class task_manager
{
	friend class nsp::toolkit::singleton<task_manager>;
public:
	~task_manager();

	void init_net_work(const std::string &localip, const std::string &routeip, uint16_t port, uint8_t node_id, uint8_t sub_code);

	int init_thread();

	void push_task(task_type_t tasktype, funcalltask_t callback, uint16_t register_index = 0);

	void push_task_attach_data(task_type_t tasktype, uint16_t register_index = 0, const std::string &data = std::string(), funcalltask_t callback = nullptr);

	void recv_data_callback(const void* data);

	//int task_no_wait();
private:
	task_manager();
private:
	std::string ip_local_;				//本地IP
	std::string ip_addr_;				//对端IP
	uint16_t port_;						//端口
	uint8_t node_id_;					//Node ID
	uint8_t sub_code_;					//副操作码

	std::shared_ptr<nsp::toolkit::task_thread_pool<net_task>> net_task_spool_ = nullptr;

	funcalltask_t	message_callback;
};

