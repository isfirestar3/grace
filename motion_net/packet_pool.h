#if !defined PACKET_POOL_H
#define PACKET_POOL_H

#include <atomic>
#include <map>
#include <functional>
#include <mutex>
#include <memory>

#include <stdint.h>
#include "asio.hpp"

namespace mn {
	class packet_pool {
		std::atomic<uint32_t> pkt_id_ { 0 };
		std::atomic<uint32_t> periodic_pkt_id_ { 0 };
		int robot_id_ = -1;
		std::recursive_mutex	lock_pooled_sequence_;
		std::map<int, std::shared_ptr<asio_partnet>> pooled_packet_sequence_;
		
	public:
		packet_pool(int robot);
		~packet_pool();
		uint32_t get_pktid();
		uint32_t get_periodic_pktid();
		void periodic_pktid_cancel();
		int queue_packet(uint32_t pktid,const std::shared_ptr<asio_partnet> &asio,const std::function<int()> &sender ); 
		int async_complete(uint32_t pktid, uint32_t type, const char *completion_data);
		void check_timeout(uint64_t tick);
		void cleanall();
	}; 
};

#endif