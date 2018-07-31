#include <memory>
#include "impls.h"
#include"operation_session.h"
#include "operation_manager.h"
#include <thread>
#include "fts_handler.h"
#include "dhcp_manager.h"

static
std::recursive_mutex re_mutex_;//登录锁

EXP(int) init_net(void(__stdcall *callback_link_closed)(int32_t), 
	void(__stdcall *callback_link_connect)(int32_t,int32_t, const char* host_name), const char* ip_port)
{
	if (!ip_port)return -1;
	//查找当前endpoint对象是否正在连接，或已经连接上
	nsp::tcpip::endpoint ep;
	if (nsp::tcpip::endpoint::build(std::string(ip_port, strlen(ip_port)), ep) < 0)
	{
		return -1;
	}
	if (nsp::toolkit::singleton<operation_manager>::instance()->is_endpoint_exist(ep)) {
		loinfo("operation_net") << "failed to init_net,the ip:" << ip_port << " is already exits.";
		return -1;
	}

	std::shared_ptr<operation_session> net_session;
	
	try{
		net_session = std::make_shared<operation_session>();
	}
	catch (const std::bad_alloc&)
	{
		loinfo("operation_net") << "failed to create " << ip_port << " share ptr of operation session.";
		return nsp::proto::errorno_t::kInsufficientResource;
	}

	uint32_t robot_id = nsp::toolkit::singleton<operation_manager>::instance()->alloc_robot_id();
	nsp::toolkit::singleton<operation_manager>::instance()->attach_seesion(robot_id, net_session);
	net_session->set_identity_id(robot_id);
	net_session->register_disconnect_callback(callback_link_closed);
	net_session->register_connect_callback(callback_link_connect);

	return robot_id;
}

EXP(int) connect_host(uint32_t robot_id, const char* str_ep)
{
	if (!str_ep)return -1;
	nsp::tcpip::endpoint ep;
	if (nsp::tcpip::endpoint::build(std::string(str_ep, strlen(str_ep)), ep) < 0) {
		//nsp::toolkit::singleton<operation_manager>::instance()->remove_seesion(robot_id);
		return nsp::proto::errorno_t::kInvalidParameter;
	}
	//登录使用锁，防止界面针对同一个ip地址多次快速进行请求连接
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);

	if (nsp::toolkit::singleton<operation_manager>::instance()->is_endpoint_exist(ep)) {
		loinfo("operation_net") << "connect to host,the endpoint is already exist.";
		//nsp::toolkit::singleton<operation_manager>::instance()->remove_seesion(robot_id);
		return nsp::proto::errorno_t::kEndpointExist;
	}
	std::shared_ptr<operation_session> net_session = nsp::toolkit::singleton<operation_manager>::instance()->find_session(robot_id);
	if (!net_session) return nsp::proto::errorno_t::kItemNoFound;

	loinfo("operation_net") << "start connect to server : " << str_ep << " ,the robot id is : " << robot_id;
	errno_t err_retval;
	err_retval = net_session->sync_connect(ep);
	
	return err_retval;
}

EXP(int) disconnect(uint32_t robot_id)
{
	loinfo("operation_net") << "start call disconnect from agvshell,the robot id is:" << robot_id;
	std::shared_ptr<operation_session> net_session = nsp::toolkit::singleton<operation_manager>::instance()->find_session(robot_id);
	if (!net_session) {
		lowarn("operation_net") << "can not file session of robot id:" << robot_id << " while disconnet from agvshell";
		return nsp::proto::errorno_t::kItemNoFound;
	}

	net_session->close();
	nsp::toolkit::singleton<operation_manager>::instance()->remove_seesion(robot_id);

	return nsp::proto::errorno_t::kSuccessful;
}

EXP(int) get_listdir(const char* ip_port, const char* data_stream, void** output_stream, int& output_cb)
{
	if ( !ip_port  || !data_stream || !output_stream) {
		return -1;
	}
	nsp::tcpip::endpoint ep;
	if (nsp::tcpip::endpoint::build(ip_port, ep) < 0){
		return -1;
	}
	return nsp::toolkit::singleton<fts_handler>::instance()->get_listdir_all(ep.ipv4(), ep.port(), data_stream, output_stream, output_cb);
}

