#include "net_interface.h"
#include "DataManage.h"
#include "log.h"
#include "proto_common_report.h"
#include "proto_typedef.h"
#include <chrono>
//#include "net_impls.h"

using namespace mn;

#include <atomic>

static std::atomic<uint64_t>	navigation_task_id{ 0xFFFF0000 };


net_interface::net_interface() 
	:agv_base(kVehicleAgv_Type_PickingAgv)
{
}


net_interface::~net_interface() {
	int i = 0;
}

int net_interface::connect_net() {
	_robot_id = init_net();
	return 0;
}
int net_interface::ready(bool& ready)
{
	ready = true;
	return 0;
}
int net_interface::goto_charge(int dock_id, int charge_on, uint64_t& task_id, std::function<void(uint64_t taskid, status_describe_t status, int err, void* user)> fn, void* user /*= nullptr*/)
{
	return 0;
}
int net_interface::get_elongate_variable()
{
	return 0;
}
void net_interface::get_elongate_variable_ack(uint32_t id, const void *data)
{

}
int net_interface::net_login( const std::string& ip, int port ) {
	//std::shared_ptr<agv_base> agv_ptr;
	set_agv_id(robot_id_);
	if (login(ip, port) < 0)
	{
		nsperror << "Agv: id=" << robot_id_ << " ip=" << ip << " port=" << port << " login_to_host failed!" ;
		return -1;
	}
	//nsp::tcpip::endpoint endpoint( ip.c_str(), port );
	//if ( login_to_host( _robot_id, endpoint, kControlorType_Dispatcher ) < 0 ) {
	//	nsperror << "login_to_host failed!ip : " << ip << ", port : " << port;
	//	return -1;
	//}
	nsperror << "login_to_host success!";
	std::lock_guard<decltype( mutex_vehicle_ip )> guard( mutex_vehicle_ip );
	vehicle_ip = ip;
	nsp::os::waitable_handle time_wait;
	auto start = std::chrono::system_clock::now();
	bool rd = false;
	while (!rd) 
	{
		int a = sizeof(var__navigation_t);
		update_agv_data();

		is_ready(rd);
		time_wait.wait(1);
		auto end = std::chrono::system_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
		double duration_ = double(duration.count()) * std::chrono::seconds::period::num / std::chrono::seconds::period::den;

		if (duration_ > 5)
		{
			break;
		}
	}
	return 0;
}

void net_interface::set_callback_notify() {
	register_callback_to_notify( _robot_id, std::bind( &net_interface::callback_rec_data, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ) );
}

int net_interface::get_opt_param( OPTINFO& opt_info ) {
	int ready = query_opt_param_ready();
	if ( ready != QUERY_SUCCESS ) {
		return ready;
	}
	return query_opt_param( opt_info );
}

int net_interface::query_opt_param_ready() {
	
	return query_task_opera_parm(_robot_id);
	//asio_t *asio;
	//nsp::os::waitable_handle waiter;
	//int retval = -1;
	//int ret = query_task_opera_parm_order( _robot_id, [&]( uint32_t robot_id, const void *data ) {
	//	if ( !data ) {
	//		waiter.sig();
	//		return;
	//	}
	//	asio = ( asio_t* )data;
	//	retval = asio->err_;
	//	waiter.sig();
	//} );
	//if ( ret < 0 ) {//如果接口调用失败，那么直接返回
	//	return retval;
	//}
	//waiter.wait();
	//if ( retval < 0 )//无数据可获取
	//{
	//	return READY_DATA_FAILED;
	//}
	//int wait_complete = wait_param_complete.wait( 2000 );
	//if ( wait_complete == 1 )//等待数据超时
	//{
	//	return QUERY_OUT_TIME;
	//}
	//return QUERY_SUCCESS;
}

