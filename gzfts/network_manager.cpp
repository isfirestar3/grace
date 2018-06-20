#include "network_manager.h"


net_manager::net_manager()
{
}


net_manager::~net_manager()
{

}

uint32_t net_manager::pkt_id()
{
	return ++pkt_id_;
}

int net_manager::write(const uint32_t pkt_id, const std::function<int()>& my_write, const std::shared_ptr<asio_block>&asio){
	return asio_manage_.write(pkt_id, asio, my_write);
}

int net_manager::exec(const int pkt_id, const void*data){
	return asio_manage_.exec(pkt_id, data);
}
