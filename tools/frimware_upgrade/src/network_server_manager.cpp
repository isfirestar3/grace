#include "network_server_manager.h"
#include "endpoint.h"
#include "log.h"

network_server_manager::network_server_manager(){

}

network_server_manager::~network_server_manager(){

}

int network_server_manager::listen(const std::string& ep_str){
	nsp::tcpip::endpoint ep;
	if (nsp::tcpip::endpoint::build(ep_str, ep) < 0){
		loerror("udp_net") << "failed to build endpoint,the input string is " << ep_str;
		return -1;
	}
	if (!server_session_ptr_){
		server_session_ptr_ = std::make_shared<network_session>();
	}
	if (server_session_ptr_->create(ep, UDP_FLAG_BROADCAST) < 0){
		loerror("udp_net") << "failed to create udp listen,the endpoint is " << ep.to_string();
		return -1;
	}
	loinfo("udp_net") << "create udp listen successfully.";
	return 0;
}

void network_server_manager::close_udp(){
	if (server_session_ptr_){
		server_session_ptr_->close();
	}
}