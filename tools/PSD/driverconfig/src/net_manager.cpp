#include "net_manager.h"


net_manager::net_manager()
{
}


net_manager::~net_manager()
{

}

uint16_t net_manager::pkt_id()
{
	return ++pkt_id_;
}


int net_manager::write(uint32_t pkt_id, const std::function<int()> &sender, const std::shared_ptr<motion::asio_block>&asio_parnet)
{
	return asio_manager_.write(pkt_id, asio_parnet, sender);
}

int net_manager::write_data_forward(uint32_t cmd, const std::function<int()> &sender, const std::shared_ptr<motion::asio_block>&asio_parnet)
{
	return asio_manager_.write_data_forward(cmd, asio_parnet, sender);
}

int net_manager::exec(uint32_t pkt_id, const char* byte_stream)
{
	return asio_manager_.exec(pkt_id, byte_stream);
}

int net_manager::exec_data_forward(uint32_t cmd, const char* byte_stream)
{
	return asio_manager_.exec_data_forward(cmd, byte_stream);
}

int net_manager::exec_message(uint8_t cmd, const char * byte_stream)
{
	return asio_manager_.exec_message(cmd,byte_stream);
}

int net_manager::post_message(uint8_t cmd, const std::function<int()> &sender, const std::shared_ptr<motion::asio_block>&asio_parnet)
{
	return asio_manager_.post_message(cmd, asio_parnet, sender);
}