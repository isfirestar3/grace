#include "fts_handler.h"
#include <iostream>
#include "log.h"
#include "rw_xml_file.h"
#include <algorithm>
#include "operation_manager.h"
#include "proto_interface.h"
#include "os_util.hpp"
#include "Shlwapi.h"

//超时等待信号量
nsp::os::waitable_handle timewait_;
//文件状态
file_status f_status_;
//当前正在pull操作的文件
std::string pull_file_;
//当前正在pull操作的对端ip
std::string pull_ip_;
//当前正在pull操作的对端端口号
uint16_t pull_port_;
//回调函数加锁操作
std::mutex callbck_mutex_;
//备份文件所存储的文件路径
std::string backup_file_path_;
//调用fts接口业务类型
Fts_Function_Type fts_type_;

void set_backup_file(const std::string& path)
{
	backup_file_path_ = path;
}

void set_fts_type(const Fts_Function_Type ty)
{
	fts_type_ = ty;
}

struct FIRMWARE_OBJ{
	FIRMWARE_SOFTWARE_TYPE type;
	std::string target_ep;
	//can自主驱动使用以下驱动
	uint8_t node_id;
	uint8_t serial_type;
};

//M core文件列表缓存
std::map<std::string, FIRMWARE_OBJ> map_m_core_;
//agv_shell文件
std::string agv_shell_file_;

int __stdcall fts_usercall(const char *ipaddr, uint16_t port,
	const char *path_local, const char *path_remote,
	int status, int error, int step,
	void *user_data, int user_data_size) {

	//std::cout << "path_local: " << path_local << " step " << step << std::endl;

	f_status_ = (file_status)status;
	switch (status)
	{
		//读出错
	case file_status::file_read_err:
		//写出错
	case file_status::file_write_err:
		if (strcmp(pull_file_.c_str(), path_local) == 0)timewait_.sig();
		break;
	case file_status::transmitting:
		//传输中
		break;
	case file_status::complete:
		//完成
		break;
	default:
		if (strcmp(pull_file_.c_str(), path_local) == 0)timewait_.sig();
		break;
	}

	if (error != 0) {
		loerror("operation_net") << "get error code is " << error << " while callback,the local_file:" << path_local;
	}

	//一键更新进度通知
	if (!agv_shell_file_.empty() && (fts_type_ == Fts_Function_Type::TY_KEY_TO_UPGRADE) 
		&& ( strcmp(agv_shell_file_.c_str(), path_local) == 0)) {
		if (f_status_ == file_status::complete) {
			uint32_t file_size;
			if (path_local) {
				//取本地文件大小
				file_read f_read;
				file_head_info h_info;
				f_read.close_file_handler(f_read.read_file_head_info(path_local, h_info));
				file_size = h_info.total_size_;
			}
			//文件发送完成，通知agv_shell进行解压缩更新包
			nsp::toolkit::singleton<operation_manager>::instance()->post_agv_shell(ipaddr, path_remote, file_size);
			loinfo("operation_net") << "success update patch to agv shell target: " << ipaddr ;
			nsp::toolkit::singleton<fts_handler>::instance()->reduce_core_task_count();
		}
		else if(f_status_ < 0){
			loerror("operation_net") << "failed to update patch to remote target: " << ipaddr << " the error code is " << error;
			nsp::toolkit::singleton<fts_handler>::instance()->reduce_core_task_count();
		}
		return 0;
	}

	//备份文件进度更新通知,排除当前回调为拉取文件可能性
	if (path_local && (fts_type_ == Fts_Function_Type::TY_KEY_BACKUP)
		&& ((std::string)path_local).find(backup_file_path_.c_str()) != std::string::npos)
	{
		if (f_status_ == file_status::complete)
		{
			nsp::toolkit::singleton<operation_manager>::instance()->update_files_progress(ipaddr, f_status_, Proto_Update_Progress);
			loinfo("operation_net") << "update backup file from remote server progress,the file:" << path_local;
		}
		else if (f_status_ < 0)
		{
			loerror("operation_net") << "can not backup file from remote server:" << ipaddr << ":" << port;
		}
		return 0;
	}

	//m核文件通知
	auto iter = map_m_core_.find(path_local);
	if (iter != map_m_core_.end()) {
		char ip_port[128];
		sprintf_s(ip_port, "%s:4410", ipaddr);
		//错误通知
		if (f_status_ == file_status::complete) {
			nsp::toolkit::singleton<operation_manager>::instance()->frimware_transfer_notify(ip_port, FRIMWARE_STATUS::kTranfmited, 0);
			//为can协议自主驱动的情况下
			if (iter->second.type == CAN_CUSTOM_SOFTWARE)
			{
				nsp::toolkit::singleton<operation_manager>::instance()->post_custom_transfer_request(ipaddr, iter->second.node_id,
					iter->second.serial_type, path_remote, iter->second.target_ep);
			}
			else
			{
				nsp::toolkit::singleton<operation_manager>::instance()->post_m_core_transfer_request(ipaddr, iter->second.type, path_remote,
					iter->second.target_ep);
			}
			loinfo("operation_net") << "success push M core file: "<< path_local <<" to remote: " << ipaddr;
			nsp::toolkit::singleton<fts_handler>::instance()->reduce_core_task_count();
		}
		else if (f_status_ < 0) {
			nsp::toolkit::singleton<operation_manager>::instance()->frimware_transfer_notify(ip_port, FRIMWARE_STATUS::kFailDownload, 0);
			nsp::toolkit::singleton<fts_handler>::instance()->reduce_core_task_count();
		}
		return 0;
	}

	//pull操作通知
	if ((strcmp(pull_file_.c_str(), path_local) == 0) && step == 100 && ipaddr && (strcmp(ipaddr, pull_ip_.c_str()) == 0))
	{
		timewait_.sig();
		return 0;
	}

	{
		std::lock_guard<decltype(callbck_mutex_)> lock_call(callbck_mutex_);

		std::string ip_remote, local_path, remote_path;
		uint32_t remote_port, f_status, file_step, file_error;
		ip_remote = ipaddr; local_path = path_local; remote_path = path_remote;
		remote_port = port; f_status = status; file_error = error; file_step = step;

		//push传输文件通知
		nsp::toolkit::singleton<fts_handler>::instance()->update_user_data(ip_remote.c_str(), remote_port, local_path.c_str(), remote_path.c_str(), 
			f_status, file_error, file_step, user_data, user_data_size);
	}

	return 0;
}

