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
	friend class nsp::toolkit::singleton<net_manager>;     //��Ԫ���ڵ����๹�캯�� ʵ�ֲ���ֱ�Ӵ�����

private:
	asio_manage asio_manager_;

public:

	//�����id
	uint16_t  pkt_id();

	//�����첽����
	int   write(uint32_t pkt_id, const std::function<int()> &sender, const std::shared_ptr<motion::asio_block>&asio_parnet);
	int   write_data_forward(uint32_t cmd, const std::function<int()> &sender, const std::shared_ptr<motion::asio_block>&asio_parnet);
	int   exec(uint32_t pkt_id, const char* byte_stream);
	int   exec_data_forward(uint32_t cmd, const char* byte_stream);
	int   exec_message(uint8_t cmd, const char * byte_stream);
	int   post_message(uint8_t cmd, const std::function<int()> &sender, const std::shared_ptr<motion::asio_block>&asio_parnet);
private:
	uint16_t pkt_id_ = 0;
};