int net_interface::query_opt_param( OPTINFO& opt_info ) {

	var__operation_parameter_t opt_param;
	nsp::os::waitable_handle water;
	do
	{
		if (agv_base::get_var_info_by_id_syn(kVarFixedObject_OptPar, opt_param) >= 0)
		{
			break;
		}
		water.wait(100);
	} while (1);

	opt_info.params[0] = opt_param.ull00_;
	opt_info.params[1] = opt_param.ull01_;
	opt_info.params[2] = opt_param.ull02_;
	opt_info.params[3] = opt_param.ull03_;
	opt_info.params[4] = opt_param.ull04_;
	opt_info.params[5] = opt_param.ull05_;
	opt_info.params[6] = opt_param.ull06_;
	opt_info.params[7] = opt_param.ull07_;
	opt_info.params[8] = opt_param.ull08_;
	opt_info.params[9] = opt_param.ull09_;
	opt_info.params[10] = opt_param.ull10_;
	opt_info.params[11] = opt_param.ull11_;
	opt_info.params[12] = opt_param.ull12_;
	opt_info.params[13] = opt_param.ull13_;
	opt_info.params[14] = opt_param.ull14_;
	opt_info.params[15] = opt_param.ull15_;
	opt_info.params[16] = opt_param.ull16_;
	opt_info.params[17] = opt_param.ull17_;
	opt_info.params[18] = opt_param.ull18_;
	opt_info.params[19] = opt_param.ull19_;

	//common_title_item title_item;
	//title_item.varid = kVarFixedObject_OptPar;
	//title_item.offset = 0;
	//title_item.length = sizeof( var__operation_parameter_t );
	//common_title title;
	//title.items.push_back( title_item );
	//int retval = 0;
	//nsp::os::waitable_handle wait_request;
	//common_data* asio_data;
	//int ret = post_common_read_request_by_id( _robot_id, title, [&]( uint32_t robot_id, const void *data ) {
	//	if ( !data ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	asio_data = ( common_data* )data;
	//	retval = asio_data->err_;
	//	if ( retval < 0 ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	if ( asio_data->items.size() == 0 ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	var__operation_parameter_t opt_param = *( var__operation_parameter_t* )asio_data->items[0].data.data();
	//	opt_info.params[0] = opt_param.ull00_;
	//	opt_info.params[1] = opt_param.ull01_;
	//	opt_info.params[2] = opt_param.ull02_;
	//	opt_info.params[3] = opt_param.ull03_;
	//	opt_info.params[4] = opt_param.ull04_;
	//	opt_info.params[5] = opt_param.ull05_;
	//	opt_info.params[6] = opt_param.ull06_;
	//	opt_info.params[7] = opt_param.ull07_;
	//	opt_info.params[8] = opt_param.ull08_;
	//	opt_info.params[9] = opt_param.ull09_;
	//	opt_info.params[10] = opt_param.ull10_;
	//	opt_info.params[11] = opt_param.ull11_;
	//	opt_info.params[12] = opt_param.ull12_;
	//	opt_info.params[13] = opt_param.ull13_;
	//	opt_info.params[14] = opt_param.ull14_;
	//	opt_info.params[15] = opt_param.ull15_;
	//	opt_info.params[16] = opt_param.ull16_;
	//	opt_info.params[17] = opt_param.ull17_;
	//	opt_info.params[18] = opt_param.ull18_;
	//	opt_info.params[19] = opt_param.ull19_;
	//	wait_request.sig();
	//} );
	//if ( ret < 0 ) {//如果接口调用失败，那么直接返回
	//	return retval;
	//}
	//wait_request.wait();
	//if ( retval < 0 ) // 获取数据失败
	//{
	//	return QUERY_DATA_FAILED;
	//}
	return QUERY_SUCCESS;
}

void net_interface::callback_rec_data( int32_t robot_id, const void *data, int type ) {
	switch ( type ) {
		//case kNAVI_TASK_STATUS_ACK:
		//	{				  
		//	}
		//	break;
	case KNAVI_POS:
	{
	}
		break;
	case kTASK_STATUS_REPORT:
	{
								nsp::proto::proto_common_report_t* report_data = (nsp::proto::proto_common_report_t* )( data );
								if ( report_data ) {
									if ( report_data->var_type_ == VAR_TYPE_OPERATION && report_data->status_ == kStatusDescribe_Completed ) {
										wait_param_complete.sig();//唤醒可查询事件
									}
									if ( report_data->var_type_ == VAR_TYPE_NAVIGATION && report_data->status_ == kStatusDescribe_Completed ) {
										DATAMNG->SetTaskStatus( kStatusDescribe_Completed );
									}
								}
	}
		break;
	default:
		break;
	}
}

