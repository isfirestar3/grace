#include <memory>
#include <atomic>
#include <queue>
#include "net_impls.h"
#include "os_util.hpp"
#include "log.h"
#include "proto_definion.h"
#include "proto_typedef.h"
#include "net_motion_session.h"
#include "net_manager.h"
#include "singleton.hpp"
#include "asio.hpp"

#define OPERA_PARM_SIZE (10)

namespace mn {

	static int init_tcp_provider(int set_id) {
		uint32_t robot_id;
		if (set_id == 0) {
			robot_id = nsp::toolkit::singleton<net_manager>::instance()->robot_id();
		}
		else {
			robot_id = set_id;
		}

		std::shared_ptr<net_motion_session> net_session;
		try {
			net_session = std::make_shared<net_motion_session>(robot_id);
		} catch ( std::bad_alloc& ) {
			return -1;
		}

		if ( net_session ) {
			nsp::toolkit::singleton<net_manager>::instance()->insert( robot_id, net_session );
			return robot_id;
		}
		return -1;
	}

	static int init_udp_provider() {
		return nsp::toolkit::singleton<net_manager>::instance()->init_udp_session();
	}

	EXP( int ) init_net() {
		int retval = -1;

		retval = init_udp_provider();
		if (retval < 0) {
			return -1;
		}

		retval = init_tcp_provider(0);
		if ( retval < 0 ) {
			return -1;
		}

		return retval;
	}

	EXP( int )login_to_host( uint32_t robot_id, const char *epstr, nsp__controlor_type_t type ) {

		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -1;
		
		nsp::tcpip::endpoint ep;
		if (nsp::tcpip::endpoint::build(epstr, ep) < 0 ){
			return -EINVAL;
		}

		net_session->setep( ep );
		net_session->settype( type );

		if ( net_session->try_connect() < 0 ) {
			return -1;
		}

		net_status_t net_stat;
		int err = 0;
		// force login robot synchronous in 5 seconds.
		for (int i = 0; i < 50; i++) {
			net_session->get_status(net_stat, err);
			if (kNetworkStatus_Ready == net_stat) {
				if (net_session->try_login() < 0) {
					return -1;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}
			if (err < 0) {
				// login failed(ACK packet is timeout or error from server)
				return -1;
			}
			if (kNetworkStatus_Established == net_stat) {
				// login successful
				net_session->enable_keepalive();
				return 0;
			}
			// other status, wait 100ms for retry 
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		// login failed, after trying 50 times
		return -1;
	}

	EXP( int ) disconnect_host( uint32_t robot_id ) {
		return nsp::toolkit::singleton<net_manager>::instance()->remove( robot_id );
	}

	EXP( int ) change_connect_host(uint32_t robot_id, const char *epstr) {
		int retval = 0;
		nsp__controlor_type_t controlor_type;

		// get robot_id's session.
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search(robot_id);
		if (!net_session) {
			return -1;
		}
		controlor_type = net_session->gettype();
		notify_t notify;
		net_session->get_notify(notify);

		// remove old session.
		retval = nsp::toolkit::singleton<net_manager>::instance()->remove(robot_id);
		if (retval < 0) {
			return -2;
		}
		// create new session and set robot_id.
		retval = init_tcp_provider(robot_id);
		if (retval < 0) {
			return -3;
		}
		register_callback_to_notify(robot_id, notify);

		// login to host and set controlor_type.
		retval = login_to_host(robot_id, epstr, controlor_type);
		if (retval < 0) {
			return -4;
		}

		return retval;
	}

	EXP( void ) reset_net() {
		nsp::toolkit::singleton<net_manager>::instance()->cleanall();
		nsp::toolkit::singleton<net_manager>::instance()->end_keepalive();
		nsp::toolkit::singleton<net_manager>::instance()->end_timecheck();
		nsp::toolkit::singleton<net_manager>::release();
	}

	EXP( int ) register_callback_to_notify( uint32_t robot_id, const notify_t& notify ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) {
			return -ENOENT;
		}

		net_session->register_notify_callback( notify );
		return 0;
	}

	EXP( int ) post_navigation_task( 
		uint32_t robot_id, uint64_t task_id, const upl_t &dest_upl, const position_t &dest_pos, int is_traj_whole, const std::vector<trail_t> &vec_trail,
		apc_t &apc) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		try {
			std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
			return net_session->post_navigation_task( task_id, dest_upl, dest_pos, is_traj_whole, vec_trail, partnet );
		} catch ( ... ) {
			return -ENOMEM;
		}
	}

