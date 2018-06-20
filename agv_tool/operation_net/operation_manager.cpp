#include "operation_manager.h"
#include <iostream>
#include "fts_handler.h"
#include "err_msg.h"
#include "proto_process_status.h"
#include "rw_xml_file.h"
#include "proto_interface.h"

operation_manager::operation_manager()
{
	init();
}


operation_manager::~operation_manager()
{
}

void operation_manager::init() {
	if (__th_keepalive)return;
	try {
		//启动心跳线程
		if (!__th_keepalive) {
			__th_keepalive = new std::thread(std::bind(&operation_manager::keepalive, this));
		}
	}
	catch (...) {
		loerror("operation_net") << "can not start the thread of query agv_shell process status.";
	}
}

void operation_manager::uinit() {
	__keepalive_stop.reset();
	is_exist_ = true;
	if (__th_keepalive) {
		if (__th_keepalive->joinable()) {
			__th_keepalive->join();
		}
		delete __th_keepalive;
		__th_keepalive = nullptr;
	}
}

void operation_manager::keepalive()
{
	static const int TEST_TIMEOUT_INTERVAL = 10000;
	while (__keepalive_stop.wait(2000) > 0) {
		if (is_exist_) break;

		std::shared_ptr<operation_session> client_session = nullptr;
		{
			std::lock_guard<decltype(mutex_net_connected_)> guard(mutex_net_connected_);
			for (const auto & iter : map_connect_session_)
			{
				if (iter.second && ((iter.second->get_network_status() == net_status_t::KNetwork_Established) || 
					(iter.second->get_network_status() == net_status_t::KNetwork_Connected))) {
					uint64_t interval = iter.second->get_interval();
					if (interval > TEST_TIMEOUT_INTERVAL) {
						loerror("operation_net") << "the client session is timeout,it can not receive alive package.";//" << client_session->get_target_endpoint().ipv4() << " 
						client_session = iter.second;
						break;
					}
				}
			}
		}
		if (client_session != nullptr)
		{
			client_session->close();
		}
	}
	__keepalive_stop.reset();
}

int  operation_manager::attach_seesion(uint32_t robot_id, std::shared_ptr<operation_session> &pseesion)
{
	std::lock_guard<decltype(mutex_net_session_)> guard(mutex_net_session_);
	map_net_session_[robot_id] = pseesion;
	return 0;
}

std::shared_ptr<operation_session> operation_manager::find_session(uint32_t robot_id)
{
	std::lock_guard<decltype(mutex_net_session_)> guard(mutex_net_session_);
	auto iter = map_net_session_.find(robot_id);
	if (iter == map_net_session_.end()) {
		return nullptr;
	}
	return iter->second;
}

int  operation_manager::remove_seesion(uint32_t robot_id)
{
	//移除已经连接上的session
	rm_connected_session(robot_id);

	std::lock_guard<decltype(mutex_net_session_)> guard(mutex_net_session_);
	auto iter = map_net_session_.find(robot_id);
	if (iter == map_net_session_.end()) {
		loerror("operation_net") << "can not find robot id:" << robot_id << " in the all robot_session";
		return -1;
	}
	map_net_session_.erase(iter);
	loinfo("operation_net") << "success to remove robot id:" << robot_id << " from all robot session.";
	return 0;
}

void operation_manager::add_connected_session(uint32_t robot_id)
{
	std::lock_guard<decltype(mutex_net_connected_)> guard(mutex_net_connected_);
	std::shared_ptr<operation_session> pseesion = find_session(robot_id);
	map_connect_session_[robot_id] = pseesion;
}

std::shared_ptr<operation_session> operation_manager::find_connect_session(const uint32_t robot_id)
{
	std::lock_guard<decltype(mutex_net_connected_)> guard(mutex_net_connected_);
	auto iter = map_connect_session_.find(robot_id);
	if (iter == map_connect_session_.end()) {
		return nullptr;
	}
	return iter->second;
}

void operation_manager::rm_connected_session(uint32_t robot_id)
{
	std::lock_guard<decltype(mutex_net_connected_)> guard(mutex_net_connected_);
	auto iter = map_connect_session_.find(robot_id);
	if (iter == map_connect_session_.end()) {
		return;
	}
	map_connect_session_.erase(iter);
}

uint32_t operation_manager::alloc_robot_id()
{
	return ++robot_id_;
}