int net_interface::get_vehicle_pos( POSXYA& posXya ) {
	//update_agv_data();
	position_t pos;
	double confident;
	int ret=get_current_pos(pos,confident);
	if (ret < 0){
		return QUERY_DATA_FAILED;
	}
	posXya.pos_x = DataConvert::world2map((pos).x_);
	posXya.pos_y = DataConvert::world2map((pos).y_);
	posXya.angle = DataConvert::radian2angle((pos).angle_);
	std::lock_guard<decltype(mutex_vehicle_ip)> guard(mutex_vehicle_ip);
	if (!DATAMNG->SetVehiclePos(vehicle_ip, posXya)) {
		nspinfo << "SetVehiclePos failed,vehicle_ip:" << vehicle_ip;
	}	
	return QUERY_SUCCESS;

	//common_title_item title_item;
	//title_item.varid = kVarFixedObject_Navigation;
	//title_item.offset = offsetof( var__navigation_t, pos_ );
	//title_item.length = sizeof( position_t );
	//common_title title;
	//title.items.push_back( title_item );
	//int retval = 0;
	//nsp::os::waitable_handle wait_request;
	//common_data* asio_data;
	//int ret = post_common_read_request_by_id( _robot_id, title, [&]( uint32_t robot_id, const void *data ) {
	//	if ( !data ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	asio_data = ( common_data* )data;
	//	retval = asio_data->err_;
	//	if ( retval < 0 ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	if ( 0 == asio_data->items.size() ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	position_t *pos = ( position_t* )( asio_data->items[0].data.data() );
	//	if ( pos ) {
	//		posXya.pos_x = DataConvert::world2map( ( *pos ).x_ );
	//		posXya.pos_y = DataConvert::world2map( ( *pos ).y_ );
	//		posXya.angle = DataConvert::radian2angle( ( *pos ).angle_ );
	//		std::lock_guard<decltype( mutex_vehicle_ip )> guard( mutex_vehicle_ip );
	//		if ( !DATAMNG->SetVehiclePos( vehicle_ip, posXya ) ) {
	//			nspinfo << "SetVehiclePos failed,vehicle_ip:" << vehicle_ip;
	//		}
	//	}
	//	retval = QUERY_SUCCESS;
	//	wait_request.sig();
	//} );
	//if ( ret < 0 ) {//如果接口调用失败，那么直接返回
	//	return retval;
	//}
	//wait_request.wait();
	//return retval;
}

int net_interface::get_vehicle_pos_asyn() {
	//update_agv_data();//此接口一次获取了多个数据，这里只需要navigation的
	get_var_info_by_id_asyn<var__navigation_t>(kVarFixedObject_Navigation);

	//var__navigation_t t;//同步获取navigation数据
	//get_var_info_by_id_syn<var__navigation_t>(kVarFixedObject_Navigation,t);
	callback_rev_pos();
	return 0;
	/*common_title_item title_item;
	title_item.varid = kVarFixedObject_Navigation;
	title_item.offset = offsetof( var__navigation_t, pos_ );
	title_item.length = sizeof( position_t );
	common_title title;
	title.items.push_back( title_item );
	post_common_read_request_by_id( _robot_id, title,
		std::bind( &net_interface::callback_rev_pos, this, std::placeholders::_1, std::placeholders::_2 ) );
	return QUERY_SUCCESS;;*/
}

