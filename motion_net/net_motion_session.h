#pragma once
#include <atomic>
#include <thread>
#include "application_network_framwork.hpp"
#include "old.hpp"
#include "vehicle.h"
#include "vartypes.h"
#include "navigation.h"
#include "operation.h"
#include "usrdef.h"
#include "asio.hpp"
#include "proto_typedef.h"
#include "proto_common.h"
#include "mntypes.h"
#include "proto_pre_login.h"
#include "proto_dbg_varls.h"
#include "proto_navigation_task.h"
#include "proto_navigation_task_status_ack.h"
#include "proto_navigation_task_traj_ack.h"
#include "proto_add_navigation_task_traj_ack.h"
#include "proto_control_navi_task_status_ack.h"
#include "proto_common_ack.h"
#include "proto_pre_login.h"
#include "proto_common_report.h"
#include "proto_task_opera_parm.h"
#include "os_util.hpp"
#include "proto_canio_msg.h"
#include "proto_operator_task_action_ack.h"
#include "proto_task_status.h"
#include "packet_pool.h"

namespace mn {
	class  net_motion_udp_session : public nsp::tcpip::obudp
	{
		int initialized_ = 0;
		uint32_t pktid_ = 0;
		std::recursive_mutex mutex_locker_;
		// read from xml file,format: 
		// <motion_net>
		//		<map ipv4="xxx.xxx.xxx.aaa" tcp_port="4409" udp_port="4409" />
		//		<map ipv4="xxx.xxx.xxx.bbb" tcp_port="4409" udp_port="4409" />
		// </motion_net>
		std::map<nsp::tcpip::endpoint, nsp::tcpip::endpoint> map_xml_udp_ep_;
		// <robotid, pktid>
		std::map<uint32_t, uint32_t> map_robotid_pktid_;
		std::map<uint32_t, uint32_t> map_pktid_robotid_;
	public:
		net_motion_udp_session()  {};
		~net_motion_udp_session() {};

	public:
		// init udp session
		int init_session();
		// send a udp alive packet
		int send_alive_packet(uint32_t robot_id, nsp::tcpip::endpoint &mt_ep, nsp::tcpip::endpoint &mn_ep);
		void stop_keep_alive(uint32_t robot_id);
	protected:
		// receive udp packet from server 
		virtual void on_recvdata(const std::string &data, const nsp::tcpip::endpoint &r_ep)override final;

	private:
		//  receive keep alive packet 
		int recv_keep_alive(const unsigned char *buffer, int &cb);
	};

	class net_motion_session : public nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol> {
		packet_pool pool_;
		nsp::tcpip::endpoint ep_;
		nsp__controlor_type_t type_;
		std::string encrypt_key_;
		std::atomic<net_status_t> net_status_ { kNetworkStatus_Closed };
		int robot_id_ = 0;
		posix__boolean_t enable_keepalive_ = posix__false;

		// record time for the last packet arrived
		// this value should be use by timeout check.
		std::atomic<uint64_t> tcp_pkt_timed_ { 0 };
		std::atomic<uint64_t> udp_pkt_timed_ { 0 };
		std::atomic<int> login_error_ = { 0 };

		mutable std::recursive_mutex lock_notify_change_;
		notify_t notify_ = nullptr;
		net_motion_udp_session *udp_sub_session;

		int check_connection_status() const;
		void notify_it(const void *data, enum notift_type_t type);
		
		template<class T>
		int build(const unsigned char *pos, int &cb, std::shared_ptr<T> &body) {
			try {
				body = std::make_shared<T>();
				if (body->build(pos, cb) < 0 ) {
					return -1;
				}
			}catch(...) {
				return -1;
			}
			return 0;
		}

		int recv_sync_connect_ack(const unsigned char *buffer, int &cb );
		int recv_login_ack( const unsigned char *buffer, int &cb );
		int recv_navigation_operation_ack( const unsigned char *buffer, int &cb);
		int recv_navigation_status_ack( const unsigned char *buffer, int &cb );
		int recv_navigation_traj_ack( const unsigned char *buffer, int &cb );
		int recv_varls_ack( const unsigned char *buffer, int &cb);
		int recv_comm_read_requst_asio( const unsigned char *buffer, int &cb );
		int recv_common_status_report( const unsigned char *buffer, int &cb );
		int recv_canio_msg( const unsigned char *buffer, int &cb );
		int recv_allocate_operation_task_ack( const unsigned char *buffer, int &cb );
		int recv_operation_task_status_ack( const unsigned char *buffer, int &cb );
		int recv_normal_complete( const unsigned char *buffer, int &cb );
		int recv_periodic_report( const unsigned char *buffer, int &cb );
		void recv_dispatch(const unsigned char *buffer, int &cb );
		int recv_mtver(const unsigned char *buffer, int &cb );

	public:
		net_motion_session(int robot_id);
		net_motion_session( HTCPLINK lnk, int robot_id );
		~net_motion_session();
		virtual void on_disconnected( const HTCPLINK previous ) override final;
		virtual void on_recvdata( const std::string &pkt ) override final;
		virtual void on_connected() override final;
		
	public:
		void register_notify_callback( const notify_t& to_notify );
		const nsp::tcpip::endpoint &getep() const;
		void setep( const nsp::tcpip::endpoint &ep );
		const nsp__controlor_type_t gettype() const;
		void settype( const nsp__controlor_type_t &type );
		void get_status(net_status_t &net_stat, int &login_err) const;
		void set_status(net_status_t net_stat, int login_err);
		void enable_keepalive();
		int check_timeout(uint64_t tick);
		void per_close();
		void get_notify(notify_t &func) const;
		// set the last udp packet arrived time
		void set_udp_alive();
	public:
		int try_connect();
		int try_login();
		void login_notify(uint32_t robot_id, const void *data);
		int keepalive();
		int post_navigation_task( uint64_t task_id, const upl_t &dest_upl, const position_t &dest_pos, int is_traj_whole, const std::vector<trail_t> &vec_trail, const std::shared_ptr<asio_partnet> &asio );
		int post_navigation_cancle_task( uint64_t task_id, const std::shared_ptr<asio_partnet> &asio );
		int post_navigation_pause_task( uint64_t task_id, const std::shared_ptr<asio_partnet> &asio );
		int post_navigation_resume_task( uint64_t task_id, const std::shared_ptr<asio_partnet> &asio );
		int query_task_opera_parm_order( const std::shared_ptr<asio_partnet> &asio );
		int query_navigation_task_detail_status( uint64_t task_id, const std::shared_ptr<asio_partnet> &asio );
		int query_navigation_task_traj( const std::shared_ptr<asio_partnet> &asio );
		int post_add_navigation_task_traj( uint64_t task_id, uint32_t index, const std::vector<trail_t> &vec_trail, int is_traj_whole, const std::shared_ptr<asio_partnet> &asio );
		int post_common_read_request_by_id( const struct common_title &read_data, const std::shared_ptr<asio_partnet> &asio );
		int post_common_write_request_by_id( const struct common_data &write_data, const std::shared_ptr<asio_partnet> &asio );
		int post_clear_fault_request( const std::shared_ptr<asio_partnet> &asio );
		int post_dbg_varls_request( const std::shared_ptr<asio_partnet> &asio );
		int post_dbg_segmentfault(const std::shared_ptr<asio_partnet> &asio);
		int post_query_mtver( const std::shared_ptr<asio_partnet> &asio );
		int post_common_compare_write_request( const struct common_compare_write& compare_wirte, const std::shared_ptr<asio_partnet>&asio );
		int post_report_status_order( uint64_t task_id, uint32_t vartype_, uint32_t status, const std::string &usr_msg, const std::shared_ptr<asio_partnet>&asio );
		int post_allocate_operation_task( uint64_t task_id, int32_t task_code, uint64_t task_params[10], const std::shared_ptr<asio_partnet> &asio );
		int post_operation_cancle_task( uint64_t operator_task_id, const std::shared_ptr<asio_partnet> &asio );
		int post_operation_pause_task( uint64_t operator_task_id, const std::shared_ptr<asio_partnet> &asio );
		int post_operation_resume_task( uint64_t operator_task_id, const std::shared_ptr<asio_partnet> &asio );
		int registry_periodic_report(const struct periodic_title &read_periodic_data, const std::shared_ptr<asio_partnet> &asio);
		int unregistry_periodic_report(const std::shared_ptr<asio_partnet> &asio);
	};
};
