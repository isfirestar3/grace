#pragma once

#include "network_handler.h"
#include "endpoint.h"
#include "serialize.hpp"
#include "asio.hpp"
#include "net_manager.h"
#include "proto_udp_typedef.h"
#include "proto_udp_common.h"

class udp_session :public ::nsp::tcpip::obudp{

public:
	udp_session();
	~udp_session();

public:
	int post_vcu_type_request(const nsp::tcpip::endpoint& ep,
		const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_vcu_version_request(const FIRMWARE_SOFTWARE_TYPE type, const nsp::tcpip::endpoint& ep, const int is_control_,
		const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_vcu_reset_request(const nsp::tcpip::endpoint& ep,  const uint32_t file_size,
		const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_vcu_write_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const nsp::tcpip::endpoint& ep, 
		const int block_offset, const std::string&file_block_data, 
		const std::shared_ptr<motion::asio_block>& asio_ack);
	int post_vcu_read_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const nsp::tcpip::endpoint& ep,
		const int block_offset, const int buffer_length,
		const std::shared_ptr<motion::asio_block>& asio_ack);
	int post_cpu_type_requst(const nsp::tcpip::endpoint& ep, const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_vcu_keep_alive_request(const nsp::tcpip::endpoint& ep);
	int post_query_vcu_keep_alive_request(const nsp::tcpip::endpoint& ep, const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_set_vcu_keep_alive_request(const nsp::tcpip::endpoint& ep, int status, const std::shared_ptr<motion::asio_block> &asio_ack);

	//以下为can协议包
	int post_start_data_foward(const nsp::tcpip::endpoint& ep, uint8_t code_serial, 
		const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_can_type_request(const nsp::tcpip::endpoint& ep, const uint8_t nodeid, const uint8_t code_serial,
		const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_can_version_request(const nsp::tcpip::endpoint& ep, const uint8_t nodeid, const uint8_t code_serial,
		const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_can_reset_request(const nsp::tcpip::endpoint& ep,const uint8_t node_id,const uint8_t code_serial,
		const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_can_abort_update_request(const nsp::tcpip::endpoint& ep, uint8_t nodeid, uint8_t code_serial,
		const std::shared_ptr<motion::asio_block> &asio_ack);
	int post_can_write_bin_file(const nsp::tcpip::endpoint& ep, const int block_offset, const std::string&file_block_data,
		const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid);
	int post_can_read_bin_file(const nsp::tcpip::endpoint& ep, const int block_offset,
		const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid);
	int post_can_read_register_request(const nsp::tcpip::endpoint& ep, uint8_t code_serial, 
		const std::shared_ptr<motion::asio_block> &asio_ack, uint16_t register_index, uint8_t nodeid);
	int post_can_write_register_request(const nsp::tcpip::endpoint& ep, uint8_t code_serial,
		const std::shared_ptr<motion::asio_block> &asio_ack, uint16_t register_index, uint8_t nodeid, std::string strdata);
	int post_can_init_rom_request(const nsp::tcpip::endpoint& ep, const uint16_t block_number, uint8_t nodeid, uint8_t code_serial, 
		const std::shared_ptr<motion::asio_block> &asio_ack);


protected:
	virtual void on_recvdata(const std::string &data, const nsp::tcpip::endpoint &r_ep)override final;

private:
	int psend(const nsp::proto::proto_interface& package,const nsp::tcpip::endpoint& ep);
	void dispatch_vcu_error_info(const uint16_t pkt_id, const EXECUTE_ACK type);
	void dispatch_vcu_recv_info(const uint16_t main_operate, const uint16_t pkt_id,
		const std::shared_ptr<nsp::proto::udp::recv_data>& pkt, const nsp::tcpip::endpoint &r_ep);
	void dispatch_vcu_get_info(const uint16_t pkt_id,const std::shared_ptr<nsp::proto::udp::recv_data>& pkt, const nsp::tcpip::endpoint &r_ep);
	void dispatch_vcu_rw_firmware(const uint16_t pkt_id,const short int sub_operate, 
		const std::shared_ptr<nsp::proto::udp::recv_data>& pkt, const nsp::tcpip::endpoint &r_ep);
	void dispatch_vcu_keep_alive(const uint16_t pkt_id, const std::shared_ptr<nsp::proto::udp::recv_data>& pkt, const nsp::tcpip::endpoint &r_ep);
};