void net_interface::callback_rev_pos() {
	position_t pos;
	double confident;
	int ret = get_current_pos(pos, confident);

	POSXYA posXya;
	posXya.pos_x = DataConvert::world2map((pos).x_);
	posXya.pos_y = DataConvert::world2map((pos).y_);
	posXya.angle = DataConvert::radian2angle((pos).angle_);
	std::lock_guard<decltype(mutex_vehicle_ip)> guard(mutex_vehicle_ip);
	if (!DATAMNG->SetVehiclePos(vehicle_ip, posXya)) {
		nspinfo << "SetVehiclePos failed,vehicle_ip:" << vehicle_ip;
	}

	/*common_data* asio_data;
	if ( !data ) {
		return;
	}
	asio_data = ( common_data * )data;
	if ( asio_data->err_ < 0 || asio_data->items.size() == 0 ) {
		return;
	}

	position_t *pos = ( position_t* )( asio_data->items[0].data.data() );
	if ( pos ) {
		POSXYA posXya;
		posXya.pos_x = DataConvert::world2map( ( *pos ).x_ );
		posXya.pos_y = DataConvert::world2map( ( *pos ).y_ );
		posXya.angle = DataConvert::radian2angle( ( *pos ).angle_ );
		std::lock_guard<decltype( mutex_vehicle_ip )> guard( mutex_vehicle_ip );
		if ( !DATAMNG->SetVehiclePos( vehicle_ip, posXya ) ) {
			nspinfo << "SetVehiclePos failed,vehicle_ip:" << vehicle_ip;
		}
	}*/
}

int net_interface::get_vehicle_upl( UPL& upl ) {
	//update_agv_data();
	upl_t curUpl;
	int ret=get_current_upl(curUpl);
	if (ret<0){
		return QUERY_DATA_FAILED;
	}
	upl.edge_id = curUpl.edge_id_;
	upl.wop_id = curUpl.wop_id_;
	upl.percent = curUpl.percentage_;
	upl.aoa = curUpl.angle_;
	std::lock_guard<decltype(mutex_vehicle_ip)> guard(mutex_vehicle_ip);
	if (!DATAMNG->SetVehicleUpl(vehicle_ip, upl)) {
		nspinfo << "SetVehicleUpl failed,vehicle_ip:" << vehicle_ip;
	}
	return QUERY_SUCCESS;

	//common_title_item title_item;
	//title_item.varid = kVarFixedObject_Navigation;
	//title_item.offset = offsetof( var__navigation_t, i.upl_  );
	//title_item.length = sizeof( upl_t );
	//common_title title;
	//title.items.push_back( title_item );
	//int retval = QUERY_DATA_FAILED;
	//nsp::os::waitable_handle wait_request;
	//common_data* asio_data;
	//int ret = post_common_read_request_by_id( _robot_id, title, [&]( uint32_t robot_id, const void *data ) {
	//	if ( !data ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	asio_data = ( common_data* )data;
	//	retval = asio_data->err_;
	//	if ( retval < 0 || 0 == asio_data->items.size()) {
	//		wait_request.sig();
	//		return;
	//	}
	//	upl_t* upl_ask = ( upl_t* )( asio_data->items[0].data.data() );
	//	if ( upl_ask ) {
	//		upl.edge_id = upl_ask->edge_id_;
	//		upl.wop_id = upl_ask->wop_id_;
	//		upl.percent = upl_ask->percentage_;
	//		upl.aoa = upl_ask->angle_;
	//		std::lock_guard<decltype( mutex_vehicle_ip )> guard( mutex_vehicle_ip );
	//		if ( !DATAMNG->SetVehicleUpl( vehicle_ip, upl ) ) {
	//			nspinfo << "SetVehicleUpl failed,vehicle_ip:" << vehicle_ip;
	//		}
	//	}
	//	retval = QUERY_SUCCESS;
	//	wait_request.sig();
	//} );
	//if ( ret < 0 ) {//如果接口调用失败，那么直接返回
	//	return retval;
	//}
	//wait_request.wait();
	//return retval;
}

int net_interface::get_nav_info(var__navigation_t& nav)
{
	return agv_base::get_nav_info(nav);
}

