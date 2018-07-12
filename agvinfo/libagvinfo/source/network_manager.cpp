#include "network_manager.h"
#include "log.h"

#define  MAX_TASK_POOL_SIZE 4

network_manager::network_manager()
{
	init_task_pool();
}

network_manager::~network_manager()
{
	if (thread_pool_){
		thread_pool_->join();
	}
}

int network_manager::init_task_pool()
{
	if (!thread_pool_){
		try{
			thread_pool_ = std::make_shared<nsp::toolkit::task_thread_pool<network_task>>(MAX_TASK_POOL_SIZE);
		}
		catch (...){
			nsperror << "init task pool failed";
			return -1;
		}
	}
	return 0;
}

int network_manager::push_task(std::string packet,std::function<int (nsp::proto::proto_interface *)> function_psend )
{
	if (!thread_pool_){
		return -1;
	}

	std::shared_ptr<network_task> task = nullptr;
	try{
		task = std::make_shared<network_task>(packet, function_psend);
	}
	catch(...){
		loinfo("libagvinfo") << "create network_task failure";
		return -1;
	}

	thread_pool_->post(task);

	return 0;
}