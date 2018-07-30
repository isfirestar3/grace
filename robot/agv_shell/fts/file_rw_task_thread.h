#pragma once

#include "data_type.h"
#include <thread>
#include <deque>
#include <mutex>
#include <atomic>

class file_rw_task_thread{
public:
	file_rw_task_thread();
	~file_rw_task_thread();

public:
	void add_write_task(const uint32_t link, const int pkt_id, const uint32_t block_num, 
		file::st_file_head*, const uint64_t offset, const std::string& stream);
	void add_read_task(const uint32_t link, const int pkt_id, const uint32_t block_num, 
		file::st_file_head*, uint64_t offset, uint32_t read_size);
	void stop_write_thread();
	void stop_read_thread();
	void clear_rw_deque(uint32_t link);

private:
	void write_run();
	void read_run();
	
private:
	std::thread* write_thread_ = nullptr;
	std::thread* read_thread_ = nullptr;

	std::recursive_mutex write_mutex_;
	std::deque<file::write_task_info> write_task_deque_;
	nsp::os::waitable_handle write_wait_;
	std::atomic<int> exit_write_thread_{0};

	std::recursive_mutex read_mutex_;
	std::deque<file::read_task_info> read_task_deque_;
	nsp::os::waitable_handle read_wait_;
	std::atomic<int> exit_read_thread_{ 0 };
};