bool operation_manager::is_endpoint_exist(const nsp::tcpip::endpoint& ep) {
	std::lock_guard<decltype(mutex_net_session_)> guard(mutex_net_session_);
	for (const auto& iter : map_net_session_) {
		if (strcmp(iter.second->get_target_endpoint().ipv4(), ep.ipv4()) == 0 && 
			iter.second->get_target_endpoint().port() == ep.port()) {
			return true;
		}
	}
	return false;
}

int operation_manager::post_agv_cmd(const std::vector<std::string>& vct_robot_id, 
	const std::vector<std::string>& vct_process_id, 
	const AgvShellCmd type)
{
	for (const auto& iter : vct_robot_id)
	{
		int robot_id = atoi(iter.c_str());
		std::shared_ptr<operation_session> client_session = find_connect_session(robot_id);
		if (!client_session)continue;
		int cmd_process = 0;
		std::vector<std::string> vct_parament;
		if (ReBoot != type)
		{
			//界面分配的进程id转换成内部id
			char ip_port[128];
			sprintf_s(ip_port, "%s:%d", client_session->get_target_endpoint().ipv4(), client_session->get_target_endpoint().port());
			auto process_reletion_iter = map_all_process_releation_.find(ip_port);
			if (process_reletion_iter == map_all_process_releation_.end())
			{
				loerror("operation_net") << "can not find the original process id of "<<client_session->get_target_endpoint().ipv4() ;
				continue;
			}
			for (const auto& relation : process_reletion_iter->second)
			{
				auto pro_iter = std::find_if(vct_process_id.begin(), vct_process_id.end(), [&](const std::string& p_id)->bool {
					int pro_id = atoi(p_id.c_str());
					return relation.allocat_id_ == pro_id ? true : false;
				});
				if (pro_iter != vct_process_id.end())
				{
					cmd_process = cmd_process | (1 << relation.original_id_);
				}
				vct_parament.push_back("");
			}
			/*for (const auto& str_id : vct_process_id)
			{
				int pro_id = atoi(str_id.c_str());
				auto orig_iter = std::find_if(process_reletion_iter->second.begin(), process_reletion_iter->second.end(), [&](const process_id_releation& p_info)->bool {
					return p_info.allocat_id_ == pro_id ? true : false;
				});
				if (orig_iter != process_reletion_iter->second.end())
				{
					cmd_process = cmd_process | (1 << orig_iter->original_id_);
				}
			}*/
		}
		client_session->post_agv_cmd(cmd_process, vct_parament, type);
	}
	return 0;
}

void operation_manager::set_cmd_result(const int value) {
	cmd_result_ = value;
}

int operation_manager::get_cmd_result() {
	return cmd_result_;
}

void operation_manager::set_wait_sig() {
	process_waiter_.sig();
}

void operation_manager::set_net_status(const std::string& ip, const nsp::proto::errorno_t& status) {
	if (ip.empty()) return;
	std::lock_guard<decltype(mutext_net_status_)> lock(mutext_net_status_);
	auto iter = map_net_status_.find(ip);
	if (iter == map_net_status_.end()) {
		map_net_status_.insert(std::make_pair(ip, status));
		return;
	}
	iter->second = status;
	return;
}

nsp::proto::errorno_t operation_manager::get_net_status(const std::string& ip) {
	std::lock_guard<decltype(mutext_net_status_)> lock(mutext_net_status_);
	auto iter = map_net_status_.find(ip);
	if (iter == map_net_status_.end()) {
		return nsp::proto::errorno_t::kNoConnected;
	}
	return iter->second;
}

void operation_manager::get_m_core_info(const std::vector<std::string>& vct_robot, const FIRMWARE_SOFTWARE_TYPE frimware_type, 
	const std::string& target_ep, const int node_id, const int can_bus) {
	
	if (frimware_type == FIRMWARE_SOFTWARE_TYPE::CAN_CUSTOM_SOFTWARE)
	{
		get_custom_info_request(vct_robot, target_ep, node_id, can_bus);
	}
	else
	{
		for (const auto& iter : vct_robot)
		{
			uint32_t robot_id = atoi(iter.c_str());
			std::shared_ptr<operation_session> client_session = find_session(robot_id);
			if (client_session)
			{
				//先通知界面正在操作
				frimware_transfer_notify(client_session->get_target_endpoint().to_string().c_str(), FRIMWARE_STATUS::kBusy, 0);
				//下发请求
				client_session->post_frimware_info_request(frimware_type, target_ep);
			}
		}
	}
}

