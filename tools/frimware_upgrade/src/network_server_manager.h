#pragma once

#include "network_session.h"
#include "singleton.hpp"

class network_server_manager{

	network_server_manager();
	~network_server_manager();
	friend class nsp::toolkit::singleton<network_server_manager>;

public:
	int listen(const std::string& ep_str);
	void close_udp();

private:
	std::shared_ptr<network_session> server_session_ptr_;

};