#include "network_client_manager.h"
#include "log.h"

network_client_manager::network_client_manager(){

}

network_client_manager::~network_client_manager(){

}

int network_client_manager::init_network(const std::string& ip_local, const std::string& ip_por_str){
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	nsp::tcpip::endpoint ep;
	if (nsp::tcpip::endpoint::build(ip_por_str, ep) < 0){
		loinfo("udp_net") << "failed to build endpoint,the input string is " << ip_por_str;
		return 0;
	}
	/*if (map_sessions_.find(ep.ipv4()) != map_sessions_.end()){
		loinfo("udp_net") << "ip is " << ep.ipv4() << " session is already exists.";
		return 0;
	}*/
	//std::shared_ptr<network_session> net_session;
	if (net_session_){
		loinfo("udp_net") << "ip is " << ep.ipv4() << " session is already exists.";
		return 0;
	}
	try{
		net_session_ = std::make_shared<network_session>();
	}
	catch (...){
		return -1;
	}
	nsp::tcpip::endpoint ep_local(ip_local.c_str(), ep.port());
	if (net_session_){
		if (net_session_->create(ep_local) < 0){
			return -1;
		}
		//map_sessions_.insert(std::make_pair(ep.ipv4(), net_session));
		map_endpoint_.insert(std::make_pair(ep.ipv4(), ep));
		return 0;
	}
	return -1;
}

int network_client_manager::clean_session(){
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	if (net_session_){
		net_session_->close();
	}
	/*for (auto& iter : map_sessions_){
		iter.second->close();
	}
	map_sessions_.clear();*/
	net_session_.swap(std::shared_ptr<network_session>(nullptr));
	map_endpoint_.clear();
	return 0;
}

int network_client_manager::post_vcu_type_request(const std::string ip, 
	const std::shared_ptr<motion::asio_block> &asio_ack){
	nsp::tcpip::endpoint ep = search_endpoint(ip);
	//std::shared_ptr<network_session> net_session = search_session(ip, ep);
	if (!net_session_){
		return -1;
	}
	return net_session_->post_vcu_type_request(ep, asio_ack);
}

int network_client_manager::post_vcu_version_request(const FIRMWARE_SOFTWARE_TYPE type,
	const std::string ip, const int is_control_, const std::shared_ptr<motion::asio_block> &asio_ack){
	nsp::tcpip::endpoint ep = search_endpoint(ip);
	//std::shared_ptr<network_session> net_session = search_session(ip, ep);
	if (!net_session_){
		return -1;
	}
	return net_session_->post_vcu_version_request(type, ep, is_control_, asio_ack);
}

int network_client_manager::post_vcu_reset_request(const std::string ip, const uint32_t file_size, const std::shared_ptr<motion::asio_block> &asio_ack){
	nsp::tcpip::endpoint ep = search_endpoint(ip);
	//std::shared_ptr<network_session> net_session = search_session(ip, ep);
	if (!net_session_){
		return -1;
	}
	return net_session_->post_vcu_reset_request(ep, file_size, asio_ack);
}

int network_client_manager::post_cpu_type_requst(const std::string ip, const std::shared_ptr<motion::asio_block> &asio_ack)
{
	nsp::tcpip::endpoint ep = search_endpoint(ip);
	//std::shared_ptr<network_session>net_session = search_session(ip,ep);
	if (!net_session_){
		return -1;
	}
	return net_session_->post_cpu_type_requst(ep, asio_ack);
}

int network_client_manager::post_vcu_write_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const std::string ip, const int block_offset,
	const std::string&file_block_data, const std::shared_ptr<motion::asio_block>& asio_ack){
	nsp::tcpip::endpoint ep = search_endpoint(ip);
	//std::shared_ptr<network_session> net_session = search_session(ip, ep);
	if (!net_session_){
		return -1;
	}
	return net_session_->post_vcu_write_bin_file(type, ep, block_offset, file_block_data, asio_ack);
}

int network_client_manager::post_vcu_read_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const std::string ip, const int block_offset,
	const int buffer_length, const std::shared_ptr<motion::asio_block>& asio_ack){
	nsp::tcpip::endpoint ep = search_endpoint(ip);
	//std::shared_ptr<network_session> net_session = search_session(ip, ep);
	if (!net_session_){
		return -1;
	}
	return net_session_->post_vcu_read_bin_file(type, ep, block_offset, buffer_length, asio_ack);
}

//std::shared_ptr<network_session> network_client_manager::search_session(const std::string ip, nsp::tcpip::endpoint& ep){
//	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
//	auto iter = map_endpoint_.find(ip);
//	if (iter == map_endpoint_.end()){
//		return nullptr;
//	}
//	ep = iter->second;
//	auto iter_session = map_sessions_.find(ip);
//	if (iter_session == map_sessions_.end()){
//		return nullptr;
//	}
//	return iter_session->second;
//}

nsp::tcpip::endpoint network_client_manager::search_endpoint(const std::string ip){
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	nsp::tcpip::endpoint ep;
	auto iter = map_endpoint_.find(ip);
	if (iter == map_endpoint_.end()){
		return ep;
	}
	ep = iter->second;
	return ep;
}
