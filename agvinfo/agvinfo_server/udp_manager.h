#pragma once

#include "singleton.hpp"
#include "udp_session.h"
#include "common_data.h"
#include "os_util.hpp"
#include <memory>
#include <thread>

class udp_manager{
	udp_manager();
	~udp_manager();
	friend class nsp::toolkit::singleton<udp_manager>;

public:
	int udp_listen(const std::string& ipv4, const uint16_t port);
	void save_mac_info(const robot_mac_info& m_info);
	void get_mac_all(std::vector<robot_mac_info>& vct_info);

private:
	void check_timeout();
	void initlizatin();
	void uinit();

private:
	std::shared_ptr<udp_session> share_session_;

	std::recursive_mutex r_mutex_;
	//key:MAC Address,value:struct
	std::multimap<std::string, robot_mac_info> map_mac_info_;

	nsp::os::waitable_handle wait_check_;
	std::thread* check_th_ = nullptr;
	std::atomic<bool> exit_th_{ false };

};
