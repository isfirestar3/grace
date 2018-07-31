#pragma once
#include "config.h"
#include "network_session.h"
#include "singleton.hpp"
#include <map>
#include <string>
#include <memory>
#include "proto_udp_typedef.h"

class network_client_manager{
	network_client_manager();
	~network_client_manager();
	friend class nsp::toolkit::singleton<network_client_manager>;

public:
	int init_network(const std::string& ip_port_local,const std::string& ip_por_str);
	void set_firmware_info(const firmware_info &firmware_info_);
	int clean_session();
	int post_vcu_type_request(const std::string ip, const std::shared_ptr<motion::asio_block> &asio_ack, uint8_t node_id = 0, enum device_type type_device = kdevice_vcu_default, uint8_t code_serial = 0x00);
	int post_vcu_version_request(const FIRMWARE_SOFTWARE_TYPE type, const std::string ip, const int is_control_, const std::shared_ptr<motion::asio_block> &asio_ack, uint8_t node_id = 0, enum device_type type_device = kdevice_vcu_default, uint8_t code_serial = 0x00);
	int post_vcu_reset_request(const std::string ip, const std::shared_ptr<motion::asio_block> &asio_ack, uint8_t code_serial, uint8_t nodeid);
	int post_vcu_write_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const std::string ip,const int block_offset,
		const std::string&file_block_data, const std::shared_ptr<motion::asio_block>& asio_ack);
	int post_vcu_read_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const std::string ip, const int block_offset, 
		const int buffer_length, const std::shared_ptr<motion::asio_block>& asio_ack);
	int post_cpu_type_requst(const std::string ip, const std::shared_ptr<motion::asio_block> &asio_ack);

	int start_data_foward(const std::string ip, const std::shared_ptr<motion::asio_block> &asio_ack, uint8_t code_serial);

	int initial_rom_update(const std::string ip, const std::shared_ptr<motion::asio_block> &asio_ack, uint8_t code_serial, const uint16_t block_number, uint8_t nodeid,uint16_t check_sum );

	int post_vcu_write_bin_file_fordward(const std::string ip, const int block_offset, const std::string&file_block_data, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid);

	int post_vcu_read_bin_file_forward(const std::string ip, const int block_offset, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid);

	int read_register_data(const std::string ip, const std::shared_ptr<motion::asio_block> &asio_ack, uint8_t code_serial, uint16_t register_index, uint8_t nodeid);

	int write_register_data(const std::string ip, const std::shared_ptr<motion::asio_block> &asio_ack, uint8_t code_serial, uint16_t register_index, uint8_t nodeid, std::string strdata);

	int abort_update_request(const std::string ip, const std::shared_ptr<motion::asio_block> &asio_ack, uint8_t code_serial, uint8_t nodeid);

private:
	//std::shared_ptr<network_session> search_session(const std::string ip, nsp::tcpip::endpoint& ep);
	nsp::tcpip::endpoint search_endpoint(const std::string ip);
private:
	std::recursive_mutex re_mutex_;
	//std::map<std::string, std::shared_ptr<network_session>> map_sessions_;//针对不同的ip地址的session集合
	std::map<std::string, nsp::tcpip::endpoint> map_endpoint_;//缓存所有的ip的endpoint对象

	std::shared_ptr<network_session> net_session_;
};