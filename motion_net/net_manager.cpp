#include "net_manager.h"

#include "os_util.hpp"

#include <iostream>

namespace mn {

	mn_task_t::mn_task_t(const unsigned char *data, int cb, const std::function<void(const unsigned char *, int &)> &handler)
	{
		task_type_ = kTaskType_Recv;
		if (data && cb > 0 && handler) {
			try {
				data_ = new unsigned char[cb];
				memcpy(data_,data, cb);
				cb_ = cb;
				recv_handler = handler;
			}catch(...) {
				;
			}
		}
	}

	mn_task_t::mn_task_t(uint32_t robot_id, const unsigned char *data, int cb, const std::function<void(uint32_t, const void *)> &handler)
	{
		task_type_ = kTaskType_Timeout;
		robot_id_ = robot_id;
		if (data && cb > 0 && handler) {
			try {
				data_ = new unsigned char[cb];
				memcpy(data_, data, cb);
				cb_ = cb;
				notify_fun_ = handler;
			}
			catch (...) {
				;
			}
		}
	}

	mn_task_t::~mn_task_t()
	{
		if (data_ && cb_ > 0 ) {
			delete []data_;
			data_ = nullptr;
			cb_ = 0;
		}	
	}

	mn_task_t::mn_task_t(const mn_task_t &lref)
	{
		task_type_ = lref.task_type_;
		if (lref.data_ && lref.cb_ > 0) {
			try {
				cb_ = lref.cb_;
				data_ = new unsigned char[cb_];
				memcpy(data_,lref.data_,cb_);
			} catch(...) {
				;
			}
		}
		robot_id_ = lref.robot_id_;
		recv_handler = lref.recv_handler;
		notify_fun_ = lref.notify_fun_;
	}

	mn_task_t::mn_task_t(mn_task_t &&rref)
	{
		task_type_ = rref.task_type_;
		data_ = rref.data_;
		cb_ = rref.cb_;
		rref.data_ = nullptr;
		rref.cb_ = 0;
		robot_id_ = rref.robot_id_;
		recv_handler = rref.recv_handler;
		notify_fun_ = rref.notify_fun_;
	}

	void mn_task_t::on_task()
	{
		if (kTaskType_Recv == task_type_) {
			int oricb = cb_;
			if (recv_handler) {
				recv_handler(data_, oricb);
			}
		}
		else if (kTaskType_Timeout == task_type_) {
			if (notify_fun_) {
				notify_fun_(robot_id_, (const void *)data_);
			}
		}
	}

	/////////////////////////////////////////////////////////////// net_manager ///////////////////////////////////////////////////////////////
	net_manager::net_manager() {
		init_keepalive();
		init_timecheck();
		mn_task_pool_.allocate(nsp::os::getnprocs());
	}

	net_manager::~net_manager() {
		mn_task_pool_.join();
	}

	uint32_t net_manager::robot_id() {
		return ++robot_id_;
	}

	int net_manager::init_udp_session() {
		try {
			udp_session = std::make_shared<net_motion_udp_session>();
		} catch (std::bad_alloc&) {
			return -1;
		}
		return udp_session->init_session();
	}

	int net_manager::insert( uint32_t robot_id, std::shared_ptr<net_motion_session> &pseesion ) {
		std::lock_guard<decltype( mutex_net_session_ ) > guard( mutex_net_session_ );
		map_net_session_[robot_id] = pseesion;
		return 0;
	}

	std::shared_ptr<net_motion_session> net_manager::search( uint32_t robot_id ) {
		std::lock_guard<decltype( mutex_net_session_ ) > guard( mutex_net_session_ );
		auto iter = map_net_session_.find( robot_id );
		if ( iter == map_net_session_.end() ) {
			return nullptr;
		}
		return iter->second;
	}

	int net_manager::remove( uint32_t robot_id ) {
		std::shared_ptr<net_motion_session> sptr;

		{
			std::lock_guard<decltype( mutex_net_session_ ) > guard( mutex_net_session_ );
			auto iter = map_net_session_.find( robot_id );
			if ( iter == map_net_session_.end() ) {
				return -ENOENT;
			}

			sptr = iter->second;
			map_net_session_.erase( iter );
		}

		// notify the session object that it will be close soon
		// it need to cleanup all cached package immediately and exchange the status to @kNetworkStatus_Closed. 
		// in this way, calling thread can get all pending request callback before 'disconnect host' implement interface returned.
		sptr->per_close();

		// clear event notify pointer if existed.
		// after 'close' call, EVT_TCP_CLOSED event must be handle by session
		// at this situation, if calling thread free the object who owned callback function, it may cause the application crash.
		sptr->register_notify_callback(nullptr);
		sptr->close();

		if (udp_session) {
			udp_session->stop_keep_alive(robot_id);
		}
		return 0;
	}

