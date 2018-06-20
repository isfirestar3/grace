#ifndef SENDER_PULL_TASK_H
#define SENDER_PULL_TASK_H

#include <stdint.h>

class sender_pull_task{
public:
	sender_pull_task();
	~sender_pull_task();

public:
	void set_file_info(const uint64_t file_size, const uint32_t pre_block_size);
	void pull_block_request_info(uint64_t& offset, uint32_t& read_size);
	bool is_file_complete(){ return is_finished_; }
	uint64_t get_file_size(){ return file_total_size_; }

private:
	//uint32_t link_ = 0;
	uint64_t file_total_size_ = 0;
	uint32_t file_total_block_ = 0;
	uint32_t current_block_index_ = 0;

	//每一片默认的传输的数据块大小
	uint32_t block_size_pre_transfer_;
	bool is_finished_ = false;
};

#endif