#include "sender_pull_task.h"

sender_pull_task::sender_pull_task(){

}

sender_pull_task::~sender_pull_task(){

}

void sender_pull_task::set_file_info(const uint64_t file_size, const uint32_t pre_block_size){
	file_total_size_ = file_size;
	block_size_pre_transfer_ = pre_block_size;

	//计算块总数
	file_total_block_ = file_total_size_ / block_size_pre_transfer_;
	file_total_block_ = (file_total_size_ % block_size_pre_transfer_ == 0) ? file_total_block_ : (file_total_block_ + 1);
}

void sender_pull_task::pull_block_request_info(uint64_t& offset, uint32_t& read_size){
	offset = current_block_index_ * block_size_pre_transfer_;
	if (current_block_index_ == (file_total_block_ - 1)){
		//最后一片，计算大小
		read_size = file_total_size_ - (uint64_t)current_block_index_ * (uint64_t)block_size_pre_transfer_;
		is_finished_ = true;
	}
	else{
		read_size = block_size_pre_transfer_;
	}
	current_block_index_++;
}