	void net_manager::cleanall() {
		std::lock_guard<decltype( mutex_net_session_ ) > guard( mutex_net_session_ );
		map_net_session_.clear();
	}

	int net_manager::set_status(uint32_t robot_id, net_status_t net_stat, int login_err) {
		std::lock_guard<decltype(mutex_net_session_) > guard(mutex_net_session_);
		auto iter = map_net_session_.find(robot_id);
		if (iter == map_net_session_.end()) {
			return -1;
		}

		iter->second->set_status(net_stat, login_err);
		return 0;
	}

	int net_manager::init_keepalive() {
		try {
			if ( !th_keepalive_ ) {
				th_keepalive_ = new std::thread( std::bind( &net_manager::keepalive, this ) );
			}
		} catch ( ... ) {
			return -ENOMEM;
		}
		return 0;
	}

	int net_manager::init_timecheck() {
		try {
			if ( !th_timecheck_ ) {
				th_timecheck_ = new std::thread( std::bind( &net_manager::check_timeout, this ) );
			}
		} catch ( ... ) {
			return -ENOMEM;
		}
		return 0;
	}

	void net_manager::keepalive() {
		while ( keepalive_waiter_.wait( 1000 ) > 0 ) {
			std::lock_guard<decltype( mutex_net_session_ ) > guard( mutex_net_session_ );
			auto iter = map_net_session_.begin();
			while ( map_net_session_.end() != iter ) {
				if ( iter->second ) {
					iter->second->keepalive();
					++iter;
				} else {
					iter = map_net_session_.erase( iter );
				}
			}
		}
	}

	void net_manager::check_timeout() {
		uint64_t tick;
		while ( timecheck_waiter_.wait( 50 ) > 0 ) {
			tick = nsp::os::clock_gettime();
			std::lock_guard<decltype( mutex_net_session_ ) > guard( mutex_net_session_ );
			auto iter = map_net_session_.begin();
			while ( map_net_session_.end() != iter ) {
				if ( iter->second ) {
					// is it necessary to destroy the session when the timeout occurs?
					if (iter->second->check_timeout(tick) < 0 ) {
						// iter = map_net_session_.erase( iter );
						;
					}else{
						++iter;
					}
				} else {
					iter = map_net_session_.erase( iter );
				}
			}
		}
	}

	void net_manager::end_keepalive() {
		keepalive_waiter_.sig();
		if ( th_keepalive_ ) {
			if ( th_keepalive_->joinable() ) {
				th_keepalive_->join();
				delete th_keepalive_;
				th_keepalive_ = nullptr;
			}
		}
	}

	void net_manager::end_timecheck() {
		timecheck_waiter_.sig();
		if ( this->th_timecheck_ ) {
			if ( this->th_timecheck_->joinable() ) {
				th_timecheck_->join();
				delete th_timecheck_;
				th_timecheck_ = nullptr;
			}
		}
	}

	void net_manager::schedule_async_receive(const std::string &pkt, const std::function<void(const unsigned char *,int &)> &handler) {
		try {
			std::shared_ptr<mn_task_t> sptr = std::make_shared<mn_task_t>((const unsigned char *)pkt.data(), pkt.size(),handler);
			mn_task_pool_.post(sptr);
		}catch(...){
			;
		}
	}
	void net_manager::schedule_timeout_callback(const uint32_t robot_id, const void *data, int cb, const std::function<void(uint32_t, const void *)> &handler) {
		try {
			std::shared_ptr<mn_task_t> sptr = std::make_shared<mn_task_t>(robot_id, (unsigned char *)data, cb, handler);
			mn_task_pool_.post(sptr);
		}
		catch (...){
			;
		}
	}

	int net_manager::send_alive_packet(uint32_t robot_id, nsp::tcpip::endpoint &mt_ep, nsp::tcpip::endpoint &mn_ep) {
		return udp_session->send_alive_packet(robot_id, mt_ep, mn_ep);
	}

	int net_manager::set_alive_session(uint32_t robot_id) {
		std::lock_guard<decltype(mutex_net_session_) > guard(mutex_net_session_);
		auto iter = map_net_session_.find(robot_id);
		if (iter == map_net_session_.end()) {
			return -ENOENT;
		}
		iter->second->set_udp_alive();
		return 0;
	}
}
