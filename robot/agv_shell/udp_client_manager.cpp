#include "udp_client_manager.h"
#include "log.h"

udp_client_manager::udp_client_manager(){

}

udp_client_manager::~udp_client_manager(){
	clean_session();
}

int udp_client_manager::init_network(){
	if (net_session_){
		loerror("agv_shell") << "udp network session is already exists.";
		return 0;
	}
	try{
		net_session_ = std::make_shared<udp_session>();
	}
	catch (...){
		return -1;
	}
	if (net_session_){
		if (net_session_->create(local_ep_obj_) < 0){
			loerror("agv_shell") << "failed to create udp session object.";
			return -1;
		}
		return 0;
	}
	return -1;
}

void udp_client_manager::clean_session(){
	if (net_session_){
		net_session_->close();
	}
	net_session_ = nullptr;
}

int udp_client_manager::post_vcu_type_request(const nsp::tcpip::endpoint& ep,
	const std::shared_ptr<motion::asio_block> &asio_ack){
	if (!net_session_){
		return -1;
	}
	return net_session_->post_vcu_type_request(ep, asio_ack);
}

int udp_client_manager::post_vcu_version_request(const FIRMWARE_SOFTWARE_TYPE type,
	const nsp::tcpip::endpoint& ep, const int is_control_, const std::shared_ptr<motion::asio_block> &asio_ack){
	if (!net_session_){
		return -1;
	}
	return net_session_->post_vcu_version_request(type, ep, is_control_, asio_ack);
}

int udp_client_manager::post_vcu_reset_request(const nsp::tcpip::endpoint& ep, const uint32_t file_size, const std::shared_ptr<motion::asio_block> &asio_ack){
	if (!net_session_){
		return -1;
	}
	return net_session_->post_vcu_reset_request(ep, file_size, asio_ack);
}

int udp_client_manager::post_cpu_type_requst(const nsp::tcpip::endpoint& ep, const std::shared_ptr<motion::asio_block> &asio_ack)
{
	if (!net_session_){
		return -1;
	}
	return net_session_->post_cpu_type_requst(ep, asio_ack);
}

int udp_client_manager::post_vcu_write_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const nsp::tcpip::endpoint& ep, const int block_offset,
	const std::string&file_block_data, const std::shared_ptr<motion::asio_block>& asio_ack){
	if (!net_session_){
		return -1;
	}
	return net_session_->post_vcu_write_bin_file(type, ep, block_offset, file_block_data, asio_ack);
}

int udp_client_manager::post_vcu_read_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const nsp::tcpip::endpoint& ep, const int block_offset,
	const int buffer_length, const std::shared_ptr<motion::asio_block>& asio_ack){
	if (!net_session_){
		return -1;
	}
	return net_session_->post_vcu_read_bin_file(type, ep, block_offset, buffer_length, asio_ack);
}

int udp_client_manager::post_vcu_keep_alive_request(const nsp::tcpip::endpoint& ep) {
	if (!net_session_){
		return -1;
	}
	return net_session_->post_vcu_keep_alive_request(ep);
}

int udp_client_manager::post_query_vcu_keep_alive_request(const nsp::tcpip::endpoint& ep, const std::shared_ptr<motion::asio_block> &asio_ack) {
	if (!net_session_){
		return -1;
	}
	return net_session_->post_query_vcu_keep_alive_request(ep, asio_ack);
}

int udp_client_manager::post_set_vcu_keep_alive_request(const nsp::tcpip::endpoint& ep, int status, const std::shared_ptr<motion::asio_block> &asio_ack) {
	if (!net_session_){
		return -1;
	}
	return net_session_->post_set_vcu_keep_alive_request(ep, status, asio_ack);
}

int udp_client_manager::post_can_write_bin_file(const nsp::tcpip::endpoint& ep, const int block_offset, const std::string&file_block_data,
	const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid)
{
	if (!net_session_){
		return -1;
	}
	return net_session_->post_can_write_bin_file(ep, block_offset, file_block_data, asio_ack, code_serial, nodeid);
}

int udp_client_manager::post_can_reset_request(const nsp::tcpip::endpoint& ep, const uint8_t node_id, const uint8_t code_serial,
	const std::shared_ptr<motion::asio_block> &asio_ack)
{
	if (!net_session_){
		return -1;
	}
	return net_session_->post_can_reset_request(ep, node_id, code_serial, asio_ack);
}

int udp_client_manager::post_can_version_request(const nsp::tcpip::endpoint& ep, const uint8_t nodeid, const uint8_t code_serial,
	const std::shared_ptr<motion::asio_block> &asio_ack)
{
	if (!net_session_){
		return -1;
	}
	return net_session_->post_can_version_request(ep, nodeid, code_serial, asio_ack);
}

int udp_client_manager::post_can_type_request(const nsp::tcpip::endpoint& ep, const uint8_t node_id, const uint8_t code_serial,
	const std::shared_ptr<motion::asio_block> &asio_ack)
{
	if (!net_session_){
		return -1;
	}
	return net_session_->post_can_type_request(ep, node_id, code_serial, asio_ack);
}

int udp_client_manager::post_can_data_forward(const nsp::tcpip::endpoint& ep, const uint8_t code_serial,
	const std::shared_ptr<motion::asio_block> &asio_ack)
{
	if (!net_session_){
		return -1;
	}
	return net_session_->post_start_data_foward(ep, code_serial, asio_ack);
}

int udp_client_manager::post_can_init_rom_request(const nsp::tcpip::endpoint& ep, const uint8_t node_id, const uint8_t code_serial,
	const std::shared_ptr<motion::asio_block> &asio_ack, const int & block_number)
{
	if (!net_session_){
		return -1;
	}
	return net_session_->post_can_init_rom_request(ep, block_number, node_id, code_serial, asio_ack);
}

void udp_client_manager::set_m_core_endpoint(const std::string& ipv4, const uint16_t port){
	m_core_ep_obj_.ipv4(ipv4);
	m_core_ep_obj_.port(port);
}

void udp_client_manager::set_camera_endpoint(const std::string& ipv4, const uint16_t port){
	camera_ep_obj_.ipv4(ipv4);
	camera_ep_obj_.port(port);
}

void udp_client_manager::set_local_endpoint(const std::string& ipv4, const uint16_t port){
	local_ep_obj_.ipv4(ipv4);
	local_ep_obj_.port(port);
}