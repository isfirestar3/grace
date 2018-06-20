#pragma once

#include <string>
#include "proto_udp_typedef.h"
#include <functional>

class net_task_download{
public:
	net_task_download(const std::string& ip, const FIRMWARE_SOFTWARE_TYPE type, const uint32_t reset_timeout, const uint32_t block_delay, const uint32_t is_control);
	net_task_download(const std::string& ip, const uint32_t reset_timeout, const uint32_t block_delay, uint8_t serial_id, uint8_t node_id);

	~net_task_download();

public:
	void on_task();
	//注册回调函数，参数一：收到数据来自哪个ip地址，参数二：数据属于哪个协议类型，参数三：数据部分,当前完成百分比,参数四：当前一片数据是否成功
	void regiset_callback(const std::function<void(const std::string&, const int type, const int, const nsp::proto::errorno_t)>& func);
	//注册在重启后回调版本号信息函数
	void regiset_get_vcu_callback(const std::function<void(const std::string&, const int operate_type, const std::string&, const nsp::proto::errorno_t)>& func);

private:
	int get_bin_file(const int block_number);
	void get_vcu_type_version_request();
	int write_bin_file_to_vcu(const int block_num);
	int post_reset_request();

	int initial_rom_update(int block_num, uint16_t check_sum);
private:
	std::string ip_addr_;
	FIRMWARE_SOFTWARE_TYPE f_type_;
	uint32_t is_control_;
	uint32_t reset_timeout_;
	uint32_t block_write_delay_;
	nsp::os::waitable_handle reset_wait_;
	nsp::os::waitable_handle block_write_wait_;
	uint8_t	node_id_;
	uint8_t  serial_code_;
	int	 is_can_type_;

	std::function<void(const std::string&, const int type, const int, const nsp::proto::errorno_t)> function_callback_;
	std::function<void(const std::string&, const int operate_type, const std::string&, const nsp::proto::errorno_t)> function_get_vcu_callback_;
};