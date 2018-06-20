#include "operation_session.h"
#include <thread>
#include "operation_manager.h"
#include "fts_handler.h"
#include "agv_shell_proto.hpp"
#include <iostream>
#include "err_msg.h"
#include "proto_process_status.h"
#include "proto_interface.h"
#include "recv_handler.h"

operation_session::operation_session():nsp::tcpip::tcp_application_client< nsp::proto::nspdef::protocol>(){}

operation_session::operation_session(HTCPLINK link) :nsp::tcpip::tcp_application_client< nsp::proto::nspdef::protocol>(link){}

operation_session::~operation_session(){}

void operation_session::set_identity_id(uint32_t id)
{
	identity_id_ = id;
}

void operation_session::on_disconnected(const HTCPLINK previous)
{
	//当前session 网络状态
	net_status_ = KNetwork_Disconnect;

	nsp::toolkit::singleton<operation_manager>::instance()->remove_seesion(identity_id_);
	//更新网络状态表中网络状态
	nsp::toolkit::singleton<operation_manager>::instance()->set_net_status(ep_.ipv4(), nsp::proto::kFailDisconnect);
	nsp::toolkit::singleton<fts_handler>::instance()->update_disconnect_status(ep_.ipv4(), ep_.port());

	//回调通知界面
	if (!disconnect_callback_) {
		lowarn("operation_net") << "the target endpoint:" << ep_.to_string() << " disconnect callback function is empty.";
		return;
	}
	loinfo("operation_net") << "disconnect from server:" << ep_.to_string() << " the link is "
		<< previous << " and the robot id is:" << identity_id_;
	disconnect_callback_(identity_id_);
}

void operation_session::on_connected()
{
	//当前连接状态至为已连接
	net_status_ = KNetwork_Connected;
	nsp::toolkit::singleton<operation_manager>::instance()->add_connected_session(identity_id_);
	//发送登录身份包
	post_login();
	//更新网络状态表中网络状态
	nsp::toolkit::singleton<operation_manager>::instance()->set_net_status(ep_.ipv4(), nsp::proto::kSuccessful);
}

void operation_session::register_disconnect_callback(void(__stdcall *notify_callback)(int32_t))
{
	disconnect_callback_ = notify_callback;
}

void operation_session::register_connect_callback(void(__stdcall *notify_callback)(int32_t, int32_t, const char* host_name))
{
	connect_callback_ = notify_callback;
}

void operation_session::on_recvdata(const std::string &pkt_data)
{
	//记录当前时间点
	refresh_tick_ = nsp::os::gettick();
	nsp::toolkit::singleton<net_recv_manager>::instance()->async_recv_handler(pkt_data,
		std::bind(&operation_session::on_recv_dispath, this, std::placeholders::_1, std::placeholders::_2));
}

void operation_session::on_recv_dispath(const unsigned char* data, int & cb)
{
	std::shared_ptr<nsp::proto::agv_shell_proto_head> head = nsp::proto::shared_for<nsp::proto::agv_shell_proto_head>
		(data, nsp::proto::agv_shell_proto_head::type_length());

	if (!head) {
		close();
		return;
	}
	uint8_t ty = head->type;
	std::string pkt_data;
	pkt_data.append((char*)data, cb);
	if (dispatch_package(ty, pkt_data)<0) {
		close();
	}
}

uint64_t operation_session::get_interval()
{
	if (refresh_tick_ == 0) {
		return 0;
	}
	return nsp::os::gettick() - refresh_tick_;
}

