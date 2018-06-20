#pragma once
#include "application_network_framwork.hpp"
#include "proto_pre_login.h"
#include "proto_login.h"
#define PROTO_SESSION_KEY_LENGTH 32
typedef enum
{
	KNetwork_Disconnect = -2,
	KNetwork_Connected = 0,
	KNetwork_Login
}net_status_t;

class agv_session :public nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>
{
public:
	agv_session();
	agv_session(HTCPLINK lnk);
	~agv_session();

public:
	virtual int on_connected();
	int receive_login_request_ack(uint32_t pkt_id, int error);
	int receive_login_request(const std::shared_ptr<nsp::proto::proto_login_robot> &pkt);

protected:
	virtual void on_recvdata(const std::string &pkt);

private:
	unsigned char key_[PROTO_SESSION_KEY_LENGTH];
	std::atomic<net_status_t> cur_status_{ KNetwork_Disconnect };
	std::atomic<uint64_t> refresh_tick_{ 0 };

private:
	HTCPLINK __session_link;
};

