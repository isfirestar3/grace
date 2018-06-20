#pragma once

#include "application_network_framwork.hpp"
#include "os_util.hpp"
#include "agv_shell_define.h"
#include "impls.h"
#include "proto_frimware_msg.h"
#include "proto_agv_msg.h"
#include "net_manager.h"

typedef enum
{
	KNetwork_Disconnect = -2,	//断开连接状态
	kNetwork_Closed,
	kNetwork_Active = 0,		
	kNetwork_Connecting,		//当前正在连接
	KNetwork_Connected,			//当前连接已经连接成功，但需要登录后才能使用
	KNetwork_Established		//当前连接已经可以使用
}net_status_t;

class operation_session:public nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>
{
public:
	operation_session();
	operation_session(HTCPLINK link);
	~operation_session();

protected:
	virtual void on_disconnected(const HTCPLINK previous) override final;
	virtual void on_recvdata(const std::string &pkt) override final;
	virtual void on_connected()override final;

private:
	nsp::tcpip::endpoint		ep_;
	std::atomic<net_status_t>	net_status_{ kNetwork_Closed };
	uint32_t                    identity_id_;//session标识id
	std::function<void(int32_t)> disconnect_callback_=nullptr;
	std::function<void(int32_t, int32_t, const char* host_name)> connect_callback_ = nullptr; //参数一：robot_id;参数二：状态;参数三：主机名

	//进程列表清单
	std::map<int, std::string> map_process_list_;

	std::atomic<uint64_t> refresh_tick_{ 0 };//记录当前时间点

	//保存当前需要备份的文件本地保存地址
	std::string local_backups_path_;
	//保存当前M core
	std::string local_m_coer_path_;

public:
	void set_identity_id(uint32_t id);

public:
	void register_disconnect_callback(void( __stdcall *notify_callback)(int32_t));
	void register_connect_callback(void(__stdcall *notify_callback)(int32_t, int32_t, const char* host_name));

	int sync_connect(const nsp::tcpip::endpoint &ep);
	int post_agv_cmd(const int process_id_all, const std::vector<std::string>& vct_parament, const int cmd);
	int post_frimware_info_request(const FIRMWARE_SOFTWARE_TYPE frimware_type, const std::string& ip_port);
	int post_m_core_transfer_request(const FIRMWARE_SOFTWARE_TYPE frimware_type, const std::string& file_path, const std::string& target_ep);
	int post_pull_m_core_request(const FIRMWARE_SOFTWARE_TYPE frimware_type, const int frimware_length,
		const std::string& target_ep, const std::string& local_path);
	void post_agv_shell(const std::string& remote_path, const uint32_t fize);
	int post_restart_m_camera_request(const FIRMWARE_SOFTWARE_TYPE frimware_type, const std::string& target_ep);
	int post_get_agv_process_table(const std::shared_ptr<motion::asio_block>&asio_ack);

	int post_can_custom_info_request(const std::string& ip_port, const int node_id, const int serial_type);
	int post_can_custom_transfer_request(const int node_id, const int serial_type, const std::string& file_path, const std::string& target_ep);

	//通用同步发包操作
	int post_package_sync(const int type, const void* str, const int cb, void **ack_msg, int *ack_len);
	//通用异步发包操作
	int post_package_async(const int type, const void* str, const int cb);

	nsp::tcpip::endpoint get_target_endpoint() { return ep_; }
	void set_target_endpoint(const nsp::tcpip::endpoint& ep) { ep_ = ep; }

	void post_keepalive();
	net_status_t get_network_status() { return net_status_; }

	uint64_t get_interval();

	int post_backup_files(const std::vector<std::string>& vct_files);
	void set_local_backups_path(const uint32_t id, const std::string& path);

private:

	int dispatch_package(const uint8_t package_type, const std::string &pkt_data);
	int post_login();
	void on_recv_dispath(const unsigned char* data, int & cb);

	int recv_get_hostname_ack(const std::shared_ptr<nsp::proto::proto_msg>& data);
	int recv_cmd_process_ack(const std::shared_ptr<nsp::proto::proto_msg_int>& data);
	int recv_frimware_info_ack(const std::shared_ptr<nsp::proto::proto_frimware_info_response>&data);
	int recv_frimware_transfer_ack(const std::shared_ptr<nsp::proto::proto_frimware_update_response>& data);
	int recv_frimware_download_ack(const std::shared_ptr<nsp::proto::proto_frimware_download_reponse>& data);
	int recv_process_status(const std::shared_ptr<nsp::proto::proto_process_status>&data);
	int recv_process_list(const std::shared_ptr<nsp::proto::proto_process_list_reponse>& data, const std::string& pkt_data);
	int recv_login_ack(const std::shared_ptr<nsp::proto::proto_msg_int>& data);
	int recv_cmd_list(const std::shared_ptr<nsp::proto::proto_process_list_reponse>& data, const std::string & pkt_msg);
	int recv_update_cmd_ack(const std::shared_ptr<nsp::proto::proto_common_stream>&data);
	int recv_file_mutex_status(const std::shared_ptr<nsp::proto::proto_keepalive_status_reponse>&data);
	int recv_modify_mutex(const std::shared_ptr<nsp::proto::proto_msg_int_sync>&data);
	int recv_shell_version_info(const std::shared_ptr<nsp::proto::proto_shell_version_t>&data);
	int recv_backups_files_ack(const std::shared_ptr<nsp::proto::proto_msg>& data);
	int recv_config_version_info(const std::shared_ptr<nsp::proto::proto_shell_version_t>&data);

	int post_request_sync(const uint32_t type, const unsigned char* str, const int cb, unsigned char** reponse, int * out_cb);
	template<class T>
	int post_request_async(const uint32_t type,const unsigned char* str,const int cb);

	bool is_demarcate_file(const unsigned char* str, const int cb);
};

