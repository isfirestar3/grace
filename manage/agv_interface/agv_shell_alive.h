#pragma once

#include  "application_network_framwork.hpp"
#include <vector>
#include <thread>
#include "os_util.hpp"

class agv_shell_alive
{
public:
	agv_shell_alive();
	~agv_shell_alive();

public:
	void add_session(const std::shared_ptr<nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>>& ptr);
	void remove_session(const std::shared_ptr<nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>>& ptr);

private:
	void init();
	void uinit();
	void run_check_th();

private:
	std::recursive_mutex session_mutex_;
	std::vector<std::shared_ptr<nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>>> vct_session_;

	nsp::os::waitable_handle th_wait_;
	std::thread* timeout_th_;
	std::atomic<bool> is_exits_ = { false };
};