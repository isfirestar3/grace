#pragma once

#include "application_network_framwork.hpp"
#include "proto_agv_msg.h"

class udp_session : public nsp::tcpip::obudp
{
public:
	udp_session();
	~udp_session();

public:

protected:
	virtual void on_recvdata(const std::string &data, const nsp::tcpip::endpoint &r_ep)override final;

private:
	int recv_mac_info(const std::shared_ptr<nsp::proto::proto_local_info>&, const std::string& ipv4);
};