std::string utf8_2_gb2312(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	std::string result = str;
	delete[] str;
	return result;
}

void convert_positive(std::string& str, const char preview_cr, const  char new_cr) {
	for (size_t i = 0; i < str.size(); i++) {
		if (str.at(i) == preview_cr) {
			str.at(i) = new_cr;
		}
	}
}

fts_handler::fts_handler() :timer_wait_(1){
	initlization();
}

fts_handler::~fts_handler() {
	uninit();
}

void fts_handler::uninit(){
	timer_wait_.reset();

	if (timeout_check_){
		if (timeout_check_->joinable()){
			timeout_check_->join();
		}
		delete timeout_check_;
		timeout_check_ = nullptr;
	}
	return;
}

void fts_handler::initlization(){
	//注册回fts回调函数
	set_fts_callback(transfer_block_size::block_64kb);
	read_config_file();

	if (timeout_check_){
		return;
	}
	try{
		timeout_check_ = new std::thread(std::bind(&fts_handler::thread_timeout_check, this));
	}
	catch (...){
		loerror("operation_net") << "failed to create timeout check thread.";
	}
	return;
}

void fts_handler::read_config_file() {
	root_path_ = nsp::os::get_module_directory<char>();
	//读取xml文件配置信息
	std::string local_xml_file = root_path_ + "/AGVSetting.xml";

	if (_access(local_xml_file.c_str(), 0) != -1) {
		rw_xml_file rw;
		rw.read_xml_element(local_xml_file, "agv_file_path", "local_standard_path", local_standard_file_setting_);
		rw.read_attribute(local_xml_file, "backups_file", "path", backup_files_);
	}
	if (local_standard_file_setting_.empty()) {
		local_standard_file_setting_ = root_path_ + "/standard";
	}
	local_remote_file_setting_ = nsp::os::get_module_directory<char>() + "/cache";
	convert_positive(local_remote_file_setting_, '\\', '/');
}

void fts_handler::thread_timeout_check(){
	static const int INTERVAL_TIME = 3000;
	while (timer_wait_.wait(INTERVAL_TIME) > 0){
		
		char* data = nullptr;
		uint32_t size = 0;
		{
			std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
			if (map_task_info_.size() == 0)continue;
			size = sizeof(task_group_info) * map_task_info_.size();
			try{
				data = new char[size];
			}
			catch (...){
				continue;
			}
			size_t offset = 0;
			for (auto& iter : map_task_info_){
				task_group_info* ptr = (task_group_info*)&data[offset];
				ptr->task_id_ = iter.first;
				sprintf_s(ptr->target_ep_, "%s:%d", iter.second.ip_.c_str(), iter.second.port_);
				ptr->task_status_ = iter.second.status_;
				ptr->task_step_ = iter.second.step_;
				offset += sizeof(task_group_info);
			}
		}

		if (push_callback_function_){
			push_callback_function_(data, size);
		}
		delete[] data;
		data = nullptr;
	}
}

