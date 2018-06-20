#include "file_rw_task_thread.h"
#include "file_read_handler.h"

#include "receiver_manager.h"
#include "sender_manager.h"

#include "long_lnk_receiver.h"
#include "long_lnk_sender.h"


file_rw_task_thread::file_rw_task_thread(){
	try{
		if (!write_thread_){
			write_thread_ = new std::thread(std::bind(&file_rw_task_thread::write_run, this));
		}
		if (!read_thread_){
			read_thread_ = new std::thread(std::bind(&file_rw_task_thread::read_run, this));
		}
	}
	catch (...){

	}
}

file_rw_task_thread::~file_rw_task_thread(){
	if (write_thread_){
		stop_write_thread();
		if (write_thread_->joinable()){
			write_thread_->join();
		}
		delete write_thread_;
		write_thread_ = nullptr;
	}

	{
		std::lock_guard<decltype(write_mutex_)> lock(write_mutex_);
		write_task_deque_.clear();
	}

	if (read_thread_){
		stop_read_thread();
		if (read_thread_->joinable()){
			read_thread_->join();
		}
		delete read_thread_;
		read_thread_ = nullptr;
	}

	{
		std::lock_guard<decltype(read_mutex_)> lock(read_mutex_);
		read_task_deque_.clear();
	}
}

void file_rw_task_thread::add_write_task(void* handler, const uint32_t link, const nsp::file::file_block& f_data,
	const nsp::file::current_identify& identify, const bool is_long_lnk, const uint64_t f_id){
	write_task_info w_info;
	w_info.handler_ = handler;
	w_info.file_block_info_ = f_data;
	w_info.identify_ = identify;
	w_info.link_ = link;
	w_info.file_id = f_id;
	w_info.is_long_lnk = is_long_lnk;
	{
		std::lock_guard<decltype(write_mutex_)> lock(write_mutex_);
		write_task_deque_.push_back(w_info);
	}
	write_wait_.sig();
}

void file_rw_task_thread::add_read_task(void* handler, const uint32_t pkt_id, const uint32_t link, const uint64_t offset,
	const uint32_t read_size, const nsp::file::current_identify identify, 
	const bool is_long_lnk, const uint64_t f_id){
	read_task_info r_info;
	r_info.handler_ = handler;
	r_info.identify_ = identify;
	r_info.link_ = link;
	r_info.offset_ = offset;
	r_info.pkt_id_ = pkt_id;
	r_info.is_long_lnk = is_long_lnk;
	r_info.file_id = f_id;
	r_info.read_size_ = read_size;
	{
		std::lock_guard<decltype(read_mutex_)> lock(read_mutex_);
		read_task_deque_.push_back(r_info);
		
	}
	read_wait_.sig();
}

void file_rw_task_thread::stop_write_thread(){
	exit_write_thread_ = 1;
	write_wait_.sig();
}

void file_rw_task_thread::stop_read_thread(){
	exit_read_thread_ = 1;
	read_wait_.sig();
}

void file_rw_task_thread::clear_rw_deque(uint32_t link){
	{
		std::lock_guard<decltype(write_mutex_)> lock(write_mutex_);
		auto iter = write_task_deque_.begin();
		while (iter != write_task_deque_.end()){
			if (link == iter->link_){
				iter = write_task_deque_.erase(iter);
			}
			else{
				iter++;
			}
		}
	}
}

