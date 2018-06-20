#pragma once

#include <thread>
#include <vector>
#include <deque>
#include "endpoint.h"
#include "os_util.hpp"
#include "log.h"
#include <mutex>

#define total_window_size 1

struct push_file_info{
	std::string local_file;
	std::string remote_file;
};

struct task_item_info{
	uint64_t task_id;
	std::string ip;
	uint16_t port;
	std::vector<push_file_info> vct_file;
};

class fts_push_task_group{
public:
	fts_push_task_group();
	~fts_push_task_group();

public:
	uint64_t get_random_id();
	uint64_t add_push_task(const char* target_ip, const uint16_t port, const std::vector<push_file_info>& vct_file, const uint64_t task_group = 0);
	void reduce_push_number();
	void remove_push_task(const uint64_t task_id);

private:
	uint32_t allocate_random();
	void initlization();
	void task_run();

private:
	std::thread* task_thread_ = nullptr;
	std::atomic<bool> exist_thread_{ false };
	//当前正在push操作文件总数，该总数定义不能超过5，如果当前总数已满足5，则后面的push文件需要等待10ms
	std::atomic<uint32_t> current_push_number_ = 0;

	std::recursive_mutex r_mutex_;
	std::deque<task_item_info> deque_task_;
	nsp::os::waitable_handle timeout_waiter_;
	nsp::os::waitable_handle push_waiter_;
	//是否退出当前task标识
	std::atomic<bool> exist_curr_task_{ false };
};