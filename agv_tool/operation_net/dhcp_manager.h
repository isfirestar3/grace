#pragma once

#include <memory>
#include <atomic>
#include "task_scheduler.hpp"
#include "singleton.hpp"

class dhcp_manager {
	dhcp_manager();
	~dhcp_manager();
	friend class nsp::toolkit::singleton<dhcp_manager>;

public:
	//DHCP使用业务
	int connect_to_dhcp();
	int disconnect_dhcp();
	void network_callback_notify();
	void set_network_callback(void(__stdcall* func)(const char* robot_ip, const int type, const int error, const char* data_str, const int cb));
	int post_sync_without_id(const int type, const void* str, const int cb, void **_ack_msg, int *_ack_len);

private:
	std::function<void(const char* ip, const int type, const int error, const char* data_str, const int cb)> network_callback_;
};