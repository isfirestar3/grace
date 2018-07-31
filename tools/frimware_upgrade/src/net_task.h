#pragma once
#include "config.h"
#include <string>
#include "proto_udp_typedef.h"
#include <functional>
#include "messageheader.h"


class net_task{
public:
	net_task(const std::string& local_ip, const std::string& ip, const unsigned int port, const FIRMWARE_SOFTWARE_TYPE type, const int is_control, 
		uint8_t node_id = 0, enum  device_type type_device = kdevice_vcu_default);

	net_task(const std::string& local_ip, const std::string& ip, const unsigned int port, enum message_type messagetype, uint8_t serial_id, uint8_t node_id, uint8_t register_index = 0, int is_upload = 0);

	~net_task();

	void set_firmware_info(const firmware_info &firmware_info_)
	{
		m_firmware_info_ = firmware_info_;
	}
public:
	void on_task();
	bool init_net();
	
	//注册回调函数，参数一：收到数据来自哪个ip地址，参数二：数据属于哪个协议类型，参数三：数据部分
	void regiset_callback(const std::function<void(const std::string&, const int operate_type, const std::string&, const nsp::proto::errorno_t)>& func);

	int start_data_foward();
	int get_vcu_type();
	int get_vcu_version();

	int read_register_data();

	int write_register_data();

	int net_reset( );
private:
	std::string ip_local_;
	std::string ip_addr_;
	unsigned int port_;
	FIRMWARE_SOFTWARE_TYPE f_type_;
	int is_control_;
	enum  device_type  kdevice_type_;
	uint8_t	node_id_;
	uint8_t  serial_code_;
	uint8_t  index_register_;
	int	 is_can_type_;
	enum message_type  messagetype_;
	int	 is_upload_;
	firmware_info m_firmware_info_;

	std::function<void(const std::string&, const int operate_type , const std::string&, const nsp::proto::errorno_t)> function_callback_;
};