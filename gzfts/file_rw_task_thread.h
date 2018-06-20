#pragma once

#include <thread>
#include <stdint.h>
#include <deque>
#include "data_type.h"
#include <mutex>
#include "os_util.hpp"
#include <atomic>
#include "file_write_handler.h"

struct write_task_info{
	void* handler_;
	uint32_t link_;
	nsp::file::file_block file_block_info_;
	nsp::file::current_identify identify_;
	bool is_long_lnk = false;
	uint64_t file_id;
};

struct read_task_info{
	void* handler_;
	uint32_t pkt_id_;
	uint32_t link_;
	uint64_t offset_;
	uint32_t read_size_;
	nsp::file::current_identify identify_;
	bool is_long_lnk = false;
	uint64_t file_id;
};

class file_rw_task_thread{
public:
	file_rw_task_thread();
	~file_rw_task_thread();

public:
	void add_write_task(void* handler, const uint32_t link, const nsp::file::file_block& f_data, 
		const nsp::file::current_identify& identify, const bool is_long_lnk = false, 
		const uint64_t f_id = 0);
	void stop_write_thread();
	void add_read_task(void* handler, const uint32_t pkt_id, const uint32_t link, const uint64_t offset,
		const uint32_t read_size, const nsp::file::current_identify identify, 
		const bool is_long_lnk = false, const uint64_t f_id = 0);
	void stop_read_thread();
	void clear_rw_deque(uint32_t link);

private:
	void write_run();
	void read_run();

private:
	std::thread* write_thread_ = nullptr;
	std::thread* read_thread_ = nullptr;

	std::recursive_mutex write_mutex_;
	std::deque<write_task_info> write_task_deque_;
	nsp::os::waitable_handle write_wait_;
	std::atomic<int> exit_write_thread_{0};

	std::recursive_mutex read_mutex_;
	std::deque<read_task_info> read_task_deque_;
	nsp::os::waitable_handle read_wait_;
	std::atomic<int> exit_read_thread_{ 0 };
};