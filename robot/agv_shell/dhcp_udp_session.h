#ifndef __GRACE_ROBOT_AGVSHELL_DHCP_UDP_SESSION_H__
#define __GRACE_ROBOT_AGVSHELL_DHCP_UDP_SESSION_H__

#include "asio.hpp"
#include "endpoint.h"
#include "network_handler.h"
#include "proto_agv_msg.h"
#include "proto_udp_common.h"
#include "serialize.hpp"
#include "singleton.hpp"

class dhcp_udp_session :public ::nsp::tcpip::obudp{

public:
	dhcp_udp_session();
	~dhcp_udp_session();
	
public:
	int psend(const nsp::proto::proto_interface& package,const nsp::tcpip::endpoint& ep);

protected:
	virtual void on_recvdata(const std::string &data, const nsp::tcpip::endpoint &r_ep) override final;

private:
	void dispatch_mac_addr();
};

class dhcp_udp_client_manager{
	dhcp_udp_client_manager();
	~dhcp_udp_client_manager();
	friend class nsp::toolkit::singleton<dhcp_udp_client_manager>;

public:
	int init_network();
	int post_local_info_request();
	
	void set_dhcp_fix_ep(const std::string& ipv4, const uint16_t port) {
		dhcp_fix_ep_.ipv4(ipv4);
		dhcp_fix_ep_.port(port);
	}
	nsp::tcpip::endpoint get_dhcp_fix_ep(){ return dhcp_fix_ep_; }

private:
	void clean_session();

private:
	nsp::tcpip::endpoint local_ep_;
	nsp::tcpip::endpoint dhcp_fix_ep_;
	std::shared_ptr<dhcp_udp_session> dhcp_session_;
	
	std::string m_mac_addr_;
};

#endif /* __GRACE_ROBOT_AGVSHELL_DHCP_UDP_SESSION_H__ */