void operation_manager::get_custom_info_request(const std::vector<std::string>& vct_robot, const std::string& target_ep, 
	const int node_id, const int can_bus)
{
	for (const auto& iter : vct_robot)
	{
		uint32_t robot_id = atoi(iter.c_str());
		std::shared_ptr<operation_session> client_session = find_session(robot_id);
		if (client_session)
		{
			//先通知界面正在操作
			frimware_transfer_notify(client_session->get_target_endpoint().to_string().c_str(), FRIMWARE_STATUS::kBusy, 0);
			//下发请求
			client_session->post_can_custom_info_request(target_ep, node_id, can_bus);
		}
	}
}

void operation_manager::set_frimware_info_callback(void(__stdcall* func)(const char* ip, const VCU_MESSAGE vcu_type, const char* msg))
{
	frimware_info_ = func;
}

void operation_manager::frimware_info_notify(const char* ip, const VCU_MESSAGE vcu_type, 
	const FRIMWARE_STATUS frimware_status, const char* msg) 
{
	if (frimware_info_)
	{
		frimware_info_(ip, vcu_type, msg);
	}
}

void operation_manager::post_m_core_transfer_request(const char* ip, const FIRMWARE_SOFTWARE_TYPE frimware_type, const std::string& remote_file, 
	const std::string& target_ep) {
	std::lock_guard<decltype(mutex_net_session_)> guard(mutex_net_session_);
	for (const auto& iter : map_net_session_)
	{
		if (strcmp(iter.second->get_target_endpoint().ipv4(), ip) == 0)
		{
			iter.second->post_m_core_transfer_request(frimware_type, remote_file, target_ep);
			break;
		}
	}
}

void operation_manager::post_custom_transfer_request(const char* ip, const int node_id, const int serial_type, const std::string&remote_file,
	const std::string& target_ep)
{
	if (!ip)return;
	std::lock_guard<decltype(mutex_net_session_)> guard(mutex_net_session_);
	for (const auto& iter : map_net_session_)
	{
		if (strcmp(iter.second->get_target_endpoint().ipv4(), ip) == 0)
		{
			iter.second->post_can_custom_transfer_request(node_id, serial_type, remote_file, target_ep);
			break;
		}
	}
}

void operation_manager::post_pull_m_core_request(const std::vector<std::string>& vct_robot,const FIRMWARE_SOFTWARE_TYPE frimware_type, 
	const int frimware_length, const std::string& target_ep, const std::string& local_path) {
	for (const auto& iter : vct_robot)
	{
		uint32_t robot_id = atoi(iter.c_str());
		std::shared_ptr<operation_session> client_session = find_session(robot_id);
		if (client_session)
		{
			client_session->post_pull_m_core_request(frimware_type, frimware_length, target_ep, local_path);
		}
	}
}

void operation_manager::post_restart_m_camera_request(const std::vector<std::string>& vct_robot, const FIRMWARE_SOFTWARE_TYPE frimware_type, 
	const std::string& target_ep)
{
	for (const auto & iter : vct_robot)
	{
		uint32_t robot_id = atoi(iter.c_str());
		std::shared_ptr<operation_session> client_session = find_session(robot_id);
		if (client_session)
		{
			client_session->post_restart_m_camera_request(frimware_type, target_ep);
		}
	}
}

void operation_manager::set_frimware_transfer_callback(void(__stdcall* func)(const char* ip, const FRIMWARE_STATUS frimware_type, const int step)) {
	frimware_transfer_callback_ = func;
}

void operation_manager::frimware_transfer_notify(const char* ip, const FRIMWARE_STATUS status, const int step) {
	std::cout << "status is " << status << " ,the step is " << step << std::endl;
	if (frimware_transfer_callback_)
	{
		frimware_transfer_callback_(ip, status, step);
	}
}

void operation_manager::set_process_update(void(__stdcall* function)(const char* endpoint, ProcessStatus status)) {
	process_status_ = function;
}

void operation_manager::update_process_status(const std::string& ep, const int status) {
	if (process_status_)
	{
		process_status_(ep.c_str(), status == 1 ? ProcessStatus::kStop : ProcessStatus::kRunning);
	}
}

