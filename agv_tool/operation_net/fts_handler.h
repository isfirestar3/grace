#pragma once

#include <string>
#include <vector>
#include "singleton.hpp"
#include "gzfts_api.h"
#include <functional>
#include "file_read.h"
#include "fts_push_task_group.h"
#include "os_util.hpp"
#include "toolkit.h"
#include <map>
#include "frimware_core_task.h"
#include "fts_pull_task_group.h"

#include "impls.h"

struct task_file_info{
	std::string local_file_path_;
	std::string remote_file_path_;
	std::string modify_datetime_;
	uint64_t file_size_;
	uint32_t status_ = 0;
	uint32_t step_ = 0;
	uint32_t error_ = 0;
};

struct task_info{
	std::string ip_;	//远端ip
	uint16_t port_;		//远端port端口
	uint32_t status_ = file_status::transmitting;	//当前任务组状态
	uint32_t step_ = 0;		//当前文件完成比率

	std::vector<task_file_info> vct_file;	//任务组中数据更新信息
};

typedef void(__stdcall* pull_callback) (const int status, const int steps);
typedef void(__stdcall* push_callback)(const char* ctr, const int cb);

void set_fts_type(const Fts_Function_Type ty);
void set_backup_file(const std::string& path);
//字符转换函数
void convert_positive(std::string& str, const char preview_cr, const  char new_cr);

class fts_handler {
	fts_handler();
	~fts_handler();
	friend class nsp::toolkit::singleton<fts_handler>;

public:
	//获取指定目录结构
	int get_listdir_all(const char* ipremote, const uint16_t port, const std::string& relative_folder, void** output_stream, int& output_cb);
	int get_current_catalog(const char* ip, const uint16_t port, const char* input_catalog, char** output_data, int* cb);
	int push_files_to_remote(const std::vector<nsp::tcpip::endpoint> vct_ep, const char* str_files, const int cb);
	int get_file_crc(const char* input_file, unsigned int* crc_value);
	int set_pull_callback(pull_callback func);
	int set_push_callback(push_callback func);
	int delete_remote_files(const char* ip, uint16_t port, const char* str_files, const int cb);

	//回调函数回调类内
	int update_user_data(const char *ipaddr, uint16_t port,
		const char *path_local, const char *path_remote,
		int status, int error, int step,
		void *user_data, int user_data_size);

	int get_push_files_detail(const uint64_t task_id, void** data_ptr, int& cb);

	int repeate_push_files(const uint64_t, const char* files, int cb);

	//更新任务组状态为正在正在传输
	void update_task_group_status(const uint64_t, const int status);

	//恢复出厂设置
	void restore_factory_setting(const std::vector<std::string> vct_ip);
	//注册恢复出厂设置回调函数
	void regiset_restore_setting(void(__stdcall *notify_callback)(int32_t step, int32_t status, const char* ip));
	//拉取指定文件
	int pull_files_from_remote(const char* ip, const uint16_t port, const char* str_files, const int cb, const char* save_path);
	void update_folder_config();
	void set_fts_callback(const transfer_block_size block_t);

	//如果远端程序崩溃，断开连接，则将map容器内任务置为失败，并在任务组中删除该任务
	void update_disconnect_status(const std::string& ip, const uint16_t port);
	//取消pull操作
	int cancel_pull_opreat();
	//pushM核文件，相机文件
	int push_m_camera_file(const std::vector<nsp::tcpip::endpoint>& vct_ep, const FIRMWARE_SOFTWARE_TYPE ty, 
		const std::string& file_path, const std::string& target_ep);
	void pull_m_camera_file(const std::string& remote_ip, const std::string& remote_file, const std::string& local_path);
	//push agv_shell文件
	int push_agv_file(const std::string ipv4, const std::string& file_path);

	//自主驱动固件文件
	int post_can_custom_file(const std::vector<nsp::tcpip::endpoint>& vct_ep, const uint32_t node_id,const uint32_t serial_type,
		const std::string& file_path, const std::string& target_ep);

	//获取需要备份的文件列表
	std::vector<std::string> get_backup_files() { return backup_files_; }
	void pull_backup_files(const std::string& remote_ip, const std::string& remote_file, const std::string& local_file);

	//减少frimware_core_task队列
	void reduce_core_task_count();

private:
	void initlization();
	void read_config_file();
	void uninit();
	void thread_timeout_check();
	
	//处理文件结构,组合结构树
	void handler_catalog(const std::string&data_stream, std::vector<std::string>& vct_folder);
	void handler_catalog(const std::string&data_stream, std::vector<std::string>& vct_files, std::vector<std::string>& vct_folder);
	int recreate_file_path(const std::string& file_name);
	
	//查询当前发送的目标ip是否在正在发送的队列中
	int query_task_transfer(const std::string& ip, const uint16_t port);
	void handler_push_task(const std::string& local_file, const file_type, const std::string& remote_file, 
		std::vector<push_file_info>& vct_push, task_info& task_i);

private:
	fts_parameter fts_par_;

	std::string root_path_;
	std::string local_standard_file_setting_;//本地xml配置文件中标准文件路径信息
	std::string local_remote_file_setting_;//本地xml配置文件中远端缓存文件路径信息

	//文件读取
	file_read f_read_;
	//pull操作回调函数
	pull_callback pull_callback_function_ = nullptr;
	//push操作回调函数
	push_callback push_callback_function_ = nullptr;

	//push任务组
	fts_push_task_group push_task_group_;
	std::recursive_mutex re_mutex_;
	//push任务组map管理容器
	std::map<uint64_t, task_info> map_task_info_;
	//周期性上报当前map管理容器中数据
	std::thread* timeout_check_ = nullptr;
	nsp::os::waitable_handle timer_wait_;

	//恢复出厂设置回调
	std::function<void(int32_t , int32_t , const char* )> restore_callback_;
	std::atomic<bool> is_pull_exist_ = { false };

	//M core发送文件任务类
	frimware_core_task f_core_task_;
	fts_pull_task_group frimware_pull_task_; //用于拉取M核文件任务类

	std::vector<std::string> backup_files_;//需要备份的文件
};