EXP(int) get_current_catalog(const char* ip_port, const char* input_catalog, char** data_ptr, int* cb)
{
	if (!input_catalog || !ip_port || !data_ptr)return -1;
	nsp::tcpip::endpoint ep;
	if (nsp::tcpip::endpoint::build(ip_port, ep) < 0){
		return -1;
	}

	return nsp::toolkit::singleton<fts_handler>::instance()->get_current_catalog(ep.ipv4(), ep.port(), input_catalog, data_ptr, cb);
}

EXP(int)  regist_pull_callback(void(__stdcall *callback_func)(const int status, const int steps)){
	return  nsp::toolkit::singleton<fts_handler>::instance()->set_pull_callback(callback_func);
}

EXP(int)  regist_push_callback(void(__stdcall *callback_func)(const char* chr, const int cb)){
	return  nsp::toolkit::singleton<fts_handler>::instance()->set_push_callback(callback_func);
}

EXP(int) get_push_files_detail(const uint64_t task_id, void** data_ptr, int& cb) {
	if (!data_ptr) {
		return -1;
	}
	return nsp::toolkit::singleton<fts_handler>::instance()->get_push_files_detail(task_id, data_ptr, cb);
}

EXP(void) free_detail(void* ptr) {
	if (ptr) {
		delete[] ptr;
	}
}

EXP(int)  push_files_to_remote(const char* ip_port, const char* push_files, const int push_cb){
	if (!ip_port || !push_files)return -1;
	std::vector<std::string> vct_ip;
	std::string ip_str = ip_port;
	ip_str += "|";
	nsp::toolkit::slicing_symbol_string(ip_str, '|', vct_ip);
	std::vector<nsp::tcpip::endpoint> vct_ep;
	for (auto& iter : vct_ip) {
		nsp::tcpip::endpoint ep;
		if (nsp::tcpip::endpoint::build(iter, ep) < 0) {
			return -1;
		}
		vct_ep.push_back(ep);
	}
	if (vct_ep.size() == 0)return 0;
	set_fts_type(Fts_Function_Type::TY_PUSH_FILE);
	return nsp::toolkit::singleton<fts_handler>::instance()->push_files_to_remote(vct_ep, push_files, push_cb);
}

EXP(int)  get_file_crc(const char* input_file, unsigned int* crc_value){
	if (!input_file)return -1;
	return nsp::toolkit::singleton<fts_handler>::instance()->get_file_crc(input_file, crc_value);
}

EXP(int) delete_remote_files(const char* ip_port, const char* str_files, const int cb){
	nsp::tcpip::endpoint ep;
	if (nsp::tcpip::endpoint::build(ip_port, ep) < 0) {
		return -1;
	}
	if (!str_files || (strcmp(str_files, "") == 0)) return -1;
	return nsp::toolkit::singleton<fts_handler>::instance()->delete_remote_files(ep.ipv4(), ep.port(), str_files, cb);
}

EXP(int) repeat_push_files(const uint64_t task_group_id, const char* str_files, const int cb)
{
	return nsp::toolkit::singleton<fts_handler>::instance()->repeate_push_files(task_group_id, str_files, cb);
}

EXP(void) restore_factory_setting(const char* ip_port, void(__stdcall * callback_step)(int32_t step, int32_t status, const char* ip))
{
	if (!ip_port)return;
	std::vector<std::string> vct_ip;
	std::string ip_str = ip_port;
	ip_str += "|";
	nsp::toolkit::slicing_symbol_string(ip_str, '|', vct_ip);

	nsp::toolkit::singleton<fts_handler>::instance()->regiset_restore_setting(callback_step);
	nsp::toolkit::singleton<fts_handler>::instance()->restore_factory_setting(vct_ip);
}

EXP(int) pull_files_from_remote(const char* ip_port, const char* str_files, const int cb, const char* save_path) {
	if (!ip_port || !str_files || !save_path)return -1;
	nsp::tcpip::endpoint ep;
	if (nsp::tcpip::endpoint::build(ip_port, ep) < 0) {
		return -1;
	}
	set_fts_type(Fts_Function_Type::TY_PULL_FILE);
	return nsp::toolkit::singleton<fts_handler>::instance()->pull_files_from_remote(ep.ipv4(), ep.port(), str_files, cb, save_path);
}

EXP(void) update_folder_config() {
	nsp::toolkit::singleton<fts_handler>::instance()->update_folder_config();
}