int fts_handler::update_user_data(const char *ipaddr, uint16_t port, const char *path_local, const char *path_remote, int status, int error, int step, void *user_data, int user_data_size){
	if (NULL == ipaddr || NULL == path_local || NULL == path_remote || port == 0) {
		loerror("operation_net") << "get invaild value in the fts callback function.";
		return -1;
	}

	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	for (auto& iter : map_task_info_){
		//进度为100的文件计数
		int step_complete_count = 0;
		//已发送处理的文件计数，包括完成的，读写失败的文件
		int handl_file_count = 0;
		bool step_update = false;
		//判断任务组状态
		if ( ( iter.second.status_ == file_status::transmitting) && (strcmp(ipaddr, iter.second.ip_.c_str()) == 0)  && (port == iter.second.port_)){
			for (auto& file_iter : iter.second.vct_file){
				if (strcmp(file_iter.remote_file_path_.c_str(), path_remote) == 0){
					file_iter.error_ = error;
					file_iter.status_ = status;
					file_iter.step_ = step;
					//更新任务组中push文件队列数，保证有1-2个文件在push
					if (step == 100){
						loinfo("operation_net") << "status:complete, reduce push file window size,file name:" << path_remote;
						push_task_group_.reduce_push_number();
					}
					//当前任务组中是否有发送错误的文件
					if (status == file_status::file_write_err || status == file_status::file_read_err) {
						loerror("operation_net") << "get an error while push the file:" << path_remote;
						push_task_group_.reduce_push_number();
					}
				}
				if (file_iter.status_ != file_status::transmitting && file_iter.status_ != file_status::wait) {
					handl_file_count++;
				}
				//计算当前任务组中已经完成的文件数目
				if (file_iter.step_ == 100){
					step_update = true;
					step_complete_count++;
				}
			}
			if (step_complete_count == iter.second.vct_file.size())iter.second.status_ = file_status::complete;
			if (iter.second.vct_file.size() == handl_file_count && step_complete_count != handl_file_count) iter.second.status_ = file_status::file_write_err;
			if (step_update) iter.second.step_ = ((double)step_complete_count / iter.second.vct_file.size()) * 100;
		}
	}
	return 0;
}

void fts_handler::set_fts_callback(const transfer_block_size block_t){
	fts_parameter parment;
	switch (block_t)
	{
		case transfer_block_size::block_4kb:
			parment.block_size_pre_transfer = 0x00001000;
			break;
		case transfer_block_size::block_8kb:
			parment.block_size_pre_transfer = 0x00002000;
			break;
		case transfer_block_size::block_16kb:
			parment.block_size_pre_transfer = 0x00004000;
			break;
		case transfer_block_size::block_32kb:
			parment.block_size_pre_transfer = 0x00008000;
			break;
		case transfer_block_size::block_64kb:
			parment.block_size_pre_transfer = 0x00010000;
			break;
		default:
			parment.block_size_pre_transfer = 0x00002000;//默认使用8kb
			break;
	}
	parment.fts_callback = &fts_usercall;
	fts_change_configure(&parment);
}

int fts_handler::set_push_callback(push_callback func){
	if (func){
		push_callback_function_ = func;
		return 0;
	}
	return -1;
}

int fts_handler::set_pull_callback(pull_callback func){
	if (func){
		pull_callback_function_ = func;
		return 0;
	}
	return -1;
}

int fts_handler::get_listdir_all(const char* ipremote, const uint16_t port, const std::string& relative_folder, void** output_stream, int& output_cb){
	char* list_dir = nullptr;
	int dir_cb = 0;
	if (fts_listdir(ipremote, port, relative_folder.c_str(), &list_dir, &dir_cb) == 0)
	{
		std::string data;
		data.assign(list_dir, dir_cb);
		//释放二级指针内内容
		fts_freedir(list_dir);
		//字符转换
		convert_positive(data, '\\', '/');
		//遍历listdir中文件夹
		std::vector<std::string> vct_folders;
		handler_catalog(data, vct_folders);
		nsp::api::proto_msg_list m_list;
		for (const auto& iter : vct_folders)
		{
			nsp::api::proto_msg tmp;
			tmp.msg = utf8_2_gb2312(iter.c_str());
			m_list.msg_list.push_back(tmp);
		}
		output_cb = m_list.length();
		unsigned char *output = new unsigned char[output_cb];
		m_list.serialize(output);
		*output_stream = output;
		return 0;
	}
	return -1;
}

int fts_handler::get_current_catalog(const char* ip, const uint16_t port, const char* input_catalog, char** output_data, int* cb)
{
	std::string input_data = input_catalog;
	convert_positive(input_data, '\\', '/');
	//查看当前ip网络连接状态
	if (nsp::toolkit::singleton<operation_manager>::instance()->get_net_status(ip) < 0) {
		loerror("operation_net") << "the ip : " << ip << " is disconnect, then can not get remote current catalog.";
		return -1;
	}
	loinfo("operation_net") << "start to get current catalog:" << input_data;
	return fts_catalog_info(ip, port, input_data.c_str(), output_data, cb);
}

