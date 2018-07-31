#include "dhcp_server_base.h"
#include "dhcp_session.h"
#include "task_scheduler.hpp"
#include "dhcp_notify.hpp"

#define NET_WINDOW_SIZE 1

static
std::shared_ptr<nsp::toolkit::task_thread_pool<dhcp_notify>> notify_spool_ = nullptr;

dhcp_server_base::dhcp_server_base()
{
	
}

dhcp_server_base::~dhcp_server_base()
{
	disconnect_dhcp_server();
}

int dhcp_server_base::login_dhcp_server(const std::string& ipv4, const uint16_t port)
{
	nsp::tcpip::endpoint ep;
	char epstr[32];
	sprintf(epstr, "%s:%u", ipv4.c_str(), port);
	if (nsp::tcpip::endpoint::build(epstr, ep) < 0) {
		return -EINVAL;
	}
	if (client_session_)
	{
		loerror("agv_interface") << "the mac server client session is already exist,so can not make client session.";
		return -1;
	}
	std::shared_ptr<mac_session> mac_sess;
	try
	{
		mac_sess = std::make_shared<mac_session>();
	}
	catch (...)
	{
		loerror("agv_interface") << "failed to make share ptr of connect to mac server client session";
		return -1;
	}
	if (mac_sess->try_connect(ep) < 0)
	{
		return -1;
	}
	if (!notify_spool_)
	{
		//注册线程池，实际可以单独写一个线程类，此处为了方便套用框架内线程池操作
		notify_spool_ = std::make_shared<nsp::toolkit::task_thread_pool<dhcp_notify>>(NET_WINDOW_SIZE);
	}
	client_session_ = std::dynamic_pointer_cast<nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>>(mac_sess);
	return 0;
}

void dhcp_server_base::disconnect_dhcp_server()
{
	if (client_session_)client_session_->close();
}

int dhcp_server_base::get_dhcp_addr_all(std::vector<mac_address_info>& vct_shell)
{
	//清除用户输入的变量
	vct_shell.clear();
	if (!client_session_) return -1;
	motion::asio_data a_data;
	nsp::os::waitable_handle waiter(0);
	mac_info_list list_info;
	std::shared_ptr<mac_session> client = std::static_pointer_cast<mac_session>(client_session_);
	if (mn::kNetworkStatus_Closed == client->get_network_status())
	{
		return -2;
	}
	int res = client->post_mac_info_req(std::make_shared<motion::asio_block>([&](const void*data){
		if (!data){
			waiter.sig();
			return;
		}
		a_data = *(motion::asio_data*)data;
		if (a_data.get_err() < 0){
			waiter.sig();
			return;
		}
		list_info = *(mac_info_list*)data;
		waiter.sig();
	}));
	if (res < 0){
		loerror("agv_interface") << "failed to post get mac address list package.";
		return -1;
	}
	waiter.wait();
	if (a_data.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		for (const auto& iter : list_info.vct_mac)
		{
			mac_address_info mac;
			mac.fts_port = iter.fts_port;
			mac.ipv4 = iter.ipv4;
			mac.mac_addr = iter.mac_addr;
			mac.shell_port = iter.shell_port;
			vct_shell.push_back(mac);
		}
		return 0;
	}
	return -1;
}

int dhcp_server_base::regist_dhcp_notify(void(STDCALL* func)())
{
	if (!client_session_) return -1;
	std::shared_ptr<mac_session> client = std::static_pointer_cast<mac_session>(client_session_);
	client->set_dhcp_callback(func, notify_spool_);
	return 0;
}
