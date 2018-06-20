#include "sender_pull_long_task.h"

sender_pull_long_task::sender_pull_long_task(){

}

sender_pull_long_task::~sender_pull_long_task(){

}

bool sender_pull_long_task::set_file_info(const uint64_t pull_id, const uint64_t file_size, const uint32_t pre_block_size){
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	auto iter = map_transfer_.find(pull_id);
	if (iter != map_transfer_.end())
	{
		return false;
	}
	transfer_info t_info;

	t_info.file_total_size_ = file_size;
	block_size_pre_transfer_ = pre_block_size;

	//计算块总数
	t_info.file_total_block_ = t_info.file_total_size_ / block_size_pre_transfer_;
	t_info.file_total_block_ = (t_info.file_total_size_ % block_size_pre_transfer_ == 0) ? t_info.file_total_block_ : (t_info.file_total_block_ + 1);

	map_transfer_.insert(std::make_pair(pull_id, t_info));
	return true;
}

bool sender_pull_long_task::pull_block_request_info(const uint64_t pull_id, uint64_t& offset, uint32_t& read_size){
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	auto iter = map_transfer_.find(pull_id);
	if (iter == map_transfer_.end())
	{
		return false;
	}
	offset = iter->second.current_block_index_ * block_size_pre_transfer_;
	if (iter->second.current_block_index_ == (iter->second.file_total_block_ - 1)){
		//最后一片，计算大小
		read_size = iter->second.file_total_size_ - (uint64_t)iter->second.current_block_index_ * (uint64_t)block_size_pre_transfer_;
		iter->second.is_finished_ = true;
	}
	else{
		read_size = block_size_pre_transfer_;
	}
	iter->second.current_block_index_++;
	return true;
}

bool sender_pull_long_task::is_file_complete(const uint64_t pull_id){
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	auto iter = map_transfer_.find(pull_id);
	if (iter == map_transfer_.end())
	{
		return false;
	}
	return iter->second.is_finished_;
}

bool sender_pull_long_task::get_file_size(const uint64_t pull_id, uint64_t& result)
{
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	auto iter = map_transfer_.find(pull_id);
	if (iter == map_transfer_.end())
	{
		return false;
	}
	result = iter->second.file_total_size_;
	return true;
}

bool sender_pull_long_task::remove_file_id(const uint64_t pull_id)
{
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	auto iter = map_transfer_.find(pull_id);
	if (iter == map_transfer_.end())
	{
		return false;
	}
	map_transfer_.erase(iter);
	return true;
}