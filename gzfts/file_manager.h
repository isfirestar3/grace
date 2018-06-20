#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "singleton.hpp"
#include "data_type.h"
#include <map>
#include <mutex>
#include <thread>
#include <functional>
#include "os_util.hpp"
#include "task_scheduler.hpp"
#include "file_read_handler.h"
#include "file_write_handler.h"
#include "file_rw_task_thread.h"
#include "file_shell_handler.h"

#define NET_WINDOW_SIZE   2

class file_manager{
	file_manager();
	~file_manager();
	friend class nsp::toolkit::singleton<file_manager>;

public:
	int create_files(const uint64_t link, const nsp::file::file_head& f_head, int & error_code, bool is_long_lnk = false, const uint64_t file_id = 0);
	int read_files(const uint32_t link, const std::string& file_path, nsp::file::file_info& f_info, int&error_code);
	int read_file_block_stream(const uint32_t pkt_id, const uint32_t link, const nsp::file::current_identify, 
		const uint64_t offset, const uint32_t read_size);
	int write_file_block_stream(const uint32_t link, const nsp::file::current_identify,
		const nsp::file::file_block& f_blcok, const bool is_long_lnk = false);
	int delete_file(const std::string& file_path);
	void remove_rw_file_handler(const uint32_t link);
	void remove_fw_long_file_handler(const uint32_t link);

	void clear_rw_task_deque(const uint32_t link);
	void is_file_complete(const std::string& file_path, const uint64_t src_file_size);

	int create_directory(const std::string& dir, int & error_code);
	int reset_file_time(const std::string& file_name, const uint64_t& create_time,
		const uint64_t& modify_time, const uint64_t& access_time);

	//////////////////////////////////以下为长连接业务//////////////////////////////////////
	int read_files_long_lnk(const uint32_t link, const std::string& file_path, nsp::file::file_info& f_info, int&error_code);
	int read_file_block_long_lnk(const uint32_t pkt_id, const uint32_t link, const uint64_t file_id,
		const nsp::file::current_identify, const uint64_t offset, const uint32_t read_size);
	bool close_fts_file_handler(const uint32_t link, const uint64_t file_id);

private:
	void push_file_handler(const uint32_t link, void* handler);
	void initlization();
	int recreate_file_path(const std::string& file_name);
	std::string convert_positive(const std::string& str, const char preview_cr, const  char new_cr);

	void push_fd_handler(const uint32_t link ,const uint64_t file_id,void* handler);

private:
	std::recursive_mutex rw_mutex_;
	//存放link 与 文件句柄
	std::map<uint32_t, void*> map_file_handler_;//key:link value:file handler

	std::map<uint32_t, std::shared_ptr<file_rw_task_thread>> map_rw_task_;

	//创建文件夹锁
	std::recursive_mutex folder_mutex_;

	file_shell_handler lnk_handler_;

	std::map<uint64_t,std::map<uint64_t, void*>> map_fd_;//key:link, key：file id 的集合 ：value:文件操作句柄
};

#endif