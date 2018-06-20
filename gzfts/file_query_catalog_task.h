#pragma once

#include <thread>
#include <string>
#include <deque>
#include <mutex>
#include "os_util.hpp"

class file_query_catalog_task{

	struct catalog_task_info{
		uint32_t link;
		uint32_t pkt_id;
		std::string file_path;
	};

public:
	file_query_catalog_task();
	~file_query_catalog_task();

public:
	void add_query_task(const uint32_t link, const uint32_t pkt_id, const std::string& f_path);

private:
	void query_task();

private:
	std::thread *run_task_ = nullptr;

	std::deque<catalog_task_info> deque_task_;
	std::recursive_mutex task_mutex_;
	nsp::os::waitable_handle task_wait_;
	std::atomic<int> exit_task_thread_{ 0 };
};