#pragma once

#include "singleton.hpp"
#include "proto_typedef.h"
#include <memory>
#include <mutex>
#include <atomic>
#include "net_motion_session.h"
#include <queue>

#include <functional>
#include "task_scheduler.hpp"

namespace mn {

	enum task_type_t{
		kTaskType_Unknown = 0,	// unknown type
		kTaskType_Recv,			// recv packet
		kTaskType_Timeout		// packet timeout
	};
	class mn_task_t {
		task_type_t task_type_ = kTaskType_Unknown;
		std::function<void(const unsigned char *, int &)> recv_handler = nullptr;
		std::function<void(uint32_t, const void *)> notify_fun_ = nullptr;
		unsigned char *data_ = nullptr;
		int cb_ = 0;
		uint32_t robot_id_ = 0;

	public:
		mn_task_t(const unsigned char *data, int cb, const std::function<void(const unsigned char *, int &)> &handler);
		mn_task_t(uint32_t robot_id, const unsigned char *data, int cb, const std::function<void(uint32_t, const void *)> &handler);
		mn_task_t(const mn_task_t &lref);
		mn_task_t(mn_task_t &&rref);
		~mn_task_t();
		void on_task();
	};

	class net_manager {

		// thread pool for package receiver and dispacth
		nsp::toolkit::task_thread_pool<mn_task_t>	mn_task_pool_;

		std::atomic<uint32_t> robot_id_ { 0 };

		std::recursive_mutex mutex_net_session_;
		std::map<uint32_t, std::shared_ptr<net_motion_session>> map_net_session_; //map<robot_id, session>
		std::shared_ptr<net_motion_udp_session> udp_session;
		nsp::os::waitable_handle keepalive_waiter_;
		std::thread * th_keepalive_ = nullptr;

		nsp::os::waitable_handle timecheck_waiter_;
		std::thread * th_timecheck_ = nullptr;
		
		net_manager();
		~net_manager();
		friend class nsp::toolkit::singleton<net_manager>;

		// 心跳线程
		void keepalive();
		int init_keepalive();

		// 超时检查线程
		void check_timeout();
		int init_timecheck();

	public:
		//分配机器人id
		uint32_t robot_id();
		//初始化udp session
		int init_udp_session();
		//管理网络对象
		int insert( uint32_t robot_id, std::shared_ptr<net_motion_session> &pseesion );
		std::shared_ptr<net_motion_session> search( uint32_t robot_id );
		int remove( uint32_t robot_id );
		void cleanall();
		int set_status(uint32_t robot_id, net_status_t net_stat, int login_err);

		//心跳线程 
		void end_keepalive();

		// 超时检查
		void end_timecheck();

		// asynchronous transfer.
		void schedule_async_receive(const std::string &pkt, const std::function<void(const unsigned char *,int &)> &handler);
		// call the callback_function, when packet is time out 
		void schedule_timeout_callback(const uint32_t robot_id, const void *data, int cb, const std::function<void(uint32_t, const void *)> &handler);
		// send a alive packet.
		int send_alive_packet(uint32_t robot_id, nsp::tcpip::endpoint &mt_ep, nsp::tcpip::endpoint &mn_ep);
		int set_alive_session(uint32_t robot_id);
	};
};
