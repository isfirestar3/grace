#include "dhcp_udp_session.h"
#include "agv_shell_common.h"
#include "agv_shell_define.h"
#include "log.h"
#include "sys_info.h"

dhcp_udp_session::dhcp_udp_session(){

}

dhcp_udp_session::~dhcp_udp_session(){

}

void dhcp_udp_session::on_recvdata(const std::string &pkt, const nsp::tcpip::endpoint &r_ep){
	nsp::proto::agv_shell_proto_head agv_sh_head;
    int cb = pkt.size();
    if (!agv_sh_head.build((const unsigned char *) pkt.data(), cb)) {
        this->close();
        return;
    }

    if (agv_sh_head.op != 0xC || agv_sh_head.fn != 'C' || agv_sh_head.ln != 'K') {
        loerror("agv_shell") << "invalid protocol head specify.";
        this->close();
        return;
    }

	int type = agv_sh_head.type;
    switch (type) {
		case kAgvShellProto_LocalInfo_ACK:
			dispatch_mac_addr();
			break;
		default:
            this->close();
            break;
    }
}

void dhcp_udp_session::dispatch_mac_addr(){
	loerror("agv_shell") << "fix ip receive local info.";
	return;
}

int dhcp_udp_session::psend(const nsp::proto::proto_interface& package, const nsp::tcpip::endpoint& ep){
	return obudp::sendto(package.length(), [&](void*buffer, int cb)->int{
		return (package.serialize((unsigned char*)buffer) < 0) ? -1 : 0;
	}, ep);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
dhcp_udp_client_manager::dhcp_udp_client_manager(){
	
}

dhcp_udp_client_manager::~dhcp_udp_client_manager(){
	clean_session();
}

int dhcp_udp_client_manager::init_network(){
	if (dhcp_session_){
		loerror("agv_shell") << "udp network session is already exists.";
		return 0;
	}
	try{
		dhcp_session_ = std::make_shared<dhcp_udp_session>();
	}
	catch (...){
		return -1;
	}
	if (dhcp_session_){
		if (dhcp_session_->create(0) < 0){
			loerror("agv_shell") << "failed to create dhcp udp session object.";
			return -1;
		}
		
		sys_info t_sys_info;
		m_mac_addr_ = t_sys_info.get_mac_addr();
		return 0;
	}
	
	return -1;
}

void dhcp_udp_client_manager::clean_session(){
	if (dhcp_session_){
		dhcp_session_->close();
	}
	dhcp_session_ = nullptr;
}

int dhcp_udp_client_manager::post_local_info_request() {
	nsp::proto::proto_local_info pkt(kAgvShellProto_LocalInfo);
	pkt.agv_port_ = nsp::toolkit::singleton<global_parameter>::instance()->get_server_port();
	pkt.fts_port_ = nsp::toolkit::singleton<global_parameter>::instance()->get_fts_port();
	pkt.mac_addr_ = m_mac_addr_;
	
	return dhcp_session_->psend(pkt, dhcp_fix_ep_);
}

