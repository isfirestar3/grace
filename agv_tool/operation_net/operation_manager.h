#pragma once
#include <memory>
#include <atomic>
#include "singleton.hpp"
#include "operation_session.h"
#include <thread>
#include "impls.h"
#include "agv_shell_define.h"

#define POST_AGV_CMD_TIMEOUT	2000

struct process_id_releation
{
	uint32_t allocat_id_;
	uint32_t original_id_;
};

class operation_manager
{
private:
	operation_manager();
	~operation_manager();
	friend class nsp::toolkit::singleton<operation_manager>;

public:
	uint32_t alloc_robot_id();
	//网络对象管理
	int  attach_seesion(uint32_t robot_id, std::shared_ptr<operation_session> &pseesion);
	std::shared_ptr<operation_session> find_session(uint32_t robot_id);
	int  remove_seesion(uint32_t robot_id);

	void add_connected_session(uint32_t robot_id);
	void rm_connected_session(uint32_t robot_id);
	std::shared_ptr<operation_session> find_connect_session(const uint32_t robot_id);
	
	bool is_endpoint_exist(const nsp::tcpip::endpoint& ep);
	int post_agv_cmd(const std::vector<std::string>& vct_robot_id, const std::vector<std::string>& vct_process_id,const AgvShellCmd type);

	void set_cmd_result(const int value);
	void set_wait_sig();

	void set_net_status(const std::string& ip, const nsp::proto::errorno_t& status);
	nsp::proto::errorno_t get_net_status(const std::string& ip);

	void set_network_callback(void(__stdcall* func)(const char* robot_ip, const int type, const int error, const char* data_str, const int cb));
	void network_callback_notify(const char* robot_ep, const int type, const int error, const char* data,const int cb);
	void post_get_agv_process_table();
	int get_process_allocat_id(const std::string& ip_port, const int original_id);
	int post_async_package(const int robot_id, const int type, const void* str, const int cb);
	int post_sync_package(const int robot_id, const int type, const void* str, const int cb, void **_ack_msg, int *_ack_len);

	//M核业务
	void get_m_core_info(const std::vector<std::string>& vct_robot, const FIRMWARE_SOFTWARE_TYPE frimware_type,
		const std::string& target_ep, const int node_id, const int can_bus);
	void set_frimware_info_callback(void(__stdcall* func)(const char* ip, const VCU_MESSAGE vcu_type, const char* msg));
	void frimware_info_notify(const char* ip, const VCU_MESSAGE vcu_type, const FRIMWARE_STATUS frimware_status, const char* msg);
	void post_m_core_transfer_request(const char* ip, const FIRMWARE_SOFTWARE_TYPE frimware_type, const std::string& remote_file, 
		const std::string& target_ep);
	void set_frimware_transfer_callback(void(__stdcall* func)(const char* ip, const FRIMWARE_STATUS frimware_type, const int step));
	void frimware_transfer_notify(const char* ip,const FRIMWARE_STATUS status,const int step);
	void post_pull_m_core_request(const std::vector<std::string>& vct_robot, const FIRMWARE_SOFTWARE_TYPE frimware_type, 
		const int frimware_length, const std::string& target_ep,const std::string& local_path);
	void post_restart_m_camera_request(const std::vector<std::string>& vct_robot, const FIRMWARE_SOFTWARE_TYPE frimware_type,
		const std::string& target_ep);

	//自主驱动升级
	void post_custom_transfer_request(const char* ip,const int node_id,const int serial_type,const std::string&remote_file,
		const std::string& target_ep);
	void get_custom_info_request(const std::vector<std::string>& vct_robot, const std::string& target_ep, 
		const int node_id, const int can_bus);

	//agv_shell进程相关业务
	void set_process_update(void(__stdcall* function)(const char* endpoint, ProcessStatus status));
	void update_process_status(const std::string& ep, const int status);
	void update_agv_shell(std::vector<std::string>&robot_id_list, const std::string& agv_path);
	void post_agv_shell(const std::string ipv_4, const std::string& remote_path, const uint32_t fize);

	//备份车上配置文件
	int post_backups_files(const void* str, const int cb);
	void update_files_progress(const std::string& ipv4,const file_status , const int callback_type);

private:
	void init();
	void uinit();
	int get_cmd_result();
	void keepalive();
	uint32_t allocat_process_id(){return process_id_++; }
	void add_process_relation(const std::string ipv4_port,const uint32_t allocat_id,const uint32_t original_id);

private:
	std::atomic<uint32_t> robot_id_{ 0 };
	std::atomic<uint32_t> process_id_{ 0 };
	std::recursive_mutex mutex_net_session_;
	//所有的生成的map集合
	std::map<uint32_t, std::shared_ptr<operation_session>> map_net_session_;//key: robot_id; value:session

	std::recursive_mutex mutex_net_connected_;
	//已连接上的map集合，此处使用两个集合，是因为原有init_net接口中代码未做修改，兼容老的版本代码
	std::map<uint32_t, std::shared_ptr<operation_session>> map_connect_session_;

	//接收到的所有进程集合，key:进程名称，value:重新分配的进程id号
	std::map<std::string, uint32_t> map_process_all_;
	//所有已经连接上的ip的重新分配id号与agv_shell上报的进程id号的关系结构
	//第一层map:key:ip索引第二层:key:重新分配的id号，value:原本得到的agv_shell进程id号
	std::map<std::string, std::vector<process_id_releation>> map_all_process_releation_;
	
	//存储网络链接状态
	std::map<std::string, nsp::proto::errorno_t> map_net_status_;
	std::recursive_mutex mutext_net_status_;

	nsp::os::waitable_handle process_waiter_;
	int cmd_result_;

	std::thread *__th_keepalive = nullptr;				//心跳线程
	nsp::os::waitable_handle __keepalive_stop;
	std::atomic<bool> is_exist_{false};

	std::function<void(const char* ip, const VCU_MESSAGE vcu_type, const char* msg)> frimware_info_;
	std::function<void(const char* ip, const FRIMWARE_STATUS frimware_type, const int step)> frimware_transfer_callback_;
	std::function<void(const char* endpoint, ProcessStatus status)> process_status_;
	std::function<void(const char* ip, const int type, const int error, const char* data_str, const int cb)> network_callback_;

	std::recursive_mutex shell_progress_mutex_;
	std::map<std::string, file_status> map_update_progress_;//存放一键更新/一键备份进度条,key:对端shell ip地址，value:完成进度
};

