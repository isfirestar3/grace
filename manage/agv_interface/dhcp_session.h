#pragma once

#include "application_network_framwork.hpp"
#include "mntypes.h"
#include "asio_manage.h"
#include "agv_shell_common.h"
#include "proto_agv_msg.h"
#include "agv_shell_define.h"
#include "task_scheduler.hpp"
#include "dhcp_notify.hpp"
#include "compiler.h"

struct mac_info_list : motion::asio_data
{
	std::vector<mac_address_info> vct_mac;
};

typedef void(STDCALL* notify_callback)();

static
notify_callback dhcp_notification_;

class mac_session :public nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>
{
public:
	mac_session();
	mac_session(HTCPLINK link);
	~mac_session();

public:
	int try_connect(const nsp::tcpip::endpoint& ep);
	int post_mac_info_req(const std::shared_ptr<motion::asio_block>&asio_ack);
	int get_network_status(){ return net_status_; }
	void set_dhcp_callback(void(STDCALL* func)(), const std::shared_ptr<nsp::toolkit::task_thread_pool<dhcp_notify>>& ptr);

protected:
	virtual void on_disconnected(const HTCPLINK previous) override final;
	virtual void on_recvdata(const std::string &pkt) override final;
	virtual void on_connected()override final;
	
private:
	int dispatch_package(const uint8_t package_type, const std::string &pkt_data);
	int recv_mac_info_rsp(const std::shared_ptr<nsp::proto::proto_mac_vec>& data);
	int recv_mac_notification();

private:
	std::atomic<mn::net_status_t> net_status_{ mn::kNetworkStatus_Closed }; // sessionÍøÂç×´Ì¬
	std::shared_ptr<nsp::toolkit::task_thread_pool<dhcp_notify>> dhcp_notify_spool_ = nullptr;
};