void file_rw_task_thread::write_run(){
	file_write_handler write_handler;

	while (true){
		if (0 == write_wait_.wait(-1));
		if (exit_write_thread_ == 1)break;

		write_task_info w_info;
		while (true){
			{
				std::lock_guard<decltype(write_mutex_)> lock(write_mutex_);
				if (write_task_deque_.size() == 0) break;
				w_info = write_task_deque_.front();
				write_task_deque_.pop_front();
			}

			int res = write_handler.write_file_block(w_info.handler_, w_info.file_block_info_.offset, w_info.file_block_info_.stream);
			if (res < 0)
			{
				int error_code;
#ifdef _WIN32
				error_code = GetLastError();

#else
				error_code = errno;
#endif
				loerror("fts") << "failed to write file block to disk,the error code is:" << error_code;
				//通知上层
				switch (w_info.identify_)
				{
				case nsp::file::current_identify::pull_identify:
					//pull本地写文件失败，通知sender_manager
					if (!w_info.is_long_lnk)
					{
						nsp::toolkit::singleton<sender_manager>::instance()->fts_pull_write_file_error(w_info.link_, nsp::proto::file_status::write_error, error_code);
					}
					else
					{
						nsp::toolkit::singleton<long_lnk_sender>::instance()->fts_pull_write_file_error(w_info.link_, w_info.file_id,
							nsp::proto::file_status::write_error, error_code);
					}
					break;
				case nsp::file::current_identify::push_identify:
					//push本地写文件失败，通知receiver_manager
					nsp::toolkit::singleton<receiver_manager>::instance()->fts_push_write_file_error(w_info.link_, nsp::proto::file_status::write_error, error_code);
					break;
				default:
					break;
				}
				//返回等待下一次响应
				continue;
			}

			//发送获取下一片数据请求
			switch (w_info.identify_)
			{
			case nsp::file::current_identify::pull_identify:
				//pull获取下一片数据，通知sender端
				if (!w_info.is_long_lnk)
				{
					nsp::toolkit::singleton<sender_manager>::instance()->fts_pull_next_block(w_info.link_, w_info.file_block_info_.offset + w_info.file_block_info_.stream.size());
				}
				else
				{
					nsp::toolkit::singleton<long_lnk_sender>::instance()->fts_pull_next_block(w_info.link_, w_info.file_id, w_info.file_block_info_.offset + w_info.file_block_info_.stream.size());
				}
				break;
			case nsp::file::current_identify::push_identify:
				//push获取下一片数据，通知receive端
				nsp::toolkit::singleton<receiver_manager>::instance()->fts_push_next_block(w_info.link_, w_info.file_block_info_.offset + w_info.file_block_info_.stream.size());
				break;
			default:
				break;
			}
		}
		
	}
}

void file_rw_task_thread::read_run(){
	file_read_headler read_handler;
	while (true){
		if (0 == read_wait_.wait(-1));
		if (exit_read_thread_ == 1) break;

		read_task_info r_info;
		while (true){
			
			{
				std::lock_guard<decltype(read_mutex_)> lock(read_mutex_);
				if (read_task_deque_.size() == 0) break;
				r_info = read_task_deque_.front();
			}

			std::string data;
			int res = read_handler.read_file_stream(r_info.handler_, r_info.offset_, r_info.read_size_, data);
			if (res < 0){
				int error_code;
#ifdef _WIN32
				error_code = GetLastError();

#else
				error_code = errno;
#endif
				loerror("fts") << "failed to read file block from disk,the error code is:" << error_code;

				//通知上一层
				switch (r_info.identify_)
				{
				case nsp::file::current_identify::pull_identify:
					//pull操作时，read文件失败,通知receive端
					if (!r_info.is_long_lnk)
					{
						nsp::toolkit::singleton<receiver_manager>::instance()->fts_pull_read_file_error(r_info.link_, nsp::proto::file_status::read_error,
							error_code);
					}
					else
					{
						nsp::toolkit::singleton<long_lnk_receiver>::instance()->fts_pull_read_file_error(r_info.link_, r_info.file_id, 
							nsp::proto::file_status::read_error, error_code);
					}
					break;
				case nsp::file::current_identify::push_identify:
					//push操作时，read文件失败,通知sender端
					nsp::toolkit::singleton<sender_manager>::instance()->fts_push_read_file_error(r_info.link_, nsp::proto::file_status::read_error,
						error_code);
					break;
				default:
					break;
				}
				break;
			}

			switch (r_info.identify_)
			{
			case nsp::file::current_identify::pull_identify:
				if (!r_info.is_long_lnk)
				{
					//通知receive 发送数据
					nsp::toolkit::singleton<receiver_manager>::instance()->fts_pull_send_file_block_stream(r_info.pkt_id_, r_info.link_,
						r_info.offset_, data);
				}
				else
				{
					nsp::toolkit::singleton<long_lnk_receiver>::instance()->fts_pull_send_file_block_stream(r_info.pkt_id_, r_info.file_id,
						r_info.link_, r_info.offset_, data);
				}
				break;
			case nsp::file::current_identify::push_identify:
				//通知sender 发送数据
				nsp::toolkit::singleton<sender_manager>::instance()->fts_push_send_file_block_stream(r_info.link_, r_info.offset_, data);
				break;
			default:
				break;
			}

			{
				std::lock_guard<decltype(read_mutex_)> lock(read_mutex_);
				read_task_deque_.pop_front();
			}
		}

	}
}