#pragma once

#include <string>
#include "proto_udp_typedef.h"
#include <functional>

class net_task{
public:
	net_task(const std::string& local_ip, const std::string& ip, const unsigned int port, const FIRMWARE_SOFTWARE_TYPE type, const int is_control);
	~net_task();

public:
	void on_task();
	//注册回调函数，参数一：收到数据来自哪个ip地址，参数二：数据属于哪个协议类型，参数三：数据部分
	void regiset_callback(const std::function<void(const std::string&, const int type, const std::string&, const nsp::proto::errorno_t)>& func);

private:
	int get_vcu_type();
	int get_vcu_version();
	int get_vcu_cpu();

private:
	std::string ip_local_;
	std::string ip_addr_;
	unsigned int port_;
	FIRMWARE_SOFTWARE_TYPE f_type_;
	int is_control_;

	std::function<void(const std::string&, const int type, const std::string&, const nsp::proto::errorno_t)> function_callback_;
};