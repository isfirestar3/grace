#ifndef __AGV_SHELL_SESSION_H__
#define __AGV_SHELL_SESSION_H__
#include "application_network_framwork.hpp"
#include <thread>
#include "os_util.hpp"
#include "proto_agv_msg.h"
#include "proto_frimware_msg.h"
#include "proto_udp_typedef.h"

#include "proto_msg.h"
#include "data_type.h"

class agv_shell_session : public nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol> {
public:
    agv_shell_session();
    ~agv_shell_session();
    agv_shell_session(HTCPLINK lnk);

public:
	int post_vcu_info(const FIRMWARE_SOFTWARE_TYPE f_type, const FRIMWARE_STATUS status, const VCU_MESSAGE msg_ty, const std::string& msg);
	int post_frimware_update(const FIRMWARE_SOFTWARE_TYPE f_type, const FRIMWARE_STATUS status,const int step);
	int post_frimware_download_ack(const std::string& file_path);

	int post_keepalive_status_ack(const int proto_type, const int status);
	int post_process_list_info(const std::shared_ptr<nsp::proto::proto_msg_int>& data);

	int get_alive_count(){ return alive_count_; }
	void add_alive_count(){ alive_count_++; }

	int post_shell_version(uint32_t id);
	int post_config_version(uint32_t id, const std::string& msg);

protected:
    virtual void on_recvdata(const std::string &pkt) override final;
    virtual void on_disconnected(const HTCPLINK previous) override final;
	virtual void on_connected() override final;

public:
	virtual int on_established() override final;
	inline uint32_t get_link() { return this->lnk_; }
	nsp::tcpip::endpoint get_remote_endpoint(){ return remote_; }
	
	//task transform new thread
	void recv_dispatch(const unsigned char *buffer, int cb);

private:
	void client_login(const std::shared_ptr<nsp::proto::proto_login_agv_t>&);
	int on_keepalive_ack(const unsigned char *buffer, int cb);
	int on_get_fix_sysinfo(const unsigned char *buffer, int cb);

	void on_deal_process_cmd(const std::shared_ptr<nsp::proto::proto_command_process>& p_info);
	void begin_upgrade(const std::shared_ptr<nsp::proto::proto_msg_str>& p_f);

	void on_frimware_update(const std::shared_ptr<nsp::proto::proto_frimware_update>& p_f);
	void on_frimware_info(const std::shared_ptr<nsp::proto::proto_frimware_info_request>& p_info);
	void on_frimware_download(const std::shared_ptr<nsp::proto::proto_frimware_download_request>& p_f);
	void on_frimware_restart(const std::shared_ptr<nsp::proto::proto_frimware_info_request>& p_f);
	void on_set_vcu_keep_alive_status(const std::shared_ptr<nsp::proto::proto_set_keepalive_status>& p_f);

	void on_process_table();
	void on_update_ntp(const std::shared_ptr<nsp::proto::proto_msg>& data);
	void on_run_script(const std::shared_ptr<nsp::proto::proto_msg>& data);
	int on_cancel_get_file_task(const std::shared_ptr<nsp::proto::proto_msg_int>& data);
	int on_update_cmd_list(const std::shared_ptr<nsp::proto::proto_process_list_reponse>& data);
	int on_post_log_type_list(const unsigned char *buffer, int cb);
	int on_post_log_info(const std::shared_ptr<nsp::proto::proto_log_condition>&data);
	int on_modify_file_mutex(const std::shared_ptr<nsp::proto::proto_msg_int_sync>& data);

	//can协议自主驱动升级
	void on_can_custom_update(const std::shared_ptr<nsp::proto::proto_frimware_custom_update>& p_f);
	void on_can_custom_info(const std::shared_ptr<nsp::proto::proto_frimware_custom_update>& data);

	int on_backup_files(const std::shared_ptr<nsp::proto::proto_logs_file_path>& data);
	void on_backupdemarcate(const std::shared_ptr<nsp::proto::proto_common_stream>& data);
	
private:
	inline int is_login() { return login;};
	int search_file(
		std::string strPath, std::string start_time, std::string end_time, int task_id,
		std::function<void(const std::string &, const std::string &)> call_back_push_path);
	
	//file transform interface
	void recv_read_file_head(const std::shared_ptr<file::proto::proto_request_file_head_t>& data);
	void recv_push_file_head(const std::shared_ptr<file::proto::proto_file_head_info_t>& data);
	void recv_read_file_block(const std::shared_ptr<file::proto::proto_request_file_data_t>& data);
	void recv_push_file_block(const std::shared_ptr<file::proto::proto_file_data_t>& data);
	void recv_file_status(const std::shared_ptr<file::proto::proto_file_status_t>& data);

	// PKTTYPE_AGV_SHELL_BACKUP_FILES & PKTTYPE_AGV_SHELL_GET_LOG_FILE_NAME
	void set_get_file_task(unsigned int task_id, std::string &file_name);
	int exist_get_file_task(unsigned int task_id);
	int release_get_file_task(unsigned int task_id, std::string &file_name);

private:
	int login { 0 };
	std::mutex get_file_task_locker_;
	std::map<unsigned int, std::string> get_file_task_map_; // map<tassk_id, file_name>
	std::atomic<int> alive_count_{ 0 };
	
	//每一片默认的传输的数据块大小
	uint32_t block_size_pre_transfer_;

	//std::shared_ptr<sender_pull_long_task> pull_task_ = nullptr;
	//login key
	unsigned char key_[PROTO_SESSION_KEY_LENGTH];
};

#endif