void fts_handler::handler_catalog(const std::string&data_stream, std::vector<std::string>& vct_folder)
{
	std::string str_tmp;
	str_tmp.assign(data_stream.c_str(), data_stream.size());
	size_t pos = str_tmp.find('\0');
	if (pos == std::string::npos)return;
	std::string root_remote_folder;
	//创建相对根目录
	if (str_tmp.at(pos - 1) != '\0') {
		//为文件夹
		root_remote_folder = str_tmp.substr(0, pos - 1);
		pos++;
	}
	else {
		//为文件
		pos += 2;
		return;
	}
	str_tmp = str_tmp.substr(pos);
	pos = str_tmp.find('\0');
	if (pos == std::string::npos)return;
	//由于各目录结构之间使用"\1\0"或者"\0\0"分割，所以在寻找过程中需要判断\0位置
	do {
		std::string file_tmp;
		if (str_tmp.at(pos - 1) == '\1') {
			//文件夹
			file_tmp = str_tmp.substr(0, pos - 1);
			if (file_tmp.size() >= root_remote_folder.size())
			{
				vct_folder.push_back(file_tmp.substr(root_remote_folder.size()));
			}
			pos++;
		}
		else {
			pos += 2;
		}
		str_tmp = str_tmp.substr(pos);
		pos = str_tmp.find('\0');
	} while (pos != std::string::npos && pos != 0);
	return;
}

void fts_handler::handler_catalog(const std::string&data_stream, std::vector<std::string>& vct_files, std::vector<std::string>& vct_folder)
{
		std::string str_tmp;
		str_tmp.assign(data_stream.c_str(), data_stream.size());
		size_t pos = str_tmp.find('\0');
		if (pos == std::string::npos)return;
		std::string root_remote_folder;
		//创建相对根目录
		if (str_tmp.at(pos - 1) != '\0') {
			//为文件夹
			root_remote_folder = str_tmp.substr(0, pos - 1);
			pos++;
		}
		else {
			//为文件
			pos += 2;
			return;
		}
		str_tmp = str_tmp.substr(pos);
		pos = str_tmp.find('\0');
		if (pos == std::string::npos)return;
		//由于各目录结构之间使用"\1\0"或者"\0\0"分割，所以在寻找过程中需要判断\0位置
		do{
			std::string file_tmp;
			if (str_tmp.at(pos - 1) == '\1'){
				//文件夹
				file_tmp = str_tmp.substr(0, pos - 1);
				if (file_tmp.size() >= root_remote_folder.size())
				{
					vct_folder.push_back(file_tmp.substr(root_remote_folder.size()));
				}
				pos++;
			}
			else{
				//文件
				file_tmp = str_tmp.substr(0, pos);
				if (file_tmp.size() >= root_remote_folder.size())
				{
					vct_files.push_back('.' + file_tmp.substr(root_remote_folder.size()));
				}
				pos += 2;
			}
			str_tmp = str_tmp.substr(pos);
			pos = str_tmp.find('\0');
		} while (pos != std::string::npos && pos != 0);
		return ;
}

int fts_handler::push_files_to_remote(const std::vector<nsp::tcpip::endpoint> vct_ep, const char* str_files, const int cb)
{
	int stream_cb = cb;
	nsp::api::proto_push_list f_list;
	f_list.build((unsigned char*)str_files, stream_cb);

	//字符转换
	convert_positive(f_list.remote_relative_path, '\\', '/');
	std::vector<push_file_info> vct_push;		//存放需要发送的文件名
	task_info t_info;	//任务集合
	for (auto &iter : f_list.local_file_list.msg_list) //遍历查看所有的文件类型，如果存在文件夹，则在文件夹中查看所有的文件
	{
		convert_positive(iter.msg, '\\', '/');	//字符转换
		switch (nsp::os::is_dir(iter.msg))		//返回0为文件，返回1为文件夹
		{
			case 0:
			{
				handler_push_task(iter.msg, file_type::file, f_list.remote_relative_path, vct_push, t_info);
				break;
			}
			case 1:
			{
				if (iter.msg.size() > 0 && iter.msg.at(iter.msg.size() - 1) == '/') iter.msg = iter.msg.substr(0, iter.msg.size() - 1);
				size_t pos = iter.msg.find_last_of('/');
				std::string folder_relative;
				if (pos != std::string::npos)folder_relative = f_list.remote_relative_path + iter.msg.substr(pos);
				handler_push_task(iter.msg, file_type::folder, folder_relative + '/', vct_push, t_info);
				std::vector<std::string> vct_folder;
				if (f_read_.get_floder_files(iter.msg, vct_folder) < 0)return -1;
				//遍历文件夹内文件
				for (auto& folder_iter : vct_folder)
				{
					int folder_type = nsp::os::is_dir(folder_iter);
					std::string remote_path = folder_relative + folder_iter.substr(iter.msg.size());
					if (folder_type == 0)
					{
						remote_path = remote_path.substr(0, remote_path.find_last_of('/'));
						handler_push_task(folder_iter, file_type::file, remote_path, vct_push, t_info);
					}
					else if (folder_type == 1)
					{
						handler_push_task(folder_iter, file_type::folder, remote_path, vct_push, t_info);
					}
				}
			}
			break;
			default:
				loerror("operation_net") << "the file:" << iter.msg << " is not exist.";
				break;
		};
	}
	if (t_info.vct_file.size() == 0) {
		lowarn("operation_net") << "the task group of push file is empty.";
		return -1;
	}
	//将该ip得到的文件信息复制给其他ip，并投递剩余任务组
	for (const auto & iter : vct_ep) {
		//查看当前目标ip地址是否已经在发送
		if (query_task_transfer(iter.ipv4(), iter.port()) < 0)
		{
			loerror("operation_net") << "current target task ip already running,the task ip is : " << iter.ipv4();
			if (vct_ep.size() == 1)return -2;
			continue;
		}
		//查看当前ip网络连接状态
		if (nsp::toolkit::singleton<operation_manager>::instance()->get_net_status(iter.ipv4()) < 0) {
			loerror("operation_net") << "the ip : " << iter.ipv4() << " is disconnect, then can not push files to remote.";
			continue;
		}
		t_info.ip_ = iter.ipv4();
		t_info.port_ = iter.port();
		uint64_t task_id = push_task_group_.get_random_id();
		{
			std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
			map_task_info_.insert(std::make_pair(task_id, t_info));
		}
		push_task_group_.add_push_task(t_info.ip_.c_str(), t_info.port_, vct_push, task_id);
	}
	return 0;
}

