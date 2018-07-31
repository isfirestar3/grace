#pragma once

#include "task_scheduler.hpp"
#include "singleton.hpp"

class recv_handler
{
public:
	recv_handler(const unsigned char *data, int cb, const std::function<void(const unsigned char *, int &)> &handler);
	~recv_handler();

public:
	void on_task();

private:
	std::function<void(const unsigned char*, int&)>task_handler_ = nullptr;
	unsigned char* data_ = nullptr;
	int cb_ = 0;
};

class net_recv_manager
{
	net_recv_manager();
	~net_recv_manager();
	friend class nsp::toolkit::singleton<net_recv_manager>;

public:
	void async_recv_handler(const std::string& pkt,const std::function<void(const unsigned char*, int&)>&hanlder);

private:
	nsp::toolkit::task_thread_pool<recv_handler> post_handler_;
};