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
	int post_start_data_transmit_func(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, nsp::tcpip::endpoint ep);

	int post_close_data_transmit_func(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, nsp::tcpip::endpoint ep);

	int post_read_register_cmd(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, nsp::tcpip::endpoint ep, uint16_t register_index);

	int post_write_register_cmd(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, nsp::tcpip::endpoint ep, uint16_t register_index, const std::string &strdata);

	int post_read_version_cmd(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, nsp::tcpip::endpoint ep);

	int post_initial_rom_update_cmd(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, nsp::tcpip::endpoint ep, uint16_t blocknumber );

	int post_download_datapacket_cmd(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, nsp::tcpip::endpoint ep, std::string & datastr, uint16_t index);

	int post_abort_update_cmd(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, nsp::tcpip::endpoint ep);

protected:
	virtual void on_recvdata(const std::string &data, const nsp::tcpip::endpoint &r_ep);
private:
	int psend(const nsp::proto::proto_interface& package,const nsp::tcpip::endpoint& ep);

	void dispatch_recv_info(const uint16_t main_operate, const uint16_t pkt_id, const std::shared_ptr<nsp::proto::udp::recv_data>& pkt, const nsp::tcpip::endpoint &r_ep);

	int assign_data(nsp::proto::udp::common_data & data, nsp::proto::udp::can_datapacket_t &candata);
};