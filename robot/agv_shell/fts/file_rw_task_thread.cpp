#include "file_rw_task_thread.h"
#include "agv_shell_server.h"
#include "file_read_headler.h"
#include "file_write_handler.h"

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

void file_rw_task_thread::add_write_task(const uint32_t link, const int pkt_id, const uint32_t block_num, 
			file::st_file_head* file_h, const uint64_t offset, const std::string& stream) {
	file::write_task_info w_info;
	w_info.p_head = file_h;
	w_info.link = link;
	w_info.pkt_id = pkt_id;
	w_info.cur_block_num = block_num;
	w_info.offset = offset;
	w_info.stream = stream;
	{
		std::lock_guard<decltype(write_mutex_)> lock(write_mutex_);
		write_task_deque_.push_back(w_info);
	}
	write_wait_.sig();
}

void file_rw_task_thread::add_read_task(const uint32_t link, const int pkt_id, const uint32_t block_num, 
			file::st_file_head* file_h, uint64_t offset, uint32_t read_size){
	file::read_task_info r_info;
	r_info.p_head = file_h;
	r_info.link = link;
	r_info.pkt_id = pkt_id;
	r_info.cur_block_num = block_num;
	r_info.offset = offset;
	r_info.read_size = read_size;
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
			if (link == iter->link){
				iter = write_task_deque_.erase(iter);
			}
			else{
				iter++;
			}
		}
	}
	{
		std::lock_guard<decltype(read_mutex_)> lock(read_mutex_);
		auto iter = read_task_deque_.begin();
		while (iter != read_task_deque_.end()){
			if (link == iter->link){
				iter = read_task_deque_.erase(iter);
			}
			else{
				iter++;
			}
		}
	}
}

void file_rw_task_thread::write_run(){
	file_write_handler write_handler;
	int error_code;

	while (true){
		if (0 == write_wait_.wait(2000));
		if (exit_write_thread_ == 1)break;

		file::write_task_info w_info;
		while (true){
			{
				std::lock_guard<decltype(write_mutex_)> lock(write_mutex_);
				if (write_task_deque_.size() == 0) break;
				w_info = write_task_deque_.front();
				write_task_deque_.pop_front();
			}

			error_code = 0;
			int res = write_handler.write_file_block(w_info.p_head->hdl_fd, w_info.offset, w_info.stream);
			if (res < 0) {
#ifdef _WIN32
				error_code = GetLastError();
#else
				error_code = errno;
#endif
				loerror("fts") << "failed to write file block to disk,the error code is:" << error_code;
			}

			w_info.p_head->oper_time = nsp::os::gettick();
			//写完成, 回包 
			nsp::toolkit::singleton<agv_shell_server>::instance()->post_write_file_status(w_info.link, w_info.pkt_id, 
						w_info.p_head->file_id, w_info.cur_block_num, error_code);
		}
	}
}

void file_rw_task_thread::read_run(){
	file_read_headler read_handler;
	int error_code;
	
	while (true){
		if (0 == read_wait_.wait(2000));
		if (exit_read_thread_ == 1) break;

		file::read_task_info r_info;
		while (true){
			{
				std::lock_guard<decltype(read_mutex_)> lock(read_mutex_);
				if (read_task_deque_.size() == 0) break;
				r_info = read_task_deque_.front();
				read_task_deque_.pop_front();
			}

			error_code = 0;
			std::string data;
			int res = read_handler.read_file_stream(r_info.p_head->hdl_fd, r_info.offset, r_info.read_size, data);
			if (res < 0){
#ifdef _WIN32
				error_code = GetLastError();
#else
				error_code = errno;
#endif
				loerror("fts") << "failed to read file block from disk,the error code is:" << error_code;
			}

			r_info.p_head->oper_time = nsp::os::gettick();
			//读完成, 回包 
			nsp::toolkit::singleton<agv_shell_server>::instance()->post_read_file_status(r_info.link, r_info.pkt_id, 
						r_info.p_head->file_id, r_info.cur_block_num, r_info.offset, data, error_code);
			
		}
	}
}
