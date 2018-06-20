#if !defined MOTION_NET_INTERFACES_H
#define MOTION_NET_INTERFACES_H

#include "vartypes.h"
#include "mntypes.h"

#include <functional>
#include <vector>

#if !defined EXP
	#if _WIN32
		#if defined _USRDLL
			#define METH dllexport
		#else
			#define METH dllimport
		#endif // !_USRDLL
		#define EXP(type)  extern "C" _declspec(METH) type __stdcall
	#else
		#define EXP(type)  extern "C" type
	#endif // !_WIN32
#endif // !EXP

#if __cplusplus > 0
namespace mn {
#endif

	////////////////////网络接口/////////////////////////////////
	EXP( int ) init_net();
	EXP( int ) login_to_host( uint32_t robot_id, const char *epstr, nsp__controlor_type_t type );
	EXP( int ) register_callback_to_notify( uint32_t robot_id, const notify_t& notify );
	EXP( int ) disconnect_host( uint32_t robot_id );
	EXP( int ) change_connect_host(uint32_t robot_id, const char *epstr);
	EXP( void ) reset_net();

	//////////////////导航任务相关接口////////////////////////////
	//发送任务, 返回 task_status
	EXP( int ) post_navigation_task(
		uint32_t robot_id, uint64_t task_id, const upl_t &dest_upl, const position_t &dest_pos, int is_traj_whole, const std::vector<trail_t> &vec_trail,
		apc_t &apc );
	//查询导航任务状态,返回 navigation_task_status_ack
	EXP( int ) query_navigation_task_status( uint32_t robot_id, uint64_t task_id, apc_t &apc );
	//查询当前任务参考轨迹,返回 recv_nav_task_traj
	EXP( int ) query_navigation_task_traj( uint32_t robot_id, apc_t &apc );
	//增加导航边,返回 asio_t
	EXP( int ) post_add_navigation_task_traj(
		uint32_t robot_id, uint64_t task_id, uint32_t index, const std::vector<trail_t> &vec_trail, int is_traj_whole,
		apc_t &apc);
	//取消任务, 返回 task_status
	EXP( int ) post_navigation_cancel_task( uint32_t robot_id, uint64_t task_id, apc_t &apc );
	//暂停任务, 返回 task_status
	EXP( int ) post_navigation_pause_task( uint32_t robot_id, uint64_t task_id, apc_t &apc );
	//恢复任务, 返回 task_status
	EXP( int ) post_navigation_resume_task( uint32_t robot_id, uint64_t task_id, apc_t &apc );
	//查询导航任务参数,返回 asio_t
	EXP( int ) query_task_opera_parm_order( uint32_t robot_id, apc_t &apc );
	//主动获取状态,返回 asio_t
	EXP( int ) post_report_status_order(
		uint32_t robot_id, uint64_t task_id, uint32_t vartype, uint32_t status, const std::string &usr_msg,
		apc_t &apc );

	//////////////////操作任务相关接口////////////////////////////
	//操作任务, 返回 task_status_t
	EXP( int ) post_allocate_operation_task(
		uint32_t robot_id, uint64_t task_id, int32_t task_code, uint64_t task_params[10],
		apc_t &apc );
	//取消操作任务, 返回 task_status
	EXP( int ) post_operation_cancle_task( uint32_t robot_id, uint64_t task_id, apc_t &apc );
	//暂停操作任务, 返回 task_status
	EXP( int ) post_operation_pause_task( uint32_t robot_id, uint64_t task_id, apc_t &apc );
	//恢复操作任务, 返回 task_status
	EXP( int ) post_operation_resume_task( uint32_t robot_id, uint64_t task_id, apc_t &apc );

	/////////////////////通用接口////////////////////////////////
	// 通用读, 返回 common_data
	EXP( int ) post_common_read_request_by_id( uint32_t robot_id, const struct common_title &read_data, apc_t &apc );
	// 通用写， 返回 asio_t
	EXP( int ) post_common_write_request_by_id( uint32_t robot_id, const struct common_data &write_data, apc_t &apc );
	EXP( int ) post_common_compare_write_request( uint32_t robot_id, const struct common_compare_write &compare_write_data, apc_t &apc );
	// 注册周期性上报, 返回 periodic_data, 周期性进行 @apc
	EXP(int) registry_periodic_report(uint32_t robot_id,const struct periodic_title &read_periodic_data,apc_t &apc);
	EXP(int) unregistry_periodic_report(uint32_t robot_id,apc_t &apc);
	// 清除错误，返回 asio_t
	EXP( int ) post_clear_fault_request( uint32_t robot_id, apc_t &apc );
	// 查询当前所有var的概要列表， 返回 var_list
	EXP( int ) post_dbg_varls_request( uint32_t robot_id, apc_t &apc );
	// query current running motion-template program version, return @mtver
	EXP(int) post_query_mtver(uint32_t robot_id, apc_t &apc);
	// kill mt process
	EXP(int) post_dbg_segmentfault(uint32_t robot_id, apc_t &apc);

#if __cplusplus > 0
}
#endif

#endif // !MOTION_NET_INTERFACES_H