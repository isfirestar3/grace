#pragma once

#include "singleton.hpp"
#include "asio_manage.h"
#include <memory>
#include <mutex>
#include <atomic>
#include <queue>

class net_manager
{
private:
	net_manager();
	~net_manager();
	friend class nsp::toolkit::singleton<net_manager>;


private:
	asio_manage asio_manager_;

public:

	//分配包id
	uint16_t  pkt_id();

	//管理异步对象
	int   write(uint16_t pkt_id, const std::function<int()> &sender, const std::shared_ptr<motion::asio_block>&asio_parnet);
	int   move_asio_object(uint16_t pkt_id, std::shared_ptr<motion::asio_block> &sptr);
	int   exec(uint16_t pkt_id, const char* byte_stream);
	void  build_error_notify(uint16_t pkt_id);

private:
	uint16_t pkt_id_ = 0;
};

