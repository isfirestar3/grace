#pragma once

#include "singleton.hpp"
#include "asio_manage.h"

class net_manager
{
private:
	net_manager();
	~net_manager();
	friend class nsp::toolkit::singleton<net_manager>;

public:
	//·ÖÅä°üid
	uint32_t  pkt_id();
	int write(const uint32_t pkt_id, const std::function<int()>& my_write, const std::shared_ptr<asio_block>&asio);
	int exec(const int pkt_id, const void*data);

private:
	uint32_t pkt_id_ = 0;
	asio_manage asio_manage_;
};

