#ifndef SENDER_TASK_H
#define SENDER_TASK_H

#include <stdint.h>

class sender_push_task{
public:
	sender_push_task(const uint32_t link);
	~sender_push_task();

public:
	void set_file_info(const uint64_t file_size, const uint32_t file_block, const uint32_t pre_block_size = 0x00010000);
	int push_file_block(const uint64_t offset);
	uint64_t get_file_size(){ return file_size_; }

private:
	uint32_t link_ = 0;
	uint64_t file_size_ = 0;
	uint32_t file_block_ = 0;
	uint32_t current_block_ = 0;

	//每一片默认的传输的数据块大小
	uint32_t block_size_pre_transfer_;
};

#endif