void fts_handler::handler_push_task(const std::string& local_file, const file_type file_ty, const std::string& remote_file, std::vector<push_file_info>& vct_push, task_info& task_i)
{
	push_file_info p_file;
	p_file.local_file = local_file;
	task_file_info t_file;				//任务文件信息
	file_head_info head_info;	//文件头信息
	switch (file_ty)
	{
		case file_type::file:
			{
				size_t pos = local_file.find_last_of('/');
				if (pos != std::string::npos)p_file.remote_file = remote_file + local_file.substr(pos);
				f_read_.close_file_handler(f_read_.read_file_head_info(local_file, head_info));
				t_file.file_size_ = head_info.total_size_;
				t_file.modify_datetime_ = head_info.last_write_time_;
			}
			break;
		case file_type::folder:
			p_file.remote_file = remote_file;
			f_read_.read_folder_info(local_file, head_info);
			t_file.file_size_ = 0;
			t_file.modify_datetime_ = head_info.last_write_time_;
			break;
		default:
			break;
	}
	t_file.local_file_path_ = p_file.local_file;
	t_file.remote_file_path_ = p_file.remote_file;
	vct_push.push_back(p_file);				//加入发送队列中
	task_i.vct_file.push_back(t_file);		//加入此次任务的队列信息中
}

int fts_handler::query_task_transfer(const std::string& ip, const uint16_t port)
{
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	for (const auto& iter : map_task_info_)
	{
		if (iter.second.ip_ == ip && iter.second.port_ == port && iter.second.status_ == file_status::transmitting)
		{
			return -1;
		}
	}
	return 0;
}

int fts_handler::delete_remote_files(const char* ip, uint16_t port, const char* str_files, const int cb) {
	int stream_cb = cb;
	nsp::api::proto_file_list file_list;
	file_list.build((unsigned char*)str_files, stream_cb);

	//遍历调用接口删除
	int result = 0;
	for (auto& file_iter : file_list.file_list) {
		convert_positive(file_iter.file_name, '\\', '/');
		//去除文件字符串最后一个斜杠字符
		if (file_iter.file_type == file_type::file && file_iter.file_name.size() > 2 && file_iter.file_name.at(file_iter.file_name.size() - 1) == '/')
		{
			file_iter.file_name = file_iter.file_name.substr(0, file_iter.file_name.size() - 1);
		}
		if (fts_delete_file(ip, port, file_iter.file_name.c_str()) < 0) {
			result = -1;
			loerror("operation_net") << "failed to delete file:" << file_iter.file_name;
			continue;
		}
	}
	return result;
}

int fts_handler::get_file_crc(const char* input_file, unsigned int* crc_value){
	if (!input_file || !crc_value){
		return -1;
	}
	std::string dir = input_file;
	convert_positive(dir, '\\', '/');
	f_read_.cala_crc(dir.c_str(), *crc_value);
	return 0;
}

int fts_handler::recreate_file_path(const std::string& file_name)
{
	std::string tmp_file = file_name;
	convert_positive(tmp_file, '\\', '/');

	size_t pos = tmp_file.find("/");
	if (pos == std::string::npos)return -1;
	std::string temp_path = tmp_file.substr(pos + 1);//去掉盘符如（"E:\\")
	std::string root_path = tmp_file.substr(0, pos + 1);
	pos = temp_path.find("/");
	if (pos == std::string::npos)return -1;
	do{
		std::string path = root_path + temp_path.substr(0, pos);
		if (nsp::os::mkdir(path) < 0){
			return -1;
		}
		root_path = path + "/";
		temp_path = temp_path.substr(pos + 1, temp_path.length() - pos - 1);
		pos = temp_path.find("/");
	} while (pos != std::string::npos);
	return 0;
}

