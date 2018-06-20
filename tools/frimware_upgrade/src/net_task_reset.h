#pragma once

#include <string>
#include "proto_udp_typedef.h"
#include <functional>
#include "singleton.hpp"

class net_task_reset{
public:
	net_task_reset();
	~net_task_reset();
	friend class nsp::toolkit::singleton<net_task_reset>;
public:
	void regiset_callback(const std::function<void(const std::string&, const int type, const int, const nsp::proto::errorno_t)>& func);
	void net_task_reset::regiset_get_vcu_callback(const std::function<void(const std::string&, const operate_code operate_type, const std::string&, const nsp::proto::errorno_t)>& func);
	void get_vcu_type_version_request(FIRMWARE_SOFTWARE_TYPE type, std::string& ip_addr, int is_control);
	int net_reset(std::string& ip_addr);
private:
	uint8_t	node_id_;
	uint8_t  bas_serial_number_;
	int	 is_can_type_;

	std::function<void(const std::string&, const int type, const int, const nsp::proto::errorno_t)> function_callback_;
	std::function<void(const std::string&, const operate_code operate_type, const std::string&, const nsp::proto::errorno_t)> function_get_vcu_callback_;
};