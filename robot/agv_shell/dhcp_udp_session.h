#ifndef __GRACE_ROBOT_AGVSHELL_DHCP_UDP_SESSION_H__
#define __GRACE_ROBOT_AGVSHELL_DHCP_UDP_SESSION_H__

#include "endpoint.h"
#include "network_handler.h"
#include "serialize.hpp"

class dhcp_udp_session :public ::nsp::tcpip::obudp{

public:
	dhcp_udp_session();
	~dhcp_udp_session();
	
public:
	int psend(const nsp::proto::proto_interface& package,const nsp::tcpip::endpoint& ep);
	
	int post_local_info_request(int shell_port, int port, const std::string& mac, const nsp::tcpip::endpoint& ep);

protected:
	virtual void on_recvdata(const std::string &data, const nsp::tcpip::endpoint &r_ep) override final;

private:
	void dispatch_mac_addr();
};

#endif /* __GRACE_ROBOT_AGVSHELL_DHCP_UDP_SESSION_H__ */
