#include "sender_push_task.h"
#include "sender_manager.h"

sender_push_task::sender_push_task(const uint32_t link)
	:link_(link)
{

}

sender_push_task::~sender_push_task(){

}

void sender_push_task::set_file_info(const uint64_t file_size, const uint32_t file_block, const uint32_t pre_block_size){
	file_size_ = file_size;
	file_block_ = file_block;
	block_size_pre_transfer_ = pre_block_size;
}

int sender_push_task::push_file_block(const uint64_t offset){

	uint32_t read_size = 0;
	if (current_block_ == (file_block_ - 1)){
		//当前片为最后一片数据
		read_size = file_size_ - (uint64_t)current_block_ * (uint64_t)block_size_pre_transfer_;
	}
	else{
		read_size = block_size_pre_transfer_;
	}

	current_block_++;
	return nsp::toolkit::singleton<sender_manager>::instance()->fts_get_file_block(link_, offset, read_size);
}