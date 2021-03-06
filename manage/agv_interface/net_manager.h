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
	uint32_t pkt_id();
	uint32_t get_robot_id();

	//管理异步对象
	int   write(uint32_t pkt_id, const std::function<int()> &sender, const std::shared_ptr<motion::asio_block>&asio_parnet);
	int   move_asio_object(uint32_t pkt_id, std::shared_ptr<motion::asio_block> &sptr);
	int   exec(uint32_t pkt_id, const char* byte_stream);
	void  build_error_notify(uint32_t pkt_id);

private:
	uint32_t pkt_id_ = 0;
	uint32_t robot_id_ = 0;
};

