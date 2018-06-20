#pragma once

#include "singleton.hpp"
#include "gzfts_api.h"
#include <thread>
#include <deque>
#include "os_util.hpp"

struct pull_task_item {
	std::string target_ipv4_;
	uint16_t target_port_;
	std::string local_path_;
	std::string remote_path_;
};

class fts_pull_task_group {
public:
	fts_pull_task_group();
	~fts_pull_task_group();

public:
	void add_pull_task(const char* target_ip, const uint16_t port, const std::string& remote_file, const std::string& local_file);

private:
	void initlization();
	void uint();

	void task_run();

private:
	std::thread* task_thread_ = nullptr;
	std::atomic<bool> exist_thread_{ false };
	std::recursive_mutex r_mutex_;
	std::deque<pull_task_item> pull_deque_;
	nsp::os::waitable_handle timeout_wait_;
	nsp::os::waitable_handle pull_waiter_;
};