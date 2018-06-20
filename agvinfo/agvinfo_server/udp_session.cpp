#include "udp_session.h"
#include "agv_shell_proto.hpp"
#include "agv_shell_define.h"
#include "udp_manager.h"
#include "common_data.h"
#include "argvs.h"

#include <algorithm>

udp_session::udp_session()
{

}

udp_session::~udp_session()
{

}

void udp_session::on_recvdata(const std::string &data, const nsp::tcpip::endpoint &r_ep)
{
	nsp::proto::agv_shell_proto_head agv_sh_head;
	int cb = data.size();
	if (!agv_sh_head.build((const unsigned char *)data.data(), cb)) {
		this->close();
		return;
	}

	if (agv_sh_head.op != 0xC || agv_sh_head.fn != 'C' || agv_sh_head.ln != 'K') {
		loerror(MODULE_NAME) << "udp server get an invalid protocol head specify.";
		return;
	}

	int type = agv_sh_head.type;
	switch (type) {
	case kAgvShellProto_LocalInfo:
		recv_mac_info(nsp::proto::shared_for<nsp::proto::proto_local_info>(data.c_str(), data.size()), r_ep.ipv4());
		break;
	default:
		break;
	}
}

int udp_session::recv_mac_info(const std::shared_ptr<nsp::proto::proto_local_info>& data, const std::string& ipv4)
{
	robot_mac_info m_info;
	m_info.mac_addr = data->mac_addr_;
	std::transform(m_info.mac_addr.begin(), m_info.mac_addr.end(), m_info.mac_addr.begin(), ::toupper);
	m_info.shell_port = data->agv_port_;
	m_info.fts_port = data->fts_port_;
	m_info.ipv4 = ipv4;
	//收到包则直接将该计数至为0
	m_info.pkt_timeout_count = 0;
	nsp::toolkit::singleton<udp_manager>::instance()->save_mac_info(m_info);
	return 0;
}