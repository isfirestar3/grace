#pragma once

#include "network_handler.h"
#include "endpoint.h"
#include "serialize.hpp"
#include "asio.hpp"
#include "net_manager.h"
#include "proto_udp_typedef.h"
#include "proto_udp_common.h"

class network_session :public ::nsp::tcpip::obudp{

public:
	network_session();
	~network_session();

public:
	int post_vcu_type_request(const nsp::tcpip::endpoint& ep,
		const std::shared_ptr<motion::asio_block> &asio_ack, uint8_t node_id = 0, enum device_type type_device = kdevice_vcu_default, uint8_t code_serial = 0x00 );
	int post_vcu_version_request(const FIRMWARE_SOFTWARE_TYPE type, const nsp::tcpip::endpoint& ep, const int is_control_,
		const std::shared_ptr<motion::asio_block> &asio_ack,uint8_t node_id = 0, enum device_type type_device = kdevice_vcu_default, uint8_t code_serial = 0x00);
	int post_vcu_reset_request(const nsp::tcpip::endpoint& ep,
		const std::shared_ptr<motion::asio_block> &asio_ack, uint8_t code_serial, uint8_t nodeid);
	int post_vcu_write_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const nsp::tcpip::endpoint& ep, 
		const int block_offset, const std::string&file_block_data, 
		const std::shared_ptr<motion::asio_block>& asio_ack);
	int post_vcu_read_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const nsp::tcpip::endpoint& ep,
		const int block_offset, const int buffer_length,
		const std::shared_ptr<motion::asio_block>& asio_ack);
	int post_cpu_type_requst(const nsp::tcpip::endpoint& ep, const std::shared_ptr<motion::asio_block> &asio_ack);

	int start_data_foward(const nsp::tcpip::endpoint& ep, uint8_t code_serial, const std::shared_ptr<motion::asio_block> &asio_ack);

	int initial_rom_update(const nsp::tcpip::endpoint& ep, uint8_t code_serial, const std::shared_ptr<motion::asio_block> &asio_ack, const uint16_t block_number, uint8_t nodeid, uint16_t check_sum);

	int post_vcu_write_bin_file_fordward(const nsp::tcpip::endpoint& ep, const int block_offset, const std::string&file_block_data, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid);

	int post_vcu_read_bin_file_forward(const nsp::tcpip::endpoint& ep, const int block_offset,const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid);

	int read_register_data(const nsp::tcpip::endpoint& ep, uint8_t code_serial, const std::shared_ptr<motion::asio_block> &asio_ack, uint16_t register_index, uint8_t nodeid);

	int write_register_data(const nsp::tcpip::endpoint& ep, uint8_t code_serial, const std::shared_ptr<motion::asio_block> &asio_ack, uint16_t register_index, uint8_t nodeid, std::string strdata);

	int abort_update_request(const nsp::tcpip::endpoint& ep, const std::shared_ptr<motion::asio_block> &asio_ack, uint8_t code_serial, uint8_t nodeid);
protected:
	virtual void on_recvdata(const std::string &data, const nsp::tcpip::endpoint &r_ep);


private:
	int psend(const nsp::proto::proto_interface& package,const nsp::tcpip::endpoint& ep);
	
	void dispatch_vcu_recv_info(const uint16_t main_operate,const uint16_t pkt_id, const std::shared_ptr<nsp::proto::udp::recv_data>& pkt, const nsp::tcpip::endpoint &r_ep);
	void dispatch_vcu_get_info(const uint16_t pkt_id,const std::shared_ptr<nsp::proto::udp::recv_data>& pkt, const nsp::tcpip::endpoint &r_ep);
	void dispatch_vcu_rw_firmware(const uint16_t pkt_id,const short int sub_operate, 
		const std::shared_ptr<nsp::proto::udp::recv_data>& pkt, const nsp::tcpip::endpoint &r_ep);
};