int net_interface::post_nav_task( const upl_t &dest_upl, const position_t &dest_pos, const std::vector<trail_t> &vec_trail ) {
	//on_upl_task( dest_upl, dest_pos,1, vec_trail);
	//return 0;

	return on_upl_nav_task(dest_upl, dest_pos, 1, vec_trail);

	//int retval = 0;
	//nsp::os::waitable_handle wait_request;
	//task_status_t* asio_data;
	//int ret = post_navigation_task( _robot_id, navigation_task_id, dest_upl, dest_pos, 1, vec_trail,
	//	[&]( uint32_t robot_id, const void *data ) {
	//	if ( !data ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	asio_data = ( task_status_t* )data;
	//	if ( asio_data->err_ < 0 ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	task_status_t* status = ( task_status_t* )data;
	//	if ( !status ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	if ( status->status_ < 0 ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	wait_request.sig();
	//} );
	//if ( ret < 0 ) {//如果接口调用失败，那么直接返回
	//	return retval;
	//}
	//wait_request.wait();
	//if ( retval != QUERY_SUCCESS ) {
	//	nspinfo << "post_navigation_task failed,dest_upl:" << dest_upl.edge_id_;
	//}
	//return retval;
}

int net_interface::post_cancel_nav_task() {
	int ret = cancel_nav_syn();
	if (ret<0){
		return QUERY_DATA_FAILED;
	}
	return QUERY_SUCCESS;

	//int retval;
	//nsp::os::waitable_handle wait_request;
	//task_status_t* asio_data;
	//int ret = post_navigation_cancel_task( _robot_id, navigation_task_id, [&]( uint32_t robot_id, const void *data ) {
	//	if ( !data ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	asio_data = ( task_status_t* )data;
	//	retval = asio_data->err_;
	//	if ( retval < 0 ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	task_status_t* status = ( task_status_t* )data;
	//	if ( !status ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	if ( status->status_ < 0 ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	retval = QUERY_SUCCESS;
	//	wait_request.sig();
	//} );
	//if ( ret < 0 ) {//如果接口调用失败，那么直接返回
	//	return retval;
	//}
	//wait_request.wait();
	//if ( retval != QUERY_SUCCESS ) {
	//	nspinfo << "post_cancel_nav_task failed.";
	//}
	//return retval;
}

int net_interface::post_suspend_nav_task() {
	int ret = pause_nav_syn();
	if (ret<0){
		return QUERY_DATA_FAILED;
	}
	return QUERY_SUCCESS;

	//int retval;
	//nsp::os::waitable_handle wait_request;
	//task_status_t* asio_data;
	//int ret = post_navigation_pause_task( _robot_id, navigation_task_id,[&]( uint32_t robot_id, const void *data ) {
	//	if ( !data ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	asio_data = ( task_status_t* )data;
	//	retval = asio_data->err_;
	//	if ( retval < 0 ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	task_status_t* status = ( task_status_t* )data;
	//	if ( !status ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	if ( status->status_ < 0 ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	retval = QUERY_SUCCESS;
	//	wait_request.sig();
	//} );
	//if ( ret < 0 ) {//如果接口调用失败，那么直接返回
	//	return retval;
	//}
	//wait_request.wait();
	//if ( retval != QUERY_SUCCESS ) {
	//	nspinfo << "post_suspend_nav_task failed.";
	//}
	//return retval;
}

int net_interface::post_resume_nav_task() {
	int ret = resume_nav_syn();
	if (ret<0){
		return QUERY_DATA_FAILED;
	}
	return QUERY_SUCCESS;

	//int retval;
	//nsp::os::waitable_handle wait_request;
	//task_status_t* asio_data;
	//int ret = post_navigation_resume_task( _robot_id, navigation_task_id,[&]( uint32_t robot_id, const void *data ) {
	//	if ( !data ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	asio_data = ( task_status_t* )data;
	//	retval = asio_data->err_;
	//	if ( retval < 0 ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	task_status_t* status = ( task_status_t* )data;
	//	if ( !status ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	if ( status->status_ < 0 ) {
	//		wait_request.sig();
	//		return;
	//	}
	//	retval = QUERY_SUCCESS;
	//	wait_request.sig();
	//});
	//if ( ret < 0 ) {//如果接口调用失败，那么直接返回
	//	return retval;
	//}
	//wait_request.wait();
	//if ( retval != QUERY_SUCCESS ) {
	//	nspinfo << "post_resume_nav_task failed.";
	//}
	//return retval;
}