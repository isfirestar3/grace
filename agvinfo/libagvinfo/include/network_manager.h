#pragma once
#include "singleton.hpp"
#include "network_task.h"
#include "task_scheduler.hpp"

class network_manager
{
private:
	network_manager();
	~network_manager();
	friend class nsp::toolkit::singleton<network_manager>;

public:
	int init_task_pool();

	int push_task(std::string packet, std::function<int(nsp::proto::proto_interface *)> function_psend);
private:
	std::shared_ptr<nsp::toolkit::task_thread_pool<network_task>> thread_pool_ = nullptr;
};

