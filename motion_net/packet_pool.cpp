#include "packet_pool.h"
#include "log.h"
#include "net_manager.h"
#include <errno.h>

namespace mn {

	packet_pool::packet_pool( int robot ) {
		robot_id_ = robot;
	}

	packet_pool::~packet_pool() {
		cleanall();
	}

	void packet_pool::cleanall() {
		std::lock_guard<decltype( lock_pooled_sequence_ )> guard( lock_pooled_sequence_ );
		auto iter = pooled_packet_sequence_.begin();
		while ( pooled_packet_sequence_.end() != iter ) {
			iter->second->commit_asio_error( -ECONNRESET );
			iter = pooled_packet_sequence_.erase( iter );
		}
	}

	uint32_t packet_pool::get_pktid() {
		uint32_t id;
		do {
			id = ++pkt_id_;
		} while ( id == periodic_pkt_id_ );

		return id;
	}

	uint32_t packet_pool::get_periodic_pktid() {
		periodic_pkt_id_ = ++pkt_id_;
		return periodic_pkt_id_;
	}

	void packet_pool::periodic_pktid_cancel() {
		periodic_pkt_id_ = 0;
	}

	int packet_pool::queue_packet( uint32_t pktid, const std::shared_ptr<asio_partnet> &asio, const std::function<int()> &sender ) {
		if ( robot_id_ < 0 || !sender ) {
			return -EINVAL;
		}

		std::pair<std::map<int, std::shared_ptr<asio_partnet>>::iterator, bool> pair_added;

		// 先行插入管理序列
		std::lock_guard<decltype(lock_pooled_sequence_)> guard( this->lock_pooled_sequence_ );
		pair_added = pooled_packet_sequence_.insert( std::pair<int, std::shared_ptr<asio_partnet>>( pktid, asio ) );
		if ( !pair_added.second ) {
			return -1;
		}

		// 尝试对该包执行投递操作, 如果直接失败， 则需要立即将该包移除管理序列
		if ( sender() < 0 ) {
			pooled_packet_sequence_.erase( pair_added.first );
			return -1;
		}

		return 0;
	}
	
	int packet_pool::async_complete( uint32_t pktid, uint32_t type, const char *completion_data ) {
		std::shared_ptr<asio_partnet> asio;
		posix__boolean_t timeo = posix__false;

		{
			std::lock_guard<decltype(lock_pooled_sequence_)> guard( this->lock_pooled_sequence_ );
			auto iter = pooled_packet_sequence_.find( pktid );
			if ( pooled_packet_sequence_.end() != iter ) {
				asio = iter->second;
				pooled_packet_sequence_.erase( iter );
				uint64_t current_clock = nsp::os::clock_gettime();
				timeo = asio->is_timedout(pktid, current_clock);
				if (timeo) {
					// these logs means packet has been marked to timedout by async_complete.
					mnlog_warn << "asyn receive timed check fatal,id=" << pktid << " current=" << current_clock;
				}
			} else {
				// these logs means request packet have a response,but can not be found in pool queue.
				mnlog_warn << "asyn completion packet no found. id=" << pktid << " ack type=" << type;
				return -ENOENT;
			}
		}
		
		if (asio) {
			timeo ? asio->commit_asio_error(-ETIMEDOUT) : asio->exec( completion_data );
		}
		return 0;
	}

	void packet_pool::check_timeout(uint64_t tick) {
		std::lock_guard<decltype( lock_pooled_sequence_ )> guard( lock_pooled_sequence_ );
		auto iter = pooled_packet_sequence_.begin();
		while ( pooled_packet_sequence_.end() != iter ) {
			if ( iter->second->is_timedout( iter->first, tick ) ) {
				asio_t asio;
				asio.err_ = -ETIMEDOUT;
				nsp::toolkit::singleton<net_manager>::instance()->schedule_timeout_callback(
					robot_id_, (void *)&asio, sizeof(asio), iter->second->get_notify_fun());
				iter = pooled_packet_sequence_.erase( iter );
			} else {
				++iter;
			}
		}
	}
}