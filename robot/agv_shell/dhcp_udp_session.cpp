#include "dhcp_udp_session.h"
#include "agv_shell_define.h"
#include "log.h"
#include "proto_agv_msg.h"
#include "proto_definion.h"

dhcp_udp_session::dhcp_udp_session(){

}

dhcp_udp_session::~dhcp_udp_session(){

}

void dhcp_udp_session::on_recvdata(const std::string &pkt, const nsp::tcpip::endpoint &r_ep){
	nsp::proto::proto_head agv_sh_head;
    int cb = pkt.size();
    if (!agv_sh_head.build((const unsigned char *) pkt.data(), cb)) {
        this->close();
        return;
    }

	int type = agv_sh_head.type_;
    switch (type) {
		case PKTTYPE_AGV_SHELL_GET_IP_MAC_ACK:
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

int dhcp_udp_session::post_local_info_request(int shell_port, int port, const std::string& mac, const nsp::tcpip::endpoint& ep) {
	nsp::proto::proto_local_info pkt(PKTTYPE_AGV_SHELL_GET_IP_MAC);
	pkt.agv_port_ = shell_port;
	pkt.fts_port_ = port;
	pkt.mac_addr_ = mac;
	
	return obudp::sendto(pkt.length(), [&](void* buffer, int cb)->int{
		return (pkt.serialize((unsigned char*)buffer) < 0) ? -1 : 0;
	}, ep);
}
