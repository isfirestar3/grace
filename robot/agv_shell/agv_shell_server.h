#ifndef __AGV_SHELL_SVR_H__
#define __AGV_SHELL_SVR_H__

#include "agv_shell_session.h"
#include "frimware_task.h"
#include "proto_msg.h" //file_status
#include "task.h"
#include "task_scheduler.hpp"
#include "singleton.hpp"
#include "sys_info.h" //sys_info

class agv_shell_server 
{
    agv_shell_server();
    ~agv_shell_server();
	friend class nsp::toolkit::singleton<agv_shell_server>;


public:
	int begin_shell_server(const int port, std::string ip = "0.0.0.0");

	int on_get_frimware_info(const uint32_t link, const int frimware_ty, const std::string& target_ep, const uint32_t node_id = 0, 
		const uint32_t serial_type = 0);
	int post_frimware_info(const uint32_t link, const FIRMWARE_SOFTWARE_TYPE f_type, const FRIMWARE_STATUS status, 
		const VCU_MESSAGE msg_ty, const std::string& msg);

	int on_frimware_update(const uint32_t link, const int f_ty, const std::string& file_path, const std::string& target_ep, 
		const uint32_t node_id = 0, const uint32_t serial_type = 0);
	int post_frimware_update(const uint32_t link, const FIRMWARE_SOFTWARE_TYPE f_type, const FRIMWARE_STATUS status, const int step);

	int on_frimware_download(const uint32_t link, const int frimware_ty, const int frimware_length,const std::string& target_ep);
	int post_frimware_download(const uint32_t link, const std::string& file_path);

	int on_frimware_restart(const uint32_t link, const int frimware_ty, const std::string& target_ep);
	void on_query_vcu_keep_alive_status(const HTCPLINK link, const unsigned char *buffer, int cb);
	void on_set_vcu_keep_alive_status(const HTCPLINK link, uint32_t id, int status);
	int post_vcu_keep_alive_status(const HTCPLINK link, int id, const int proto_type, int status, int err);
	//void on_deal_process_cmd(const HTCPLINK link, int cmd, int process_id); 
	void on_deal_process_cmd(const HTCPLINK link, const std::shared_ptr<nsp::proto::proto_command_process> p_info);
	
	int on_get_sysinfo_fixed(const HTCPLINK link, const unsigned char *buffer, int cb);
	int on_get_sysinfo_changed(const HTCPLINK link, const unsigned char *buffer, int cb);
	int post_pkgTsession_bylink(const HTCPLINK link, const void *buffer, int cb);
	int post_pkgTsession_bylink(const HTCPLINK link, const nsp::proto::proto_interface *package);

	uint16_t get_client_number(){ return vct_lnk_.size(); }
	void add_client_lnk(const uint32_t link);
	void reduce_client_lnk(const uint32_t link);

	void set_workpath(const std::string& path){ work_path_ = path; }
	std::string get_workpath(){ return work_path_; }

	void close_all();
	void post_notify_all(const nsp::proto::proto_interface& package);
	void post_file_mutex(const uint32_t link, const unsigned char *buffer, int cb);

	void post_tar_backups(const uint32_t lnk, int id, const std::string& des_file, int err);
	
	//blank box
	void post_write_file_status(const uint32_t link, const int pkt_id, const uint64_t file_id, 
		const uint32_t block_num, int error_code);

	void post_read_file_status(const uint32_t link, const int pkt_id, const uint64_t file_id, 
		const uint32_t block_num, const uint32_t off, std::string& data, int error_code);

private:
	void initlization();
	void uinit();
	void post_frimware_download_complete(const uint32_t link, const std::string& file_path);

	void alive_check_thread();

private:
    std::shared_ptr<nsp::tcpip::tcp_application_service<agv_shell_session>> service_;

    nsp::tcpip::endpoint ep_;

	//获取M核固件信息
	std::shared_ptr<nsp::toolkit::task_thread_pool<frimware_get_info_task>> frimware_info_spool_ = nullptr;
	//转发升级固件至M核或相机
	std::shared_ptr<nsp::toolkit::task_thread_pool<frimware_update_task>> frimware_push_spool_ = nullptr;
	//下载M核固件文件
	std::shared_ptr<nsp::toolkit::task_thread_pool<frimware_download_task>> frimware_download_spool = nullptr;
	//重启固件线程池
	std::shared_ptr<nsp::toolkit::task_thread_pool<frimware_restart_task>> frimware_restart_spool = nullptr;
	std::shared_ptr<nsp::toolkit::task_thread_pool<base_task>> base_task_spool = nullptr;

	//存储当前配置管理平台客户端的link，要求只允许当前有且只有一个客户端连接上服务端
	std::vector<uint32_t> vct_lnk_;
	std::string work_path_;

	std::thread* alive_check_ = nullptr;
	std::atomic<int> is_exist_{ 0 };
	nsp::os::waitable_handle check_wait_;
	
	sys_info sys_info_;
};


#endif

