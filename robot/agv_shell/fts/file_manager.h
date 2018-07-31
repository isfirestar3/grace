#ifndef __AGV_SHELL_FILE_MANAGER_H__
#define __AGV_SHELL_FILE_MANAGER_H__

#include "data_type.h"
#include "file_read_headler.h"
#include "file_rw_task_thread.h"
#include "file_write_handler.h"
#include "singleton.hpp"
#include <map>
#include <mutex>

class file_manager{
	file_manager();
	~file_manager();
	friend class nsp::toolkit::singleton<file_manager>;

public:
	//创建文件 
	int create_files(const uint64_t link, file::st_file_head& f_head, int& error_code);
	//读取文件信息 
	int read_file_info(const uint32_t link, file::st_file_head& f_head, int&error_code);
	//读取指定块数据流
	int read_file_block_stream(const uint32_t link, const int pkt_id, const uint64_t file_id, 
		const uint32_t block_num, const uint64_t offset, const uint32_t read_size);
	//写文件流 
	int write_file_block_stream(const uint32_t link, const int pkt_id, const uint64_t file_id, 
		const uint32_t block_num, const uint64_t offset, const std::string& stream);
	//比较源文件大小与目标文件大小是否一致，如果不一致，则删除目标文件
	int file_complete(const uint32_t link, const uint64_t file_id);
	//删除文件 
	int delete_file(const std::string& file_path);
	//关闭连接下某个文件句柄 
	int close_rw_file_handler(const uint32_t link, const uint64_t file_id);
	//删除连接下所有读写文件句柄 
	void close_rw_file_handler(const uint32_t link);
	//删除读写线程中队列数据 
	void clear_rw_task_deque(const uint32_t link);
	
	//创建目录 
	int create_directory(const std::string& dir, int & error_code);
	//重置文件时间属性 
	int reset_file_time(const std::string& file_name, const uint64_t& create_time,
		const uint64_t& modify_time, const uint64_t& access_time);
		
	void check_file_timeout();
	
	inline void set_file_dir(std::string& dir) {
		file_dir = dir;
	}

	//设置升级状态(状态值加1)，状态值非零期间不允许再执行升级动作 
	void set_upgrade();
	//释放升级状态(状态值减1)，状态为零时允许升级动作 
	void release_upgrade();
private:
	void initlization();
	
	//保存关联的link, file id, 文件信息
	void inert_head_t_map(const uint32_t link, const uint64_t file_id, file::st_file_head&);
	//根据 link+file_id 获取文件head 结构指针 
	int get_head_f_map(const uint32_t link, const uint64_t file_id, file::st_file_head**);
	//格式化之后创建文件夹 
	int recreate_file_path(const std::string& file_name);
private:
	//升级状态 0：非升级状态 !0：升级状态(升级文件正在传输，升级文件正在解压缩等动作正在执行)
	std::atomic<int> is_upgrade_{ 0 };
    std::map<uint32_t, std::shared_ptr<file_rw_task_thread>> map_rw_task_;//key:id, value:file_rw_task_thread
	
	std::recursive_mutex rw_mutex_;
	std::map<uint32_t,std::map<uint64_t, file::st_file_head>> map_file_head_;//key:link, <key：file id 的集合,  value:文件信息 >
	
	//创建文件夹锁
	std::recursive_mutex folder_mutex_;
	//default store file directory
	std::string file_dir;
	
	file_read_headler read_handler_;
	file_write_handler write_handle_;
};

#endif  //__AGV_SHELL_FILE_MANAGER_H__