void operation_manager::update_agv_shell(std::vector<std::string>&robot_id_list, const std::string& agv_path) {
	{
		//记录所有需要更新升级的目标对象
		std::lock_guard<decltype(shell_progress_mutex_)> lock(shell_progress_mutex_);
		map_update_progress_.clear();

		for (const auto& iter : robot_id_list) {
			uint32_t robot_id = atoi(iter.c_str());
			std::shared_ptr<operation_session> client_session = find_session(robot_id);
			if (client_session)
			{
				map_update_progress_.insert(std::make_pair(client_session->get_target_endpoint().ipv4(), file_status::wait));
			}
		}
	}
	//遍历每一个需要更新的目标对象进行更新操作
	for (const auto& iter : robot_id_list) {
		uint32_t robot_id = atoi(iter.c_str());
		std::shared_ptr<operation_session> client_session = find_session(robot_id);
		if (client_session)
		{
			std::string target_ipv4 = client_session->get_target_endpoint().ipv4();
			loinfo("operation_net") << "push agv shell file into task deque,the target ipv4:" << target_ipv4;
			nsp::toolkit::singleton<fts_handler>::instance()->push_agv_file(target_ipv4, agv_path);
		}
	}
}

void operation_manager::post_agv_shell(const std::string ipv_4, const std::string& remote_path, const uint32_t fize) {
	{
		std::lock_guard<decltype(mutex_net_connected_)> guard(mutex_net_connected_);
		for (const auto& iter : map_connect_session_)
		{
			if (iter.second->get_target_endpoint().ipv4() == ipv_4)
			{
				iter.second->post_agv_shell(remote_path, fize);
				break;
			}
		}
	}
	
	{
		//更新本地缓存中目标对象传输进度为complete完成状态
		update_files_progress(ipv_4, file_status::complete, Proto_Update_Progress);
	}
}

void operation_manager::network_callback_notify(const char* robot_ep, const int type, const int error, const char* data, const int cb)
{
	if (network_callback_)
	{
		network_callback_(robot_ep, type, error, data , cb);
	}
}

void operation_manager::set_network_callback(void(__stdcall* func)(const char* robot_ip, const int type, const int error, 
	const char* data_str, const int cb))
{
	network_callback_ = func;
}

void operation_manager::post_get_agv_process_table()
{
	std::lock_guard<decltype(mutex_net_connected_)> guard(mutex_net_connected_);
	map_process_all_.clear();
	map_all_process_releation_.clear();
	process_id_ = 0;//初始化内部分配的进程ID
	loinfo("operation_net") << "connect session count is :" << map_connect_session_.size();
	for (const auto& iter : map_connect_session_)
	{
		nsp::os::waitable_handle water;
		motion::asio_data data_asio;
		process_list_info p_list_info;
		if (!iter.second)continue;
		if (iter.second->post_get_agv_process_table(std::make_shared<motion::asio_block>([&](const void* data){
			if (!data){
				water.sig();
				return;
			}
			data_asio = *(motion::asio_data*)data;
			if (data_asio.get_err() != nsp::proto::kSuccessful){
				loerror("operation_net") << "failed to get asio data while post get agv process list package.";
				water.sig();
				return;
			}
			p_list_info = *(process_list_info*)data;
			water.sig();
		})) < 0)
		{
			loerror("operation_net") << "failed to post get agv process list package.";
			continue;
		}
		water.wait();
		if (nsp::proto::kSuccessful == data_asio.get_err()){
			char ip_port[128];
			sprintf_s(ip_port, "%s:%d", iter.second->get_target_endpoint().ipv4(), iter.second->get_target_endpoint().port());
			for (const auto& list_iter : p_list_info.vct_process_)
			{
				auto map_iter = map_process_all_.find(list_iter.process_name);
				if (map_iter == map_process_all_.end())
				{
					//集合中没有该进程，则添加之
					uint32_t id = allocat_process_id();
					map_process_all_.insert(std::make_pair(list_iter.process_name, id));
					//添加内部id映射
					add_process_relation(ip_port, id, list_iter.process_id);
				}
				else
				{
					add_process_relation(ip_port, map_iter->second, list_iter.process_id);
				}
			}
		}
		else
		{
			loerror("operation_net") << "get post_get_agv_process_table ack timeout.the target ip is :" << iter.second->get_target_endpoint().to_string();
			return ;
		}
	}
	//序列化数据，回调给界面使用者
	nsp::proto::process_list pkt;
	for (const auto& iter : map_process_all_)
	{
		nsp::proto::process_relation relation;
		relation.process_id_ = iter.second;
		relation.process_name_ = iter.first;
		pkt.process_list_.push_back(relation);
	}
	loinfo("operation_net") << "server process number:" << map_process_all_.size();

	unsigned char *buffer = new  unsigned char[pkt.length()];
	pkt.serialize(buffer);
	nsp::toolkit::singleton<operation_manager>::instance()->network_callback_notify("", Proto_Agv_Process_List, 0, (char*)buffer, pkt.length());
	delete[] buffer;
}

