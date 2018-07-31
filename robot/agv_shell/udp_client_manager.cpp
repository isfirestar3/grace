#include "udp_client_manager.h"
#include "const.h"
#include "log.h"

udp_client_manager::udp_client_manager() : waiter_(0) {

}

udp_client_manager::~udp_client_manager(){
	clean_session();
	
	waiter_.sig();
    if (loop_th_) {
        if (loop_th_->joinable()) {
            loop_th_->join();
            delete loop_th_;
            loop_th_ = nullptr;
        }
    }
    waiter_.reset();
}

int udp_client_manager::init_network(){
	do 
	{
		if ( !net_session_ ){
			try{
				net_session_ = std::make_shared<udp_session>();
				
				if (net_session_->create(local_ep_obj_) < 0){
					loerror("agv_shell") << "failed to create udp session object.";
					break;
				}
			} catch (...) {
				loerror("agv_shell") << "make udp_session point failed.";
			}
		}
	} while(0);
	
	do 
	{
		if ( !dhcp_session_ ){
			try{
				dhcp_session_ = std::make_shared<dhcp_udp_session>();
				
				if (dhcp_session_->create(0) < 0){
					loerror("agv_shell") << "failed to create dhcp udp session object.";
					break;
				}
			} catch (...) {
				loerror("agv_shell") << "make dhcp udp session point failed.";
			}
		}
	} while(0);
	
	try {
        if (!loop_th_) {
            loop_th_ = new std::thread( std::bind( &udp_client_manager::keepalive, this ) );
        }
    } catch (...) {
        loerror("agv_shell") << "udp manager create thread failure ";
        return -1;
    }
	
	return 0;
}

void udp_client_manager::clean_session(){
	if (net_session_){
		net_session_->close();
	}
	net_session_ = nullptr;
	
	if (dhcp_session_){
		dhcp_session_->close();
	}
	dhcp_session_ = nullptr;
}

void udp_client_manager::keepalive() {
	int res = -1;
	
    while (waiter_.wait(VCU_UDP_KEEPALIVE_TIME_INT) > 0) {
		//每3秒钟向vcu发送一次心跳包
		res = nsp::toolkit::singleton<udp_client_manager>::instance()->post_vcu_keep_alive_request(m_core_ep_obj_);
		if (res < 0) {
			loerror("agv_shell") << "send keep alive package to vcu failed.";
		}
    }
    waiter_.reset();
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

int udp_client_manager::post_local_info_request(int shell_port, int port, const std::string& mac) {
	if (!dhcp_session_){
		return -1;
	}
	
	return dhcp_session_->post_local_info_request(shell_port, port, mac, dhcp_fix_ep_);
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