#pragma once

#include <string>
#include "proto_udp_typedef.h"
#include <functional>
#include "file_write_handler.h"

class net_task_upload{
public:
	net_task_upload(const std::string& local_ip, const std::string& ip, const unsigned int port,
		const FIRMWARE_SOFTWARE_TYPE type, const unsigned int file_length, const std::string& f_name);
	~net_task_upload();

public:
	void on_task();
	//注册回调函数，参数一：收到数据来自哪个ip地址，参数二：数据属于哪个协议类型，参数三：数据部分,当前完成百分比,参数四：当前一片数据是否成功
	void regiset_callback(const std::function<void(const std::string&, const int, const std::string& data, const nsp::proto::errorno_t)>& func);
	
private:

	void get_bin_file();
private:
	std::string ip_local_;
	std::string ip_addr_;
	uint32_t port_;
	FIRMWARE_SOFTWARE_TYPE f_type_;
	unsigned int firmware_length_;
	std::string file_name_;

	uint8_t	node_id_;
	uint8_t  bas_serial_number_;
	int	 is_can_type_;

	std::function<void(const std::string&, const int operate_type, const std::string& , const nsp::proto::errorno_t)> function_callback_;
};