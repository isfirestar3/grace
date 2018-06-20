#include "task_manager.h"
#include "log.h"
#include "network_client_manager.h"

#define NET_WINDOW_SIZE     2

task_manager::task_manager( )
{

}

task_manager::~task_manager()
{
	if (net_task_spool_)
	{
		net_task_spool_->join();
	}
}

int task_manager::init_thread()
{
	if (!net_task_spool_){
		try{
			net_task_spool_ = std::make_shared<nsp::toolkit::task_thread_pool<net_task>>(NET_WINDOW_SIZE);
		}
		catch (...){
			nsperror << "start_task_thead failed!";
			return -1;
		}
	}
	return 0;
}

void task_manager::init_net_work(const std::string &localip, const std::string &routeip, uint16_t port, uint8_t node_id, uint8_t sub_code)
{
	ip_local_ = localip;
	ip_addr_ = routeip;
	port_ = port;
	node_id_ = node_id;
	sub_code_ = sub_code;
}

void task_manager::push_task(task_type_t tasktype, funcalltask_t callback, uint16_t register_index)
{
	std::shared_ptr<net_task> task = nullptr;

	if (!net_task_spool_){
		return;
	}

	try{
		task = std::make_shared<net_task>(ip_local_, ip_addr_, port_, node_id_, sub_code_, tasktype, register_index, callback);
	}
	catch (...){
		nsperror << "new net_task make shared failed!";
		return;
	}
	//if (task){
	//	task->register_callback(callback);
	//}

	net_task_spool_->post(task);
}

void task_manager::push_task_attach_data(task_type_t tasktype, uint16_t register_index, const std::string &data, funcalltask_t callback)
{
	std::shared_ptr<net_task> task = nullptr;

	if (!net_task_spool_){
		return;
	}

	try{
		task = std::make_shared<net_task>(ip_local_, ip_addr_, port_, node_id_, sub_code_, tasktype, register_index, data, callback);
	}
	catch (...){
		nsperror << "new net_task make shared failed!";
		return;
	}

	net_task_spool_->post(task);
}

//int task_manager::task_no_wait()
//{
//	//int res = nsp::toolkit::singleton<network_client_manager>::instance()->post_read_register_cmd(ip_addr_,
//	//	std::make_shared<motion::asio_block>(std::bind(&task_manager::recv_data_callback, this, std::placeholders::_1)), serial_code_, node_id_, 0);
//
//	//if (res < 0)
//	//{
//	//	if (message_callback)
//	//	{
//	//		message_callback(ip_addr_, 0);
//	//	}
//	//	return -1;
//	//}
//
//	return 0;
//}

void task_manager::recv_data_callback(const void* data)
{
	recv_data recv_data_;
	if (!data){
		return;
	}
	recv_data_ = *(recv_data*)data;

	if (recv_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		if (message_callback)
		{
			message_callback(ip_addr_, 0);
		}
		return;
	}
	else
	{
		if (message_callback)
		{
			message_callback(ip_addr_, 0);
		}
	}
}