int fts_handler::get_push_files_detail(const uint64_t task_id, void** data_ptr, int& cb) {
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	auto iter = map_task_info_.find(task_id);
	if (iter == map_task_info_.end()) {
		loerror("operation_net") << "can not find task in the task group,the id is " << task_id;
		return -1;
	}

	//数据序列化
	char* file_data = nullptr;
	try {
		cb = sizeof(task_file_detail_info) * iter->second.vct_file.size();
		file_data = new char[cb];
	}
	catch (...) {
		return -1;
	}

	size_t offset = 0;
	for (auto& file_iter : iter->second.vct_file) {
		task_file_detail_info* file_info = (task_file_detail_info*)&file_data[offset];
		strcpy_s( file_info->file_name_,sizeof(file_info->file_name_), file_iter.remote_file_path_.c_str());
		strcpy_s(file_info->file_modify_time_, sizeof(file_info->file_modify_time_), file_iter.modify_datetime_.c_str());
		file_info->file_size_ = file_iter.file_size_;
		file_info->file_status_ = file_iter.status_;
		file_info->file_step_ = file_iter.step_;
		offset += sizeof(task_file_detail_info);
	}

	*data_ptr = file_data;
	return 0;
}

int fts_handler::repeate_push_files(const uint64_t task_id, const char* files, int cb) {
	std::string file_tmp = files;
	std::vector<std::string> vct_input;
	convert_positive(file_tmp, '\\', '/');
	nsp::toolkit::slicing_symbol_string(file_tmp, '|', vct_input);
	if (vct_input.size() == 0)return -1;
		
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	auto iter = map_task_info_.find(task_id);
	if (iter == map_task_info_.end()) {
		loerror("operation_net") << "can not find task in the task group,the id is " << task_id;
		return -1;
	}

	//查看该任务组的ip是否断链
	if (nsp::toolkit::singleton<operation_manager>::instance()->get_net_status(iter->second.ip_) < 0) {
		loerror("operation_net") << "the ip : "<< iter->second.ip_ << " is disconnect, then can not repeate push files to remote.";
		return -1;
	}

	//存放需要发送的文件名
	std::vector<push_file_info> vct_push;
	for (auto& input : vct_input) {
		auto file_iter = std::find_if(iter->second.vct_file.begin(), iter->second.vct_file.end(), [&](const task_file_info& t_info) ->bool{
			return t_info.remote_file_path_ == input;
		});
		if (file_iter != iter->second.vct_file.end()) {
			file_iter->step_ = 0;
			file_iter->error_ = 0;
			file_iter->status_ = file_status::transmitting;//更改文件状态
			push_file_info p_tmp;
			p_tmp.local_file = file_iter->local_file_path_;
			p_tmp.remote_file = file_iter->remote_file_path_;
			//更改该任务组传输状态
			iter->second.status_ = file_status::transmitting;//更改任务状态
			vct_push.push_back(p_tmp);
		}
	}
	//投递任务组队列中
	if(vct_push.size() != 0)push_task_group_.add_push_task(iter->second.ip_.c_str(), iter->second.port_, vct_push, iter->first);
	return 0;
}

//更新任务组状态为正在正在传输
void fts_handler::update_task_group_status(const uint64_t task_id, const int status) {
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	auto iter = map_task_info_.find(task_id);
	if (iter != map_task_info_.end()) {
		if (iter->second.status_ == file_status::wait) {
			iter->second.status_ = status;
		}
	}
}

void fts_handler::restore_factory_setting(const std::vector<std::string> vct_ip) {
	std::string folder_file = "./";
	for (size_t i = 0; i < vct_ip.size(); i++) {
		nsp::tcpip::endpoint ep;
		if (nsp::tcpip::endpoint::build(vct_ip[i], ep) < 0) {
			loerror("operation_net") << "failed to build endpoint of " << vct_ip[i];
			if (restore_callback_) {
				restore_callback_((double)i / (double)vct_ip[i].size() * 100, -1, vct_ip[i].c_str());
			}
			continue;
		}

		if (fts_delete_file(ep.ipv4(), ep.port(), folder_file.c_str()) < 0) {
			if (restore_callback_) {
				restore_callback_((double)i / (double)vct_ip[i].size() * 100, -1, ep.ipv4());
			}
			loerror("operation_net") << "failed to restore factory setting,then can not delete file:" << folder_file;
			continue;
		}
		loinfo("operation_net") << "restore factory setting of remote :" << ep.ipv4() << " successfully.";

		//成功，回调给调用者
		if (restore_callback_) {
			restore_callback_((double)i / (double)vct_ip[i].size() * 100, 0, ep.ipv4());
		}
	}
}

void fts_handler::regiset_restore_setting(void(__stdcall *notify_callback)(int32_t step, int32_t status, const char* ip)) {
	restore_callback_ = notify_callback;
}

