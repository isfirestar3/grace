#pragma once

#include "singleton.hpp"
#include "task_scheduler.hpp"


enum task_type_t{
	kTaskType_Unknown = 0,	// unknown type
	kTaskType_Recv,			// recv packet
	kTaskType_Timeout		// packet timeout
};
class base_task_t {
	task_type_t task_type_ = kTaskType_Unknown;
	std::function<void(const unsigned char *, int)> recv_handler = nullptr;
	std::function<void(uint32_t, const void *)> notify_fun_ = nullptr;
	unsigned char *data_ = nullptr;
	int cb_ = 0;

public:
	base_task_t(const unsigned char *data, int cb, const std::function<void(const unsigned char *, int)> &handler);
	base_task_t(const base_task_t &lref);
	base_task_t(base_task_t &&rref);
	~base_task_t();
	void on_task();
};

class session_task_manager {
	// thread pool for package receiver and dispacth
	nsp::toolkit::task_thread_pool<base_task_t>	task_pool_;
	
	session_task_manager();
	~session_task_manager();
	friend class nsp::toolkit::singleton<session_task_manager>;

public:
	// asynchronous transfer.
	void schedule_async_receive(const std::string &pkt, const std::function<void(const unsigned char *,int)> &handler);
};

class session_keepalive_manager {
	// thread pool for package receiver and dispacth
	nsp::toolkit::task_thread_pool<base_task_t>	task_pool_;

	session_keepalive_manager();
	~session_keepalive_manager();
	friend class nsp::toolkit::singleton<session_keepalive_manager>;

public:
	// asynchronous transfer.
	void schedule_async_receive(const std::string &pkt, const std::function<void(const unsigned char *, int)> &handler);
};
