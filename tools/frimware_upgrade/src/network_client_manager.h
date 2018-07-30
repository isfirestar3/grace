#pragma once

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
	int init_network(const std::string& ip_local,const std::string& ip_por_str);
	int clean_session();
	int post_vcu_type_request(const std::string ip, const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_vcu_version_request(const FIRMWARE_SOFTWARE_TYPE type, const std::string ip, const int is_control_, const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_vcu_reset_request(const std::string ip,const uint32_t file_size, const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_vcu_write_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const std::string ip,const int block_offset,
		const std::string&file_block_data, const std::shared_ptr<motion::asio_block>& asio_ack);
	int post_vcu_read_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const std::string ip, const int block_offset, 
		const int buffer_length, const std::shared_ptr<motion::asio_block>& asio_ack);
	int post_cpu_type_requst(const std::string ip, const std::shared_ptr<motion::asio_block> &asio_ack);

private:
	//std::shared_ptr<network_session> search_session(const std::string ip, nsp::tcpip::endpoint& ep);
	nsp::tcpip::endpoint search_endpoint(const std::string ip);

private:
	std::recursive_mutex re_mutex_;
	//std::map<std::string, std::shared_ptr<network_session>> map_sessions_;//针对不同的ip地址的session集合
	std::map<std::string, nsp::tcpip::endpoint> map_endpoint_;//缓存所有的ip的endpoint对象

	std::shared_ptr<network_session> net_session_;
};