int fts_handler::pull_files_from_remote(const char* ip, const uint16_t port, const char* str_files, const int cb, const char* save_path) {
	int stream_cb = cb;
	nsp::api::proto_file_list file_list;
	file_list.build((unsigned char*)str_files, stream_cb);

	//遍历找所有文件，如果是文件夹，则更新文件夹内所有文件
	std::vector<std::string> vct_total_files;
	std::vector<std::string> vct_total_folders;
	for (auto& iter : file_list.file_list)
	{
		convert_positive(iter.file_name, '\\', '/');
		if (file_type::file == iter.file_type)//为文件
		{
			if (iter.file_name.size() > 0 && iter.file_name.at(iter.file_name.size() - 1) == '/')iter.file_name = iter.file_name.substr(0, iter.file_name.size() - 1);
			vct_total_files.push_back(iter.file_name);
		}
		else//为目录
		{
			char* list_dir = nullptr;
			int dir_cb = 0;
			std::string file_path = iter.file_name;
			if (file_path.size() > 2 && file_path.at(file_path.size() - 1) == '/')file_path = file_path.substr(0, file_path.size() - 1);
			if (fts_listdir(ip, port, file_path.c_str(), &list_dir, &dir_cb) == 0)
			{
				std::string data_stream;
				data_stream.assign(list_dir, dir_cb);
				//释放二级指针
				fts_freedir(list_dir);
				handler_catalog(data_stream, vct_total_files, vct_total_folders);
			}
		}
	}
	std::string local_path;
	if (strcmp(save_path, "") == 0) local_path = local_remote_file_setting_;
	else local_path = save_path;

	convert_positive(local_path, '\\', '/');
	if (!local_path.empty() && local_path.at(local_path.size() - 1) != '/') local_path += '/';
	//创建文件夹
	for (const auto& iter : vct_total_folders)  
	{
		std::string folder_path = local_path + iter;
		convert_positive(folder_path, '\\', '/');
		if (folder_path.size() > 0 && folder_path.at(folder_path.size() - 1) != '/') folder_path += '/';
		recreate_file_path(folder_path);
	}

	pull_ip_ = ip;
	pull_port_ = port;
	is_pull_exist_ = false;
	//循环下载，并返回状态，进度
	int result = 0;
	for (size_t i = 0; i < vct_total_files.size(); i++) {
		//去除相对路径中'.'开始的情况
		if (vct_total_files[i].size() > 1 && vct_total_files[i].substr(0,2) == "./")
		{
			pull_file_ = local_path + vct_total_files[i].substr(2);
		}
		else
		{
			pull_file_ = local_path + vct_total_files[i];
		}
		//字符转换
		convert_positive(pull_file_,'\\','/');
		std::string remote_file = vct_total_files[i].c_str();
		if (remote_file.at(0) != '.') remote_file = '.' + remote_file;
		//调用接口拉文件
		fts_pull(ip, port, pull_file_.c_str(), remote_file.c_str(), 0);
		//下载等待
		if (timewait_.wait(-1) == WAIT_OBJECT_0);
		//判断当前是否取消pull操作，如果存在，则取消所有下载操作
		if (is_pull_exist_) {
			result = -1;
			break;
		}
		//回调通知上层，当前进度，状态
		if (pull_callback_function_) {
			pull_callback_function_(f_status_, (double(i + 1) / vct_total_files.size()) * 100);
		}
		//判断当前文件状态传输是否正常，如果发现文件传输错误，则直接删除文件夹内所有内容，并返回-1
		if ( f_status_ == file_status::file_write_err || f_status_ == file_status::file_read_err) {
			result = -1;
			break;
		}
	}

	//清空操作
	pull_file_.clear();
	pull_ip_.clear();
	pull_port_ = 0;
	return result;
}

void fts_handler::update_folder_config() {
	read_config_file();
}

void fts_handler::update_disconnect_status(const std::string& ip, const uint16_t port) {
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	for (auto& iter : map_task_info_) {
		if (iter.second.ip_ == ip  && iter.second.status_ == file_status::transmitting) {
			iter.second.status_ = file_status::network_error;
			for (auto& file_iter : iter.second.vct_file) {
				//所有正在传输的文件状态全部转换成网络错误状态
				if (file_iter.status_ == file_status::transmitting) {
					file_iter.status_ = file_status::network_error;
					file_iter.step_ = 0;
				}
			}
			//删除任务组中发送任务
			push_task_group_.remove_push_task(iter.first);
		}
	}
}

int fts_handler::cancel_pull_opreat() {
	if (pull_ip_.empty() || pull_port_ == 0) {
		loerror("operation_net") << "[cancel pull]the current of ip is invaild,the ip is " << pull_ip_ << " ,the port is " << pull_port_;
		return -1;
	}
	is_pull_exist_ = true;
	//取消当前pull文件操作
	fts_cancel(pull_ip_.c_str(), pull_port_);
	//发送信号
	timewait_.sig();
	return 0;
}

