#pragma once

#include <string>
#include <stdint.h>
#include <vector>
#include "agv_shell_common.h"
#include "application_network_framwork.hpp"
#include "compiler.h"

class dhcp_server_base
{
public:
	dhcp_server_base();
	~dhcp_server_base();

public:
	int login_dhcp_server(const std::string& ipv4,const uint16_t port);
	void disconnect_dhcp_server();

	int get_dhcp_addr_all(std::vector<mac_address_info>& vct_shell);
	//登陆成功之后调用注册回调通知函数
	int regist_dhcp_notify(void(STDCALL* func)());

private:
	std::shared_ptr<nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>> client_session_;

};