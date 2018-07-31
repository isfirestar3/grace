#pragma once

#include "application_network_framwork.hpp"
#include "network_handler.h"
#include "asio_manage.h"
#include "agv_shell_define.h"
#include "proto_agv_msg.h"
#include "mntypes.h"
#include "agv_shell_common.h"

struct process_list_info : motion::asio_data
{
	std::vector<process_info> vct_process_;
};

class agv_shell_session :public nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>
{
public:
	agv_shell_session();
	agv_shell_session(HTCPLINK link);
	~agv_shell_session();

public:
	void register_status_callback(void(STDCALL *notify_callback)(int32_t));
	int try_connect(const nsp::tcpip::endpoint& ep);
	int post_get_process_list(const std::shared_ptr<motion::asio_block>&asio_ack);
	int post_agv_shell_process_cmd(const int process_id_all, const std::vector<std::string>& vct_parmanet, const command_agv_shell cmd);
	void get_agv_shell_process_status(std::vector<process_status>& vct_p);
	int is_network_conntected();
	int get_alive_count(){ return alive_count_; }
	void add_alive_count(){ alive_count_++; }
	void clear_process_list();

	uint32_t get_link(){ return this->lnk_; }
	nsp::tcpip::endpoint get_target_endpoint(){ return target_ep_; }

protected:
	virtual void on_disconnected(const HTCPLINK previous) override final;
	virtual void on_recvdata(const std::string &pkt) override final;
	virtual void on_connected()override final;

private:
	int dispatch_package(const uint8_t package_type, const std::string &pkt_data);

	int recv_process_list(const std::shared_ptr<nsp::proto::proto_process_list_reponse>& data, const std::string& pkt_data);
	int recv_process_status(const std::shared_ptr<nsp::proto::proto_process_status>&data);

private:
	std::atomic<mn::net_status_t> net_status_{ mn::kNetworkStatus_Closed }; // session网络状态

	nsp::tcpip::endpoint target_ep_;
	uint32_t link_;

	//进程信息锁
	std::recursive_mutex process_mutex_;
	//进程列表清单
	process_list_info process_list_;
	//进程状态列表
	std::vector<process_status> vct_process_status_;
	//心跳包计数
	std::atomic<int> alive_count_{ 0 };
};
