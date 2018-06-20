#ifndef __AGV_SHELL_SESSION_H__
#define __AGV_SHELL_SESSION_H__
#include "application_network_framwork.hpp"
#include <thread>
#include "os_util.hpp"
#include "proto_agv_msg.h"
#include "proto_frimware_msg.h"
#include "proto_udp_typedef.h"
#include "proto_agv_msg.h"

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
	int post_file_status(const int status);

	int get_alive_count(){ return alive_count_; }
	void add_alive_count(){ alive_count_++; }

	int post_shell_version(const std::string &msg);
	int post_tar_backups(const std::string& msg);
	int post_config_version(const std::string& msg);

protected:
    virtual void on_recvdata(const std::string &pkt) override final;
    virtual void on_disconnected(const HTCPLINK previous) override final;
	virtual void on_connected() override final;

public:
	uint32_t get_link();
    virtual int on_established() override final;
	nsp::tcpip::endpoint get_remote_endpoint(){ return remote_; }

private:
	void post_login_ack();

	int on_get_hostname();
	void on_deal_process_cmd(const std::shared_ptr<nsp::proto::proto_command_process>& p_info);
	void begin_upgrade(const std::shared_ptr<nsp::proto::proto_msg_str>& p_f);

	void on_get_login(const std::shared_ptr<nsp::proto::proto_login_agv>& p_f);
	void on_frimware_update(const std::shared_ptr<nsp::proto::proto_frimware_update>& p_f);
	void on_frimware_info(const std::shared_ptr<nsp::proto::proto_frimware_info_request>& p_info);
	void on_frimware_download(const std::shared_ptr<nsp::proto::proto_frimware_download_request>& p_f);
	void on_frimware_restart(const std::shared_ptr<nsp::proto::proto_frimware_info_request>& p_f);
	void on_query_vcu_keep_alive_status();
	void on_set_vcu_keep_alive_status(const std::shared_ptr<nsp::proto::proto_set_keepalive_status>& p_f);

	void on_process_table();
	void on_update_ntp(const std::shared_ptr<nsp::proto::proto_msg>& data);
	void on_run_script(const std::shared_ptr<nsp::proto::proto_msg>& data);
	int on_cmd_list(const std::shared_ptr<nsp::proto::proto_msg_int>& data);
	int on_update_cmd_list(const std::shared_ptr<nsp::proto::proto_process_list_reponse>& data);
	int on_post_log_type_list();
	int on_post_log_info(const std::shared_ptr<nsp::proto::proto_log_condition>&data);
	int on_modify_file_mutex(const std::shared_ptr<nsp::proto::proto_msg_int_sync>& data);

	//can协议自主驱动升级
	void on_can_custom_update(const std::shared_ptr<nsp::proto::proto_frimware_custom_update>& p_f);
	void on_can_custom_info(const std::shared_ptr<nsp::proto::proto_frimware_custom_update>& data);

	int on_backup_files(const std::shared_ptr<nsp::proto::proto_logs_file_path>& data);
	void on_backupdemarcate(const std::shared_ptr<nsp::proto::proto_common_stream>& data);

private:
	void search_file(std::string strPath, std::string start_time, std::string end_time, std::function<void(const std::string &, const std::string &)> call_back_push_path);
private:
	int logs_cancel_flag_;
	std::atomic<int> alive_count_{ 0 };
};

#endif

