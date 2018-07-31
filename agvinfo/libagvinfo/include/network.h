#pragma once
#include "stdint.h"
#include "motion_ack.h"
#include "singleton.hpp"
#include "map"
#include "thread"
#include <condition_variable>

class network
{
private:
	network();
	~network();

	friend class nsp::toolkit::singleton<network>;
public:
	uint32_t pid(){ return  pid_++; };
	
	int write(uint32_t pid, std::shared_ptr<agv_motion::motion_ack> &motion, const std::function<int()> &sender);

	void exec(uint32_t, void *,int );

	void reg_notify_callback(std::function<void()> notify_callback){ notify_callback_ = notify_callback; };

	void notify();
private:
	int push(uint32_t pid, std::shared_ptr<agv_motion::motion_ack> & motion);

	void remove(uint32_t pid);

	int check_time();

	void notify_timeout(std::shared_ptr<agv_motion::motion_ack> &motion);

private:
	nsp::os::waitable_handle timeout_waiter_;
	std::map<uint32_t, std::shared_ptr<agv_motion::motion_ack>> map_callback_;
	std::mutex mutex_;
	std::thread * thread_ = nullptr;
	std::condition_variable condition_;
	std::function<void()> notify_callback_ = nullptr;
	int thread_exit_ = 0;
	uint32_t pid_ = 1;
};