int operation_session::dispatch_package(const uint8_t package_type, const std::string &pkt_data)
{
	int retval = 0;
	switch (package_type) {
	case kAgvShellProto_Hostname_Ack:
		retval = recv_get_hostname_ack(nsp::proto::shared_for<nsp::proto::proto_msg>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_Status:
		retval = recv_process_status(nsp::proto::shared_for<nsp::proto::proto_process_status>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_StartProcess_Ack:
	case kAgvShellProto_StopProcess_Ack:
	case kAgvShellProto_RestartProcess_Ack:
		retval = recv_cmd_process_ack(nsp::proto::shared_for<nsp::proto::proto_msg_int>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_Frimware_Info_Ack:
		retval = recv_frimware_info_ack(nsp::proto::shared_for<nsp::proto::proto_frimware_info_response>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_Frimware_Update_Ack:
		retval = recv_frimware_transfer_ack(nsp::proto::shared_for<nsp::proto::proto_frimware_update_response>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_Frimware_Download_Ack:
		retval = recv_frimware_download_ack(nsp::proto::shared_for<nsp::proto::proto_frimware_download_reponse>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_Process_List_Ack:
		retval = recv_process_list(nsp::proto::shared_for<nsp::proto::proto_process_list_reponse>(pkt_data.c_str(), pkt_data.length()),
			pkt_data.substr(nsp::proto::agv_shell_proto_head::type_length()));
		break;
	case kAgvShellProto_Login_Ack:
		retval = recv_login_ack(nsp::proto::shared_for<nsp::proto::proto_msg_int>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_CmdList_ACK:
		retval = recv_cmd_list(nsp::proto::shared_for<nsp::proto::proto_process_list_reponse>(pkt_data.c_str(), pkt_data.length()),
			pkt_data.substr(nsp::proto::agv_shell_proto_head::type_length()));
		break;
	case kAgvShellProto_UpdateCmdList_ACK:
		retval = recv_update_cmd_ack(nsp::proto::shared_for<nsp::proto::proto_common_stream>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_Set_Keepalive_Status_Ack:

		break;
	case kAgvShellProto_FileMutexStatus:
		retval = recv_file_mutex_status(nsp::proto::shared_for<nsp::proto::proto_keepalive_status_reponse>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_BackupDemarcate_ACK:
	case kAgvShellProto_ModifyFileMutex_ACK:
		retval = recv_modify_mutex(nsp::proto::shared_for<nsp::proto::proto_msg_int_sync>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_ShellVersionInfo:
		retval = recv_shell_version_info(nsp::proto::shared_for<nsp::proto::proto_shell_version_t>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_BackupFiles_ACK:
		retval = recv_backups_files_ack(nsp::proto::shared_for<nsp::proto::proto_msg>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_ConfigVersion:
		retval = recv_config_version_info(nsp::proto::shared_for<nsp::proto::proto_shell_version_t>(pkt_data.c_str(), pkt_data.length()));
		break;
	default:
		loerror("operation_session") << "ERROR UNKNOWN PACKET ,unknown TCP packet,the type is:" << package_type;
		break;
	}
	return retval;
}

int operation_session::sync_connect(const nsp::tcpip::endpoint &ep)
{
	ep_ = ep;
	// 未初始化
	net_status_t exp = kNetwork_Closed;
	if (net_status_.compare_exchange_strong(exp, kNetwork_Active)) {
		if (this->create() < 0) {
			net_status_ = kNetwork_Closed;
			return nsp::proto::errorno_t::kFailCreate;
		}
		else {
			;
		}
	}

	// 连接中的会话不再重复执行连接
	exp = kNetwork_Active;
	if (net_status_.compare_exchange_strong(exp, kNetwork_Connecting)) {
		loinfo("operation_net") << "try connect to host " << ep_.to_string();
		if (connect2(ep_) < 0) {
			loerror("operation_net") << "failed to call ns API connect.this link will be destory,robot:" << this->identity_id_;
			close();
			return nsp::proto::errorno_t::kFailConnected;
		}
	}

	return nsp::proto::errorno_t::kSuccessful;
}

int operation_session::post_login() {
	nsp::proto::proto_login_agv pkt(kAgvShellProto_Login);
	pkt.login_identify = AgvClientType_CMP;
	return psend(&pkt);
}

int operation_session::recv_get_hostname_ack(const std::shared_ptr<nsp::proto::proto_msg>& data) {
	if (!data) {
		loerror("operation_net") << "failed to build hostname ack.";
		return 0;
	}
	if (!connect_callback_) {
		return -1;
	}
	loinfo("operation_net") << "success to connect to server,the robot id is "<< identity_id_ <<",the link is " << this->lnk_;
	connect_callback_(identity_id_, 0, data->msg_);
	return 0;
}

int operation_session::recv_cmd_process_ack(const std::shared_ptr<nsp::proto::proto_msg_int>& data){
	if (!data) {
		loerror("operation_net") << "failed to build command process ack.";
		return 0;
	}
	nsp::toolkit::singleton<operation_manager>::instance()->set_cmd_result(data->msg_int_);
	nsp::toolkit::singleton<operation_manager>::instance()->set_wait_sig();
	return 0;
}

int operation_session::recv_process_list(const std::shared_ptr<nsp::proto::proto_process_list_reponse>& data, const std::string& pkt_data)
{
	if (!data) {
		loerror("operation_net") << "failed to build process list ack.";
		return 0;
	}

	process_list_info info;
	info.set_err(nsp::proto::errorno_t::kSuccessful);

	for (const auto & iter : data->process_list_)
	{
		process_info p_info;
		p_info.process_id = iter.process_id_;
		p_info.process_name = iter.process_name_;
		info.vct_process_.push_back(p_info);
		map_process_list_.insert(std::make_pair(iter.process_id_, iter.process_name_));
	}
	return nsp::toolkit::singleton<net_manager>::instance()->exec(data->pkt_id_, (char *)&info);
}

int operation_session::recv_cmd_list(const std::shared_ptr<nsp::proto::proto_process_list_reponse>& data, const std::string & pkt_msg)
{
	if (!data) {
		loerror("operation_net") << "failed to build cmd list ack.";
		return 0;
	}

	common_msg info;
	info.set_err(nsp::proto::errorno_t::kSuccessful);
	const char* msg_pos = pkt_msg.c_str();
	//去除包中前4个字节的pkt_id
	info.msg_.assign(msg_pos + 4, pkt_msg.size() - 4);
	return nsp::toolkit::singleton<net_manager>::instance()->exec(data->pkt_id_, (char *)&info);
}

int operation_session::recv_update_cmd_ack(const std::shared_ptr<nsp::proto::proto_common_stream>&data)
{
	if (!data) {
		loerror("operation_net") << "failed to build update cmd ack.";
		return 0;
	}

	common_msg info;
	info.set_err(nsp::proto::errorno_t::kSuccessful);
	info.msg_.assign(data->common_stream.c_str(), data->common_stream.size());
	return nsp::toolkit::singleton<net_manager>::instance()->exec(data->pkt_id, (char *)&info);
}

int operation_session::recv_process_status(const std::shared_ptr<nsp::proto::proto_process_status>&data) 
{
	if (!data) {
		loerror("operation_net") << "failed to build process status ack.";
		return 0;
	}

	//回包给agv_shell
	nsp::proto::agv_shell_proto_head alive_pkt(kAgvShellProto_KeepAlive_ACk);
	psend(&alive_pkt);

	//解析数据
	char ip_port[128];
	sprintf_s(ip_port, "%s:%d", ep_.ipv4(), ep_.port());
	//取机器人时间
	uint64_t datetime = data->robot_time_;
	struct tm *timeinfo = localtime((time_t*)&datetime) ;
	char r_time[80];
	strftime(r_time, 80, "%Y/%m/%d %I:%M:%S", timeinfo);
	nsp::api::proto_linux_info l_info;
	l_info.robot_time_ = r_time;
	l_info.vcu_enable_ = data->vcu_enable_;

	//回调A core系统时间，VCU使能状态
	unsigned char *linux_buffer = new  unsigned char[l_info.length()];
	l_info.serialize(linux_buffer);
	nsp::toolkit::singleton<operation_manager>::instance()->network_callback_notify(ip_port, Proto_A_Core_Info, 0, (char*)linux_buffer, l_info.length());
	delete[] linux_buffer;

	//取机器人下进程状态
	if (map_process_list_.size() == 0)return 0;
	int status = data->status_;
	nsp::proto::process_status_list pkt;
	for (const auto & iter : map_process_list_)
	{
		nsp::proto::process_status ps;
		ps.process_id_ = nsp::toolkit::singleton<operation_manager>::instance()->get_process_allocat_id(ip_port, iter.first);
		if (ps.process_id_ < -1)
		{
			return 0;
		}
		ps.process_status_ = (status & (1 << iter.first)) != 0 ? 1 : 0;
		pkt.process_list_.push_back(ps);
	}
	//序列化回调给界面
	unsigned char *buffer = new  unsigned char[pkt.length()];
	pkt.serialize(buffer);
	nsp::toolkit::singleton<operation_manager>::instance()->network_callback_notify(ip_port, Proto_Agv_Process_Status, 0, (char*)buffer, pkt.length());
	delete[] buffer;
	return 0;
}

int operation_session::recv_login_ack(const std::shared_ptr<nsp::proto::proto_msg_int>& data)
{
	if (!data) {
		loerror("operation_net") << "failed to build login ack.";
		return 0;
	}

	if (data->msg_int_ == nsp::proto::errorno_t::kItemExist)
	{
		loinfo("operation_net") << "the server has already connect other client,the link is " << this->lnk_;
		//如果连接已经存在，则回调给界面，并断链
		if (connect_callback_)
		{
			connect_callback_(identity_id_, -1, "");
		}
		this->close();
	}
	return 0;
}

int operation_session::recv_file_mutex_status(const std::shared_ptr<nsp::proto::proto_keepalive_status_reponse>&data)
{
	if (!data) {
		loerror("operation_net") << "failed to build file mutex status ack.";
		return 0;
	}

	char ip_port[128];
	sprintf_s(ip_port, "%s:%d", ep_.ipv4(), ep_.port());
	nsp::api::proto_msg_int status;
	status.msg_id = data->status_;

	//序列化回调给界面
	unsigned char *buffer = new  unsigned char[status.length()];
	status.serialize(buffer);
	nsp::toolkit::singleton<operation_manager>::instance()->network_callback_notify(ip_port, kAgvShellProto_FileMutexStatus, 0, (char*)buffer, status.length());
	delete[] buffer;

	return 0;
}

int operation_session::recv_modify_mutex(const std::shared_ptr<nsp::proto::proto_msg_int_sync>&data)
{
	if (!data) {
		loerror("operation_net") << "failed to build modify file mutex status ack.";
		return 0;
	}

	common_msg info;
	if (data->msg_int < 0)
	{
		info.set_err(nsp::proto::errorno_t::kUnsuccessful);
	}
	else
	{
		info.set_err(nsp::proto::errorno_t::kSuccessful);
	}
	info.msg_="";
	return nsp::toolkit::singleton<net_manager>::instance()->exec(data->pkt_id, (char *)&info);
}

int operation_session::recv_shell_version_info(const std::shared_ptr<nsp::proto::proto_shell_version_t>&data)
{
	if (!data) {
		loerror("operation_net") << "failed to build shell version ack.";
		return 0;
	}

	net_status_ = KNetwork_Established;
	char ip_port[128];
	sprintf_s(ip_port, "%s:%d", ep_.ipv4(), ep_.port());
	nsp::api::proto_msg version_msg;
	size_t pos = data->version_.find('\n');
	if (pos != std::string::npos) {
		version_msg.msg = data->version_.substr(0, pos);
	}
	else {
		version_msg.msg = data->version_;
	}

	//序列化回调给界面
	unsigned char *buffer = new  unsigned char[version_msg.length()];
	version_msg.serialize(buffer);
	nsp::toolkit::singleton<operation_manager>::instance()->network_callback_notify(ip_port, kAgvShellProto_ShellVersionInfo, 0, (char*)buffer, version_msg.length());
	delete[] buffer;

	return 0;
}

int operation_session::post_agv_cmd(const int process_id_all, const std::vector<std::string>& vct_parament, const int cmd) {
	if (KNetwork_Disconnect == net_status_)return -1;
	nsp::proto::proto_command_process pkt(kAgvShellProto_ProcessCmd);
	pkt.command_ = cmd;
	pkt.process_id_all_ = process_id_all;
	for (const auto & iter : vct_parament)
	{
		pkt.list_param_.push_back(iter);
	}
	return psend(&pkt);
}

int operation_session::post_frimware_info_request(const FIRMWARE_SOFTWARE_TYPE frimware_type, const std::string& ip_port)
{
	nsp::proto::proto_frimware_info_request pkt(kAgvShellProto_Frimware_Info);
	pkt.frimware_type_ = frimware_type;
	pkt.target_endpoint_ = ip_port;
	return psend(&pkt);
}

int operation_session::post_m_core_transfer_request(const FIRMWARE_SOFTWARE_TYPE frimware_type, const std::string& file_path, 
	const std::string& target_ep)
{
	nsp::proto::proto_frimware_update pkt(kAgvShellProto_Frimware_Update);
	pkt.frimware_type_ = frimware_type;
	pkt.frimware_path_ = file_path;
	pkt.target_endpoint_ = target_ep;
	return psend(&pkt);
}

int operation_session::post_can_custom_info_request(const std::string& ip_port, const int node_id, const int serial_type)
{
	nsp::proto::proto_frimware_custom_update pkt(kAgvShellProto_Frimware_Custom_Info);
	pkt.node_id = node_id;
	pkt.serial_type = serial_type;
	pkt.target_endpoint_ = ip_port;
	return psend(&pkt);
}

int operation_session::post_can_custom_transfer_request(const int node_id, const int serial_type, const std::string& file_path,
	const std::string& target_ep)
{
	nsp::proto::proto_frimware_custom_update pkt(kAgvShellProto_Custom_Update);
	pkt.node_id = node_id;
	pkt.serial_type = serial_type;
	pkt.frimware_path_ = file_path;
	pkt.target_endpoint_ = target_ep;
	return psend(&pkt);
}

int operation_session::post_pull_m_core_request(const FIRMWARE_SOFTWARE_TYPE frimware_type, const int frimware_length, 
	const std::string& target_ep, const std::string& local_path) {

	local_m_coer_path_ = local_path;

	nsp::proto::proto_frimware_download_request pkt(kAgvShellProto_Frimware_Download);
	pkt.frimware_type_ = frimware_type;
	pkt.frimware_length_ = frimware_length;
	pkt.target_endpoint_ = target_ep;
	return psend(&pkt);
}

int operation_session::recv_frimware_info_ack(const std::shared_ptr<nsp::proto::proto_frimware_info_response>&data)
{
	if (!data) {
		loerror("operation_net") << "failed to build frimware information ack.";
		return 0;
	}

	char ip_port[128];
	sprintf_s(ip_port, "%s:%d", ep_.ipv4(), ep_.port());
	FRIMWARE_STATUS status =(FRIMWARE_STATUS)(int)data->frimware_status;
	std::string msg;
	if (status == FRIMWARE_STATUS::kFailReadVCUInfo) {
		msg = FAILED_GET;
	}
	else
	{
		msg = data->frimware_msg_;
	}
	nsp::toolkit::singleton<operation_manager>::instance()->frimware_info_notify(ip_port, (VCU_MESSAGE)(int)data->vcu_type_, status, msg.c_str());
	//更新状态栏
	nsp::toolkit::singleton<operation_manager>::instance()->frimware_transfer_notify(ip_port, (FRIMWARE_STATUS)(int)data->frimware_status, 0);
	 return 0;
}

int operation_session::recv_frimware_transfer_ack(const std::shared_ptr<nsp::proto::proto_frimware_update_response>& data) 
{
	if (!data) {
		loerror("operation_net") << "failed to build frimware transfer ack.";
		return 0;
	}

	char ip_port[128];
	sprintf_s(ip_port, "%s:%d", ep_.ipv4(), ep_.port());
	nsp::toolkit::singleton<operation_manager>::instance()->frimware_transfer_notify(ip_port, (FRIMWARE_STATUS)(int)data->frimware_status, data->frimware_step);
	return 0;
}

int operation_session::recv_frimware_download_ack(const std::shared_ptr<nsp::proto::proto_frimware_download_reponse>& data) {
	nsp::toolkit::singleton<fts_handler>::instance()->pull_m_camera_file(ep_.ipv4(), data->frimware_path_, local_m_coer_path_);
	 return 0;
}

void operation_session::post_agv_shell(const std::string& remote_path, const uint32_t fize)
{
	nsp::proto::proto_msg_str pkt(kAgvShellProto_Upgrade);
	pkt.msg_int_ = fize;
	pkt.msg_str_ = remote_path;
	psend(&pkt);
}

int operation_session::post_restart_m_camera_request(const FIRMWARE_SOFTWARE_TYPE frimware_type, const std::string& target_ep)
{
	nsp::proto::proto_frimware_info_request pkt(kAgvShellProto_Frimware_Restart);
	pkt.frimware_type_ = frimware_type;
	pkt.target_endpoint_ = target_ep;
	return psend(&pkt);
}

void operation_session::post_keepalive()
{
	nsp::proto::agv_shell_proto_head pkt(kAgvShellProto_KeepAlive);
	psend(&pkt);
}

int operation_session::post_get_agv_process_table(const std::shared_ptr<motion::asio_block>&asio_ack)
{
	if (KNetwork_Disconnect == net_status_)return -1;
	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::proto_msg_int pkt(kAgvShellProto_Process_List);
	pkt.msg_int_ = pkt_id;
	return nsp::toolkit::singleton<net_manager>::instance()->write(pkt_id, [&]()->int{
		return psend(&pkt);
	}, asio_ack);
}

int operation_session::post_package_sync(const int type, const void* ser_stream, const int cb, void **ack_msg, int *ack_len)
{
	int res;
	switch (type)
	{
	case Proto_Cmd_List:
	{
		std::string stream = "";
		res = post_request_sync((uint32_t)AgvShellNetProtoType::kAgvShellProto_CmdList, (unsigned char*)stream.c_str(), cb, (unsigned char**)ack_msg, ack_len);
	}
		break;
	case Proto_UpdateCmd:
		res = post_request_sync((uint32_t)AgvShellNetProtoType::kAgvShellProto_UpdateCmdList, (unsigned char*)ser_stream, cb, (unsigned char**)ack_msg, ack_len);
		break;
	case Proto_ModifyMutex:
		res = post_request_sync((uint32_t)AgvShellNetProtoType::kAgvShellProto_ModifyFileMutex, (unsigned char*)ser_stream, cb, (unsigned char**)ack_msg, ack_len);
		break;
	case Proto_BackupDemarcate:
	{
		//查看本次文件是否为标定文件列表中
		/*if (!is_demarcate_file((unsigned char*)ser_stream, cb))
		{
			return 0;
		}*/
		res = post_request_sync((uint32_t)AgvShellNetProtoType::kAgvShellProto_BackupDemarcate, (unsigned char*)ser_stream, cb, (unsigned char**)ack_msg, ack_len);
	}
		break;
	default:
		res = -1;
		break;
	}
	return res;
}

int operation_session::post_package_async(const int type, const void* str, const int cb)
{
	int res;
	switch (type)
	{
	case Proto_UpdateNTPServer:
		res = post_request_async<nsp::api::proto_msg>((uint32_t)AgvShellNetProtoType::kAgvShellProto_UpdateNTP, (unsigned char*)str, cb);
		break;
	case Proto_RunScript:
		res = post_request_async<nsp::api::proto_msg>((uint32_t)AgvShellNetProtoType::kAgvShellProto_RunScript, (unsigned char*)str, cb);
		break;
	case Proto_UpdateCmd:
		res = post_request_async<nsp::api::proto_msg>((uint32_t)AgvShellNetProtoType::kAgvShellProto_UpdateCmdList, (unsigned char*)str, cb);
		break;
	case Proto_VCUEnable:
		res = post_request_async<nsp::api::proto_msg_int>((uint32_t)AgvShellNetProtoType::kAgvShellProto_Set_Keepalive_Status, (unsigned char*)str, cb);
		break;
	default:
		res = -1;
		break;
	}
	return res;
}

int operation_session::post_request_sync(const uint32_t type,  const unsigned char* str,const int cb,unsigned char** reponse,int * out_cb)
{
	if (!str || cb < 0 || !out_cb) return nsp::proto::kInvalidParameter;
	//封装包头
	nsp::proto::agv_shell_proto_head pkt_head;
	pkt_head.type = type;
	unsigned char* head= new unsigned char[pkt_head.length()];
	pkt_head.serialize(head);
	//由于包头部分不包含包ID,故考虑将包ID下发至body数据段部分,需要在传入的字符流前加上包ID
	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	unsigned char* pkt_stream = nullptr;
	int pkt_size;
	try
	{
		pkt_size = cb + 4 + pkt_head.length();
		pkt_stream = new unsigned char[pkt_size];
		memset(pkt_stream, 0, pkt_size);
	}
	catch (...)
	{
		return -1;
	}
	//拷贝包头
	memcpy_s(pkt_stream, sizeof(pkt_head.length()), head, sizeof(pkt_head.length()));
	//拷贝包id
	memcpy_s(pkt_stream + 4, 4, &pkt_id, 4);
	//拷贝字符流
	memcpy_s(pkt_stream + 4 + pkt_head.length(), cb, str, cb);
	//释放head
	delete[] head;

	nsp::os::waitable_handle wait;
	int internal_retval = 0;
	motion::asio_data data_asio;
	common_msg com_msg;
	int res = nsp::toolkit::singleton<net_manager>::instance()->write(
		pkt_id,  [&]()->int {
		if (send(pkt_stream, pkt_size)< 0)
		{
			return nsp::proto::kNoConnected;
		}
		return 0;
		}, std::make_shared<motion::asio_block>([&](const void* data){
		if (!data) {
			internal_retval = -1;
			wait.sig();
			return;
		}
		data_asio = *(motion::asio_data*)data;
		if (data_asio.get_err() != nsp::proto::kSuccessful) {
			internal_retval = data_asio.get_err();
			loerror("operation_net") << "failed to get asio data while post_request_sync package.";
			wait.sig();
			return;
		}
		com_msg = *(common_msg*)data;
		if (com_msg.msg_.size() != 0 && out_cb != nullptr)
		{
			*reponse = new unsigned char[com_msg.msg_.size()];
			memcpy(*reponse, com_msg.msg_.c_str(), com_msg.msg_.size());
			*out_cb = (int)com_msg.msg_.size();
		}
		internal_retval = nsp::proto::kSuccessful;
		wait.sig();
		return;
	}));
	delete[]pkt_stream;
	if (res < 0) {
		return nsp::proto::kUnsuccessful;
	}
	wait.wait();
	return internal_retval;
}

template<class T>
int operation_session::post_request_async(const uint32_t type, const unsigned char* str, const int cb)
{
	if (!str || cb <= 0) return nsp::proto::kInvalidParameter;
	nsp::proto::package<T, PKTTYPE_UNKNOWN> pkt_unkonwn_request;
	if (pkt_unkonwn_request.from_body_stream(str, cb) < 0)
	{
		return nsp::proto::kBuildError;
	}
	pkt_unkonwn_request.type = type;
	return psend(&pkt_unkonwn_request);
}

int operation_session::post_backup_files(const std::vector<std::string>& vct_files)
{
	nsp::proto::proto_logs_file_path pkt(kAgvShellProto_BackupFiles);
	for (const auto iter : vct_files)
	{
		pkt.vct_log_file_name_.push_back(iter);
	}
	return psend(&pkt);
}

void operation_session::set_local_backups_path(const uint32_t id, const std::string& path)
{
	char car_id[32];
	//传入id无效时，则使用path作为路径
	if (_itoa_s(id, car_id, 10) < 0)
	{
		local_backups_path_ = path + "/" + ep_.ipv4() + "/";
	}
	else
	{
		local_backups_path_ = path + "/" + car_id + "_" + ep_.ipv4() + "/";
	}
}

int operation_session::recv_backups_files_ack(const std::shared_ptr<nsp::proto::proto_msg>& data)
{
	if (!data) {
		loerror("operation_net") << "failed to build backup file ack.";
		return 0;
	}

	//截取linux系统文件名
	size_t pos = data->msg_.find_last_of("/");
	std::string local_file = local_backups_path_;
	if (pos != std::string::npos)
	{
		local_file += data->msg_.substr(pos);
	}
	nsp::toolkit::singleton<fts_handler>::instance()->pull_backup_files(ep_.ipv4(), data->msg_, local_file);
	return 0;
}

bool operation_session::is_demarcate_file(const unsigned char* str, const int cb)
{
	nsp::api::proto_msg pkt;
	int size = cb;
	if (pkt.build(str, size) < 0)
	{
		return false;
	}
	bool result = false;
	convert_positive(pkt.msg, '\\', '/');
	size_t pos = pkt.msg.find("./");
	if (pos != std::string::npos)
	{
		std::string file_name = pkt.msg.substr(pos);
		for (const auto iter : nsp::toolkit::singleton<fts_handler>::instance()->get_backup_files())
		{
			if (iter.find(file_name) != std::string::npos)
			{
				result = true;
				break;
			}
		}
	}
	return result;
}

int operation_session::recv_config_version_info(const std::shared_ptr<nsp::proto::proto_shell_version_t>&data)
{
	if (!data) {
		loerror("operation_net") << "failed to build shell version ack.";
		return 0;
	}

	char ip_port[128];
	sprintf_s(ip_port, "%s:%d", ep_.ipv4(), ep_.port());
	nsp::api::proto_msg version_msg;
	size_t pos = data->version_.find('\n');
	if (pos != std::string::npos) {
		version_msg.msg = data->version_.substr(0, pos);
	}
	else {
		version_msg.msg = data->version_;
	}

	//序列化回调给界面
	unsigned char *buffer = new  unsigned char[version_msg.length()];
	version_msg.serialize(buffer);
	nsp::toolkit::singleton<operation_manager>::instance()->network_callback_notify(ip_port, kAgvShellProto_ConfigVersion, 0, (char*)buffer, version_msg.length());
	delete[] buffer;
	return 0;
}