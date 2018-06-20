#pragma once

#include <stdint.h>
#include <map>
#include <mutex>

struct transfer_info{
	uint64_t file_total_size_ = 0;
	uint32_t file_total_block_ = 0;
	uint32_t current_block_index_ = 0;
	bool is_finished_ = false;
};

class sender_pull_long_task{

public:
	sender_pull_long_task();
	~sender_pull_long_task();

public:
	bool set_file_info(const uint64_t pull_id, const uint64_t file_size, const uint32_t pre_block_size);
	bool pull_block_request_info(const uint64_t pull_id, uint64_t& offset, uint32_t& read_size);
	bool is_file_complete(const uint64_t pull_id);
	bool get_file_size(const uint64_t pull_id,uint64_t& result);
	bool remove_file_id(const uint64_t pull_id);

private:
	std::recursive_mutex re_mutex_;
	std::map<uint64_t, transfer_info> map_transfer_;

	//每一片默认的传输的数据块大小
	uint32_t block_size_pre_transfer_;
};
