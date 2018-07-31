#include "dhcp_session.h"
#include "net_manager.h"
#include "dhcp_server_base.h"

mac_session::mac_session()
{

}

mac_session::mac_session(HTCPLINK link) : tcp_application_client(link) {

}

mac_session::~mac_session()
{
	this->close();
}

void mac_session::on_disconnected(const HTCPLINK previous)
{
	this->net_status_ = mn::kNetworkStatus_Closed;
	loinfo("agv_interface") << "mac server session client disconnect, the link is " << previous;
}

void mac_session::on_recvdata(const std::string &pkt)
{
	std::shared_ptr<nsp::proto::agv_shell_proto_head> head = nsp::proto::shared_for<nsp::proto::agv_shell_proto_head>(pkt.c_str(),
		nsp::proto::agv_shell_proto_head::type_length());

	if (!head) {
		loerror("agv_shell_interface") << "agv shell session ger an error head package.";
		close();
		return;
	}
	uint8_t ty = head->type;
	if (dispatch_package(ty, pkt)<0) {
		close();
	}
}

void mac_session::on_connected()
{
	this->net_status_ = mn::kNetworkStatus_Connected;
}

int mac_session::dispatch_package(const uint8_t package_type, const std::string &pkt_data)
{
	int retval = 0;
	switch (package_type)
	{
	case kAgvShellProto_LocalInfo_ACK:
		retval = recv_mac_info_rsp(nsp::proto::shared_for<nsp::proto::proto_mac_vec>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_MacNotify:
		retval = recv_mac_notification();
		break;
	default:
		loerror("agv_interface") << "mac server client session get an invaild type package,so close this session link.";
		retval = -1;
		break;
	}
	return retval;
}

int mac_session::try_connect(const nsp::tcpip::endpoint& ep)
{
	mn::net_status_t exp = mn::kNetworkStatus_Closed;
	if (net_status_.compare_exchange_strong(exp, mn::kNetworkStatus_Actived))
	{
		if (this->create() < 0)
		{
			net_status_ = mn::kNetworkStatus_Closed;
			this->close();
			return -1;
		}
	}

	exp = mn::kNetworkStatus_Actived;
	if (net_status_.compare_exchange_strong(exp, mn::kNetworkStatus_Connecting))
	{
		if (connect(ep) < 0)
		{
			loerror("agv_interface") << "mac server client session failed to call ns API connect.this link will be destory.the target endpoint is " << ep.to_string() << " the link is " << this->lnk_;
			this->close();
			return -1;
		}
		loinfo("agv_interface") << "mac server client session success connect to target host " << ep.to_string();
	}
	return 0;
}

int mac_session::post_mac_info_req(const std::shared_ptr<motion::asio_block>&asio_ack)
{
	if (mn::kNetworkStatus_Connected != this->net_status_)
	{
		return -1;
	}
	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::proto_msg_int pkt(kAgvShellProto_LocalInfo);
	pkt.msg_int_ = pkt_id;
	return nsp::toolkit::singleton<net_manager>::instance()->write(pkt_id, [&]()->int{
		return psend(&pkt);
	}, asio_ack);
}

int mac_session::recv_mac_info_rsp(const std::shared_ptr<nsp::proto::proto_mac_vec>& data)
{
	mac_info_list m_list;
	m_list.set_err(nsp::proto::errorno_t::kSuccessful);
	for (const auto& iter : data->vct_mac_)
	{
		mac_address_info tmp;
		tmp.fts_port = iter.fts_port_;
		tmp.ipv4 = iter.ipv4_;
		tmp.mac_addr = iter.mac_addr_;
		tmp.shell_port = iter.agv_port_;
		m_list.vct_mac.push_back(tmp);
	}
	return nsp::toolkit::singleton<net_manager>::instance()->exec(data->pkt_id, (char *)&m_list);
}

int mac_session::recv_mac_notification()
{
	if (!dhcp_notify_spool_)
	{
		loerror("agv_interface") << "the DHCP notification spool is empty.";
		return 0;
	}
	std::shared_ptr<dhcp_notify> task = nullptr;
	try {
		task = std::make_shared<dhcp_notify>(dhcp_notification_);
	}
	catch (...)
	{
		loerror("agv_interface") << "failed to make shared task of dhcp notification.";
		return 0;
	}
	dhcp_notify_spool_->post(task);
	return 0;
}

void mac_session::set_dhcp_callback(void(STDCALL* func)(), const std::shared_ptr<nsp::toolkit::task_thread_pool<dhcp_notify>>& ptr)
{
	dhcp_notification_ = func;
	dhcp_notify_spool_ = ptr;
}