EXP(void) set_block_size(const transfer_block_size block_size) {
	nsp::toolkit::singleton<fts_handler>::instance()->set_fts_callback(block_size);
}

EXP(int) post_cmd_agv_process(const char* robot_id_all, const char* process_id_all, const int cmd) {
	loinfo("operation_net") << "call cmd agv process API.";
	if (!robot_id_all || !process_id_all) return -1;
	std::vector<std::string> vct_robot;
	std::vector<std::string> vct_process_id;
	nsp::toolkit::slicing_symbol_string((std::string)robot_id_all + '|', '|', vct_robot);
	if (vct_robot.size() == 0)return 0;
	nsp::toolkit::slicing_symbol_string((std::string)process_id_all + '|', '|', vct_process_id);
	return nsp::toolkit::singleton<operation_manager>::instance()->post_agv_cmd(vct_robot, vct_process_id, (AgvShellCmd)cmd);
}

EXP(int) cancel_pull_opreate() {
	return nsp::toolkit::singleton<fts_handler>::instance()->cancel_pull_opreat();
}

EXP(int) get_m_camera_info_request(const char* robot_id_all, const FIRMWARE_SOFTWARE_TYPE frimware_type, const char* target_endpoint,
	const int node_id, const int can_bus)
{
	if (frimware_type < 0)return -1;
	if (!robot_id_all) return -1;
	std::vector<std::string> vct_robot;
	std::string robot_all = robot_id_all;
	robot_all += "|";
	nsp::toolkit::slicing_symbol_string(robot_all, '|', vct_robot);
	if (vct_robot.size() == 0)return 0;
	nsp::toolkit::singleton<operation_manager>::instance()->get_m_core_info(vct_robot, frimware_type, target_endpoint, node_id, can_bus);
	return 0;
}

EXP(void) regist_m_core_info_callback(void(__stdcall * function)(const char* ip, const VCU_MESSAGE vcu_type, const char* msg))
{
	nsp::toolkit::singleton<operation_manager>::instance()->set_frimware_info_callback(function);
}

EXP(int) push_m_camera_file(const char* ip_port, const FIRMWARE_SOFTWARE_TYPE type, const char* file_path, const char* target_endpoint)
{
	if (!ip_port || !file_path || !target_endpoint)return -1;
	std::vector<std::string>vct_ip;
	std::string ip_str = ip_port;
	ip_str += "|";
	nsp::toolkit::slicing_symbol_string(ip_str, '|', vct_ip);
	std::vector<nsp::tcpip::endpoint> vct_ep;
	for (auto& iter : vct_ip) {
		nsp::tcpip::endpoint ep;
		if (nsp::tcpip::endpoint::build(iter, ep) < 0) {
			return -1;
		}
		vct_ep.push_back(ep);
	}
	if (vct_ep.size() == 0)return 0;
	set_fts_type(Fts_Function_Type::TY_M_CORE_FILE);
	return nsp::toolkit::singleton<fts_handler>::instance()->push_m_camera_file(vct_ep, type, file_path, target_endpoint);
}

EXP(void) regist_m_core_transfer_callback(void(__stdcall* function)(const char* ip, const FRIMWARE_STATUS frimware_type, const int step))
{
	nsp::toolkit::singleton<operation_manager>::instance()->set_frimware_transfer_callback(function);
}

EXP(int) restart_m_core(const char* robot_id_all, const FIRMWARE_SOFTWARE_TYPE frimware_type, const char* target_endpoint)
{
	if (!robot_id_all) return -1;
	std::vector<std::string> vct_robot;
	std::string robot_all = robot_id_all;
	robot_all += "|";
	nsp::toolkit::slicing_symbol_string(robot_all, '|', vct_robot);
	if (vct_robot.size() == 0)return 0;
	nsp::toolkit::singleton<operation_manager>::instance()->post_restart_m_camera_request(vct_robot, frimware_type, target_endpoint);
	return 0;
}

//请求获取M核文件,保存在当前目录下的m_core文件夹内
EXP(int) pull_m_camera_file(const char* robot_id_all, const FIRMWARE_SOFTWARE_TYPE frimware_type, const int frimware_length, 
	const char* local_path, const char* target_endpoint)
{
	if (!robot_id_all || !local_path) return -1;
	std::vector<std::string> vct_robot;
	std::string robot_all = robot_id_all;
	robot_all += "|";
	nsp::toolkit::slicing_symbol_string(robot_all, '|', vct_robot);
	if (vct_robot.size() == 0)return 0;
	nsp::toolkit::singleton<operation_manager>::instance()->post_pull_m_core_request(vct_robot, frimware_type, 
		frimware_length, target_endpoint, local_path);
	return 0;
}