	EXP( int ) query_navigation_task_status( uint32_t robot_id, uint64_t task_id, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		try {
			std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
			return net_session->query_navigation_task_detail_status( task_id, partnet );
		} catch ( ... ) {
			return -ENOMEM;
		}
	}

	EXP( int ) query_navigation_task_traj( uint32_t robot_id, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->query_navigation_task_traj( partnet );
	}

	EXP( int ) post_add_navigation_task_traj(
		uint32_t robot_id, uint64_t task_id, uint32_t index, const std::vector<trail_t> &vec_trail, int is_traj_whole, 
		apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_add_navigation_task_traj( task_id, index, vec_trail, is_traj_whole, partnet );
	}

	EXP( int ) post_navigation_cancel_task( uint32_t robot_id, uint64_t task_id, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_navigation_cancle_task( task_id, partnet );
	}

	EXP( int ) post_navigation_pause_task( uint32_t robot_id, uint64_t task_id, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_navigation_pause_task( task_id, partnet );
	}

	EXP( int ) post_navigation_resume_task( uint32_t robot_id, uint64_t task_id, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_navigation_resume_task( task_id, partnet );
	}

	EXP( int ) query_task_opera_parm_order( uint32_t robot_id, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->query_task_opera_parm_order( partnet );
	}

	EXP( int ) post_report_status_order( 
		uint32_t robot_id, uint64_t task_id, uint32_t vartype, uint32_t status, const std::string &usr_msg, 
		apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_report_status_order( task_id, vartype, status, usr_msg, partnet );
	}

	EXP( int ) post_allocate_operation_task( 
		uint32_t robot_id, uint64_t task_id, int32_t task_code, uint64_t task_params[10], 
		apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_allocate_operation_task( task_id, task_code, task_params, partnet );
	}

	EXP( int ) post_operation_cancle_task( uint32_t robot_id, uint64_t task_id, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_operation_cancle_task( task_id, partnet );
	}

	EXP( int ) post_operation_pause_task( uint32_t robot_id, uint64_t task_id, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_operation_pause_task( task_id, partnet );
	}

	EXP( int ) post_operation_resume_task( uint32_t robot_id, uint64_t task_id,  apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_operation_resume_task( task_id, partnet );
	}

	EXP( int ) post_common_read_request_by_id( uint32_t robot_id, const struct common_title &read_data, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_common_read_request_by_id( read_data, partnet );
	}

	EXP( int ) post_common_write_request_by_id( uint32_t robot_id, const struct common_data &write_data, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_common_write_request_by_id( write_data, partnet );
	}

	EXP( int ) post_common_compare_write_request( uint32_t robot_id, const struct common_compare_write& comm_compare, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_common_compare_write_request( comm_compare, partnet );
	}
	
	EXP(int) registry_periodic_report(uint32_t robot_id, const struct periodic_title &read_periodic_data,apc_t &apc) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;
		
		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->registry_periodic_report( read_periodic_data, partnet );
	}

	EXP(int) unregistry_periodic_report(uint32_t robot_id, apc_t &apc) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;
		
		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->unregistry_periodic_report( partnet );
	}

	EXP( int ) post_clear_fault_request( uint32_t robot_id, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_clear_fault_request( partnet );
	}

	EXP( int ) post_dbg_varls_request( uint32_t robot_id, apc_t &apc ) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_dbg_varls_request( partnet );
	}

	EXP( int ) post_query_mtver(uint32_t robot_id, apc_t &apc) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_query_mtver( partnet );
	}

	EXP(int) post_dbg_segmentfault(uint32_t robot_id, apc_t &apc) {
		std::shared_ptr<net_motion_session> net_session = nsp::toolkit::singleton<net_manager>::instance()->search( robot_id );
		if ( !net_session ) return -ENOENT;

		std::shared_ptr<asio_partnet> partnet = std::make_shared<asio_partnet>( robot_id, apc );
		return net_session->post_dbg_segmentfault( partnet );
	}
}