int fts_handler::push_m_camera_file(const std::vector<nsp::tcpip::endpoint>& vct_ep, const FIRMWARE_SOFTWARE_TYPE ty,
	const std::string& file_path, const std::string& target_ep) {
	//判断文件是否存在，如果不存在该文件，直接返回失败
	if (!PathFileExistsA(file_path.c_str()))
	{
		loerror("operation_net") << "m core file is not exists.";
		return -1;
	}
	auto iter = map_m_core_.find(file_path);
	if (iter == map_m_core_.end()) {
		FIRMWARE_OBJ f_obj;
		f_obj.type = ty;
		f_obj.target_ep = target_ep;
		map_m_core_.insert(std::make_pair(file_path, f_obj));
	}
	else
	{
		//如果存在，则更新内部信息
		iter->second.target_ep = target_ep;
		iter->second.type = ty;
	}
	std::string dir = file_path;
	convert_positive(dir, '\\', '/');
	size_t pos = dir.find_last_of('/');
	if (pos == std::string::npos) return -1;
	std::string remote_file = "../m_core/" + dir.substr(pos + 1);

	for (const auto& iter : vct_ep)
	{
		//此处测试预先固定写死4410端口
		char ip_port[128];
		sprintf_s(ip_port, "%s:4410", iter.ipv4());
		nsp::toolkit::singleton<operation_manager>::instance()->frimware_transfer_notify(ip_port, FRIMWARE_STATUS::kTranfmiting, 0);
		f_core_task_.add_push_core_task(iter.ipv4(), iter.port(), file_path, remote_file);
	}
	return 0;
}

int fts_handler::post_can_custom_file(const std::vector<nsp::tcpip::endpoint>& vct_ep, const uint32_t node_id, const uint32_t serial_type,
	const std::string& file_path, const std::string& target_ep)
{
	//判断文件是否存在，如果不存在该文件，直接返回失败
	if (!PathFileExistsA(file_path.c_str()))
	{
		loerror("operation_net") << "custom bin file is not exists.";
		return -1;
	}
	auto iter = map_m_core_.find(file_path);
	if (iter == map_m_core_.end()) {
		FIRMWARE_OBJ f_obj;
		f_obj.type = CAN_CUSTOM_SOFTWARE;
		f_obj.target_ep = target_ep;
		f_obj.node_id = node_id;
		f_obj.serial_type = serial_type;
		map_m_core_.insert(std::make_pair(file_path, f_obj));
	}
	else
	{
		//如果找到，则直接更新node_id,serial code两个参数
		iter->second.node_id = node_id;
		iter->second.serial_type = serial_type;
		iter->second.target_ep = target_ep;
	}
	std::string dir = file_path;
	convert_positive(dir, '\\', '/');
	size_t pos = dir.find_last_of('/');
	if (pos == std::string::npos) return -1;
	std::string remote_file = "../m_core/" + dir.substr(pos + 1);

	for (const auto& iter : vct_ep)
	{
		//此处测试预先固定写死4410端口
		char ip_port[128];
		sprintf_s(ip_port, "%s:4410", iter.ipv4());
		nsp::toolkit::singleton<operation_manager>::instance()->frimware_transfer_notify(ip_port, FRIMWARE_STATUS::kTranfmiting, 0);
		loinfo("operation_net") << "push can custom bin file:"<< iter.to_string();
		f_core_task_.add_push_core_task(iter.ipv4(), iter.port(), file_path, remote_file);
	}
	return 0;
}

void fts_handler::pull_m_camera_file(const std::string& remote_ip, const std::string& remote_file, const std::string& local_path) {
	size_t pos = remote_file.find_last_of('/');
	std::string local_file = pos != std::string::npos ? local_path + "/" + remote_ip + remote_file.substr(pos) : local_path + "/" + remote_ip + "file.bin";
	frimware_pull_task_.add_pull_task(remote_ip.c_str(), 4411, remote_file, local_file);
}

int fts_handler::push_agv_file(const std::string ipv4, const std::string& file_path) {
	agv_shell_file_ = file_path;
	convert_positive(agv_shell_file_, '\\', '/');
	size_t pos = agv_shell_file_.find_last_of('/');
	std::string remote_file;
	if (pos != std::string::npos) {
		remote_file = "." + agv_shell_file_.substr(pos);
	}
	else {
		remote_file = "./agv_shell_new";
	}
	f_core_task_.add_push_core_task(ipv4.c_str(), 4411, agv_shell_file_, remote_file);
	return 0;
}

void fts_handler::pull_backup_files(const std::string& remote_ip, const std::string& remote_file, const std::string& local_file)
{
	//使用固件获取的任务类，该任务类可以通用
	frimware_pull_task_.add_pull_task(remote_ip.c_str(), 4411, remote_file, local_file);
}

void fts_handler::reduce_core_task_count()
{
	f_core_task_.reduce_push_count();
}