EXP(void) regist_process_status_callback(void(__stdcall* function)(const char* endpoint, ProcessStatus status))
{
	return nsp::toolkit::singleton<operation_manager>::instance()->set_process_update(function);
}

EXP(int) update_agv_shell(const char* robot_id_list, const char* shell_path)
{
	if (!robot_id_list || !shell_path) return -1;
	std::vector<std::string> vct_robot;
	std::string robot_all = robot_id_list;
	robot_all += "|";
	nsp::toolkit::slicing_symbol_string(robot_all, '|', vct_robot);
	if (vct_robot.size() == 0)return 0;
	set_fts_type(Fts_Function_Type::TY_KEY_TO_UPGRADE);
	nsp::toolkit::singleton<operation_manager>::instance()->update_agv_shell(vct_robot, shell_path);
	return 0;
}

EXP(void) regist_netowrk_callback(void(__stdcall *callback_func)(const char* robot_ip, const int type, const int error, const char* str, const int cb))
{
	nsp::toolkit::singleton<operation_manager>::instance()->set_network_callback(callback_func);
	nsp::toolkit::singleton<dhcp_manager>::instance()->set_network_callback(callback_func);
}

EXP(void) get_agv_process_table()
{
	loinfo("operation_net") << "call get all process table API.";
	nsp::toolkit::singleton<operation_manager>::instance()->post_get_agv_process_table();
}

EXP(int) post_async_package(const int robot_id, const int type, const void* str, const int cb)
{
	if (!str)return -1;
	return nsp::toolkit::singleton<operation_manager>::instance()->post_async_package(robot_id, type, str, cb);
}

EXP(int) post_sync_package(const int robot_id, const int type, const void* str, const int cb, void **_ack_msg, int *_ack_len)
{
	if (!_ack_msg || !_ack_len) {
		return -1;
	}
	return nsp::toolkit::singleton<operation_manager>::instance()->post_sync_package(robot_id, type, str, cb, _ack_msg, _ack_len);
}

EXP(int) is_open_dhcp(bool status)
{
	if (status)
	{
		return nsp::toolkit::singleton<dhcp_manager>::instance()->connect_to_dhcp();
	}
	else
	{
		return nsp::toolkit::singleton<dhcp_manager>::instance()->disconnect_dhcp();
	}
}

EXP(int) post_sync_without_id(const int type, const void* str, const int cb, void **_ack_msg, int *_ack_len)
{
	if (!_ack_msg || !_ack_len) {
		return -1;
	}
	return nsp::toolkit::singleton<dhcp_manager>::instance()->post_sync_without_id(type, str, cb, _ack_msg, _ack_len);
}

EXP(int) push_can_bin_file(const char* ip_port, const uint32_t node_id, const uint32_t serial_type, const char* file_path, const char* target_endpoint)
{
	if (!ip_port || !file_path || !target_endpoint)return -1;
	std::vector<std::string>vct_ip;
	std::string ip_str = ip_port;
	ip_str += "|";
	nsp::toolkit::slicing_symbol_string(ip_str, '|', vct_ip);
	std::vector<nsp::tcpip::endpoint> vct_ep;
	for (auto& iter : vct_ip) {
		nsp::tcpip::endpoint ep;
		if (nsp::tcpip::endpoint::build(iter, ep) < 0) {
			return -1;
		}
		vct_ep.push_back(ep);
	}
	if (vct_ep.size() == 0)return 0;
	loinfo("operation_net") << "write can custom bin file,the node id is " << node_id << " and serial code is" << serial_type;
	set_fts_type(Fts_Function_Type::TY_M_CORE_FILE);
	return nsp::toolkit::singleton<fts_handler>::instance()->post_can_custom_file(vct_ep, node_id, serial_type, file_path, target_endpoint);
}

EXP(int) post_backup_file(const void* str, const int cb)
{
	if (!str) {
		return -1;
	}
	set_fts_type(Fts_Function_Type::TY_KEY_BACKUP);
	return nsp::toolkit::singleton<operation_manager>::instance()->post_backups_files(str, cb);
}