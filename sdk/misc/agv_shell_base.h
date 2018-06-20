#pragma once

#include <string>
#include <vector>
#include <mutex>
#include "agv_shell_common.h"
#include "application_network_framwork.hpp"

class agv_shell_base{
public:
	agv_shell_base();
	~agv_shell_base();

public:
	int login_agv_shell(const std::string& ipv4, const uint16_t port);
	void disconnect_agv_shell();

	int post_agv_shell_process_cmd(const std::vector<process_command>& vct_p_command, const command_agv_shell cmd);
	int get_agv_shell_process_list(std::vector<process_info>& vct_process);
	int get_agv_shell_process_status(std::vector<process_status>& vct_p);

private:
	std::shared_ptr<nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>> client_session_;
	std::vector<process_info> list_process_;//存放agv_shell的进程列表信息

	std::recursive_mutex lock_mutex_;
};