void operation_manager::add_process_relation(const std::string ipv4_port, const uint32_t allocat_id, const uint32_t original_id)
{
	//添加内部id映射
	auto target_list = map_all_process_releation_.find(ipv4_port);
	process_id_releation pe_id;
	pe_id.allocat_id_ = allocat_id;
	pe_id.original_id_ = original_id;
	if (target_list == map_all_process_releation_.end())
	{
		std::vector<process_id_releation> vct_id;
		vct_id.push_back(pe_id);
		map_all_process_releation_.insert(std::make_pair(ipv4_port, vct_id));
	}
	else
	{
		target_list->second.push_back(pe_id);
	}
}

int operation_manager::get_process_allocat_id(const std::string& ip_port, const int original_id)
{
	std::lock_guard<decltype(mutex_net_session_)> guard(mutex_net_session_);
	auto iter = map_all_process_releation_.find(ip_port);
	if (iter == map_all_process_releation_.end())
	{
		return -1;
	}
	for (const auto& id_iter : iter->second)
	{
		if (id_iter.original_id_ == original_id)
		{
			return id_iter.allocat_id_;
		}
	}
	return -1;
}

int operation_manager::post_async_package(const int robot_id, const int type, const void* str, const int cb)
{
	std::shared_ptr<operation_session> client_session = find_connect_session(robot_id);
	if (!client_session) return nsp::proto::errorno_t::kItemNoFound;
	return client_session->post_package_async(type, str, cb);
}

int operation_manager::post_sync_package(const int robot_id, const int type, const void* str, const int cb, void ** ack_msg, int * ack_len)
{
	std::shared_ptr<operation_session> client_session = find_connect_session(robot_id);
	if(!client_session) return nsp::proto::errorno_t::kItemNoFound;
	return client_session->post_package_sync(type, str, cb, ack_msg, ack_len);
}

int operation_manager::post_backups_files(const void* str, const int cb)
{
	nsp::api::proto_backups_info unpkt;
	int cb_build = cb;
	if (unpkt.build((unsigned char*)str, cb_build) < 0)
	{
		loerror("operation_net") << "failed to build backups interface data.";
		return -1;
	}

	{
		//记录所有需要更新升级的目标对象
		std::lock_guard<decltype(shell_progress_mutex_)> lock(shell_progress_mutex_);
		map_update_progress_.clear();

		for (const auto& iter : unpkt.vct_robot_item_) {
			std::shared_ptr<operation_session> client_session = find_session(iter.robot_id_);
			if (client_session)
			{
				map_update_progress_.insert(std::make_pair(client_session->get_target_endpoint().ipv4(), file_status::wait));
			}
		}
	}
	//设置fts发送文件所在路径
	set_backup_file(unpkt.local_path_);

	std::vector<std::string> vct_file = nsp::toolkit::singleton<fts_handler>::instance()->get_backup_files();
	for (const auto iter : unpkt.vct_robot_item_)
	{
		std::shared_ptr<operation_session> client_session = find_connect_session(iter.robot_id_);
		if (client_session)
		{
			client_session->set_local_backups_path(iter.car_id_, unpkt.local_path_);
			client_session->post_backup_files(vct_file);
		}
	}
}

void operation_manager::update_files_progress(const std::string& ipv4, const file_status status, const int callback_type)
{
	//更新本地缓存中目标对象传输进度为complete完成状态
	std::lock_guard<decltype(shell_progress_mutex_)> shell_lock(shell_progress_mutex_);
	auto iter = map_update_progress_.find(ipv4);
	if (iter != map_update_progress_.end())
	{
		iter->second = status;
	}
	//寻找所有已经完成传输的目标对象
	int complete_count = 0;
	for (const auto & iter : map_update_progress_)
	{
		if (file_status::complete == iter.second)complete_count++;
	}
	int progress = ((double)complete_count / map_update_progress_.size()) * 100;
	loinfo("operation_net") << "the update target robot ipv4: " << ipv4 << " current files progress is: " << progress;
	//回调通知
	if (network_callback_)
	{
		char buffer[32] = { 0 };
		itoa(progress, buffer, 10);
		nsp::api::proto_msg msg;
		msg.msg = buffer;
		unsigned char *chr_buffer = new  unsigned char[msg.length()];
		msg.serialize(chr_buffer);
		network_callback_("", callback_type, 0, (char*)chr_buffer, msg.length());
		delete[] chr_buffer;
		loinfo("operation_net") << "success callback target robot ipv4: " << ipv4 << " current files progress is: " << progress;
	}
}