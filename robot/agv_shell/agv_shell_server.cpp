#include "agv_shell_server.h"
#include "frimware_task.h"
#include "file_read_handler.h"
#include "file_can_read.h"
#include "gzfts_api.h"
#include <algorithm>
#include "agv_shell_common.h"

#define NET_WINDOW_SIZE     1

agv_shell_server::agv_shell_server() {
	initlization();
}

agv_shell_server::~agv_shell_server() {
	uinit();
}

void agv_shell_server::initlization(){
	if (!frimware_push_spool_ && !frimware_info_spool_ && !frimware_download_spool){
		try{
			frimware_push_spool_ = std::make_shared<nsp::toolkit::task_thread_pool<frimware_update_task>>(NET_WINDOW_SIZE);
			frimware_info_spool_ = std::make_shared<nsp::toolkit::task_thread_pool<frimware_get_info_task>>(NET_WINDOW_SIZE);
			frimware_download_spool = std::make_shared<nsp::toolkit::task_thread_pool<frimware_download_task>>(NET_WINDOW_SIZE);
			frimware_restart_spool = std::make_shared<nsp::toolkit::task_thread_pool<frimware_restart_task>>(NET_WINDOW_SIZE);
			base_task_spool = std::make_shared<nsp::toolkit::task_thread_pool<base_task>>(NET_WINDOW_SIZE);
		}
		catch (...){
			loerror("agv_shell") << "failed to create frimware spool.";
			return;
		}
	}

	if (!alive_check_)
	{
		try
		{
			alive_check_ = new std::thread(std::bind(&agv_shell_server::alive_check_thread, this));
		}
		catch (...)
		{
			loerror("agv_shell") << "failed to create alive check thread.";
			return;
		}
	}
	//设置fts回调函数，此函数用于M核固件下载给客户端时，如果agv_shell将固件文件push给客户端的回调处理
	/*fts_parameter parment;
	parment.block_size_pre_transfer = 0x00002000;
	parment.fts_callback = &fts_usercall;
	fts_change_configure(&parment);*/
}

void agv_shell_server::uinit(){
	if (frimware_push_spool_) frimware_push_spool_->join();
	if (frimware_info_spool_) frimware_info_spool_->join();
	if (frimware_download_spool) frimware_download_spool->join();
	if (frimware_restart_spool) frimware_restart_spool->join();

	//安全退出线程
	is_exist_ = 1;
	check_wait_.sig();
	if (alive_check_)
	{
		if (alive_check_->joinable())
		{
			alive_check_->join();
		}
		delete alive_check_;
		alive_check_ = nullptr;
	}
	check_wait_.reset();
}

void agv_shell_server::alive_check_thread()
{
	const int ALIVE_TIMEOUT_INTERVAL = 2000;
	const int ALIVE_MAX_COUNT = 5;
	while (check_wait_.wait(ALIVE_TIMEOUT_INTERVAL))
	{
		if (is_exist_ == 1)break;

		if (service_)
		{
			service_->notify_all([&](const std::shared_ptr<agv_shell_session> &client) {
				if (client->get_alive_count() > ALIVE_MAX_COUNT){
					loerror("agv_shell") << "get a timeout then close session, the remote endpoint is " << client->get_remote_endpoint().to_string() << " the lnk is " << client->get_link();
					client->close();
				}
				else
				{
					client->add_alive_count();
				}
			});
		}
	}
}

int agv_shell_server::begin_shell_server(const int port, std::string ip) {
	if (service_) {
        return 0;
    }

    try {
        service_ = std::make_shared<nsp::tcpip::tcp_application_service < agv_shell_session >> ();
    } catch (...) {
        return -1;
    }
	ep_.ipv4(ip);
	ep_.port(port);

    if (service_->begin(ep_) < 0) {
        loerror("agv_shell") << "failed to begin,agv shell server.";
        return -1;
    }

    return 0;
}


int agv_shell_server::on_get_frimware_info(const uint32_t link, const int frimware_ty, const std::string& target_ep,
	const uint32_t node_id, const uint32_t serial_type){
	std::shared_ptr<frimware_get_info_task> task = nullptr;
	try{
		task = std::make_shared<frimware_get_info_task>(link, (FIRMWARE_SOFTWARE_TYPE)frimware_ty, target_ep, node_id, serial_type);
	}
	catch (...)
	{
		loerror("agv_shell") << "failed to make shared task of get frimware info.";
		return -1;
	}
	if (!frimware_info_spool_){
		loerror("agv_shell") << "the frimware spool is not exists.";
		return 0;
	}
	frimware_info_spool_->post(task);
	return 0;
}

int agv_shell_server::post_frimware_info(const uint32_t link, const FIRMWARE_SOFTWARE_TYPE f_type, const FRIMWARE_STATUS status,
	const VCU_MESSAGE msg_ty, const std::string& msg){
	std::shared_ptr<agv_shell_session> client_session_ = nullptr;
	if (service_ && (service_->search_client_by_link(link, client_session_) >= 0)){
		if (client_session_){
			return client_session_->post_vcu_info(f_type, status, msg_ty, msg);
		}
	}
	loerror("agv_shell") << "can not find agv_shell client session.";
	return -1;
}

int agv_shell_server::on_frimware_update(const uint32_t link, const int f_ty, const std::string& file_path, const std::string& target_ep,
	const uint32_t node_id, const uint32_t serial_type){
	std::string file;
	if (file_path.size() > 3 && (file_path.substr(0, 3) == "../")){
		size_t pos = work_path_.find_last_of('/');
		if (pos != std::string::npos)
		{
			file = work_path_.substr(0, pos) + '/' + file_path.substr(3);
		}
		else
		{
			file = file_path;
		}
	}
	else
	{
		file = file_path;
	}
	//此处加延时，由于网络传输速度快，而fts并没有将写完的数据刷新至内存，故有可能在打开文件时，获取的文件数据为0
	nsp::os::waitable_handle waiter;
	waiter.wait(100);
	if (f_ty == FIRMWARE_SOFTWARE_TYPE::CAN_CUSTOM_SOFTWARE)
	{
		if (nsp::toolkit::singleton<file_can_read>::instance()->open_file(file) < 0)
		{
			post_frimware_update(link, (FIRMWARE_SOFTWARE_TYPE)f_ty, FRIMWARE_STATUS::kFailUpdate, 0);
			return -1;
		}
	}
	else
	{
		if (nsp::toolkit::singleton<file_read_handler>::instance()->open_file(file) < 0){
			post_frimware_update(link, (FIRMWARE_SOFTWARE_TYPE)f_ty, FRIMWARE_STATUS::kFailUpdate, 0);
			return -1;
		}
	}

	std::shared_ptr<frimware_update_task> task = nullptr;
	try{
		task = std::make_shared<frimware_update_task>(link, (FIRMWARE_SOFTWARE_TYPE)f_ty, target_ep, node_id, serial_type);
	}
	catch (...)
	{
		loerror("agv_shell") << "failed to make shared task of update frimware.";
		return -1;
	}
	if (!frimware_push_spool_){
		loerror("agv_shell") << "the frimware spool is not exists.";
		return 0;
	}
	frimware_push_spool_->post(task);
	return 0;
}

int agv_shell_server::on_frimware_download(const uint32_t link, const int frimware_ty, const int frimware_length, const std::string& target_ep){
	std::string file_name;
	switch (frimware_ty)
	{
	case BOOT_SOFTWARE:
		file_name = "boot.bin";
		break;
	case MAIN_SOFTWARE:
		file_name = "main.bin";
		break;
	case USER_SOFTWARE:
		file_name = "user.bin";
		break;
	case CAMERA_SOFTWARE:
		file_name = "camera.bin";
		break;
	default:
		file_name = "file.bin";
		break;
	}

	std::shared_ptr<frimware_download_task> task = nullptr;
	try{
		std::string file_path;
		size_t pos = work_path_.find_last_of('/');
		if (pos != std::string::npos)
		{
			file_path = work_path_.substr(0, pos) + '/' + file_name;
		}
		else
		{
			file_path = work_path_ + '/' + file_name;
		}
		task = std::make_shared<frimware_download_task>(link, (FIRMWARE_SOFTWARE_TYPE)frimware_ty, frimware_length, file_path, target_ep);
	}
	catch (...)
	{
		loerror("agv_shell") << "failed to make shared task of download frimware bin file.";
		return -1;
	}
	if (!frimware_download_spool)
	{
		loerror("agv_shell") << "the frimware spool is not exists.";
		return 0;
	}
	frimware_download_spool->post(task);
	return 0;
}

int agv_shell_server::post_frimware_download(const uint32_t link, const std::string& file_path){
	std::shared_ptr<agv_shell_session> client_session_ = nullptr;
	if (service_ && (service_->search_client_by_link(link, client_session_) >= 0)){
		if (client_session_){
			nsp::tcpip::endpoint target_ep = client_session_->remote();
			loinfo("agv_shell") << "post frimware file to client,the target endpoint is " << target_ep.to_string();

			size_t work_pos = work_path_.find_last_of('/');
			size_t pos = file_path.find_last_of('/');
			std::string file_remote;
			if (work_pos != std::string::npos)
			{
				file_remote = pos != std::string::npos ? file_remote = work_path_.substr(0, work_pos) + file_path.substr(pos) : "./file.bin";
			}
			else file_remote = "./file.bin";
			post_frimware_download_complete(link, file_remote);
			return 0;
		}
	}
	return -1;
}

void agv_shell_server::post_frimware_download_complete(const uint32_t link, const std::string& file_path){
	if (service_){
		service_->notify_all([&](const std::shared_ptr<agv_shell_session> &client) {
			if (client->get_link() == link){
				client->post_frimware_download_ack(file_path);
				return;
			}
		});
	}
}

int agv_shell_server::post_frimware_update(const uint32_t link, const FIRMWARE_SOFTWARE_TYPE f_type, const FRIMWARE_STATUS status, const int step){
	std::shared_ptr<agv_shell_session> client_session_ = nullptr;
	if (service_ && (service_->search_client_by_link(link, client_session_) >= 0)){
		if (client_session_){
			return client_session_->post_frimware_update(f_type, status, step);
		}
	}
	loerror("agv_shell") << "can not find agv_shell client session.";
	return -1;
}

int agv_shell_server::on_frimware_restart(const uint32_t link, const int frimware_ty, const std::string& target_ep){
	std::shared_ptr<frimware_restart_task> task = nullptr;
	try{
		task = std::make_shared<frimware_restart_task>(link, (FIRMWARE_SOFTWARE_TYPE)frimware_ty, target_ep);
	}
	catch (...)
	{
		loerror("agv_shell") << "failed to make shared task of restart frimware.";
		return -1;
	}
	if (!frimware_restart_spool){
		loerror("agv_shell") << "the frimware restart spool is not exists.";
		return 0;
	}
	frimware_restart_spool->post(task);
	return 0;
}

void agv_shell_server::on_query_vcu_keep_alive_status(const uint32_t link){
	std::shared_ptr<query_keepalive_status_task> task = nullptr;
	try{
		task = std::make_shared<query_keepalive_status_task>(link);
		auto base_one = std::static_pointer_cast<base_task>(task);
		if (!base_task_spool){
			loerror("agv_shell") << "the frimware restart spool is not exists.";
			return;
		}
		base_task_spool->post(base_one);
	}
	catch (...)
	{
		loerror("agv_shell") << "failed to make shared task of query vcu keepalive status.";
		return;
	}
	return;
}
void agv_shell_server::on_set_vcu_keep_alive_status(const uint32_t link, int status){
	std::shared_ptr<set_keepalive_status_task> task = nullptr;
	try{
		task = std::make_shared<set_keepalive_status_task>(link, status);
		auto base_one = std::static_pointer_cast<base_task>(task);
		if (!base_task_spool){
			loerror("agv_shell") << "the frimware restart spool is not exists.";
			return;
		}
		base_task_spool->post(base_one);
	}
	catch (...)
	{
		loerror("agv_shell") << "failed to make shared task of set vcu keepalive status.";
		return;
	}
	return ;
}
int agv_shell_server::post_vcu_keep_alive_status(const uint32_t link, const int proto_type, int status) {
	std::shared_ptr<agv_shell_session> client_session_ = nullptr;
	if (service_ && (service_->search_client_by_link(link, client_session_) >= 0)){
		if (client_session_){
			return client_session_->post_keepalive_status_ack(proto_type, status);
		}
	}
	loerror("agv_shell") << "can not find agv_shell client session.";
	return -1;
}

/*void agv_shell_server::on_deal_process_cmd(const HTCPLINK link, int cmd, int process_id) {
	std::shared_ptr<deal_process_cmd_task> task = nullptr;
	try{
		task = std::make_shared<deal_process_cmd_task>(link, cmd, process_id);
		auto base_one = std::static_pointer_cast<base_task>(task);
		if (!base_task_spool){
			loerror("agv_shell") << "the base task spool is not exists.";
			return;
		}
		base_task_spool->post(base_one);
	}
	catch (...)
	{
		loerror("agv_shell") << "failed to make shared task of deal process cmd task.";
		return;
	}
	return ;
}*/

void agv_shell_server::on_deal_process_cmd(const HTCPLINK link, const std::shared_ptr<nsp::proto::proto_command_process> p_info) {
	std::shared_ptr<deal_process_cmd_task> task = nullptr;
	try{
		loinfo("agv_shell") << "get handler process list command,now create a task.";
		task = std::make_shared<deal_process_cmd_task>(link, p_info->command_, p_info->process_id_all_);
		auto iter = p_info->list_param_.begin();
		while (iter != p_info->list_param_.end() ) {
			task->add_cmd_param( *iter );
			++iter;
		}
		auto base_one = std::static_pointer_cast<base_task>(task);
		if (!base_task_spool){
			loerror("agv_shell") << "the base task spool is not exists.";
			return;
		}
		base_task_spool->post(base_one);
	}
	catch (...)
	{
		loerror("agv_shell") << "failed to make shared task of deal process cmd task.";
		return;
	}
	return ;
}

void agv_shell_server::close_all() {
	if (service_) {
    service_->notify_all([&](const std::shared_ptr<agv_shell_session>&client){
        client->close();
    });
    service_->close();
    loerror("agv_shell") << "agv_shell_session closed.";
	}
}

void agv_shell_server::post_notify_all(const nsp::proto::proto_interface& package) {
    if (service_) {
		service_->notify_all([&](const std::shared_ptr<agv_shell_session>&client){
			if (client->psend(&package) < 0)
			{
				loerror("agv_shell") << "failed to send status to client,then close the session";
				client->close();
			}
		});
	}
}

void agv_shell_server::post_file_mutex(const int status)
{
	if (service_) {
		service_->notify_all([&](const std::shared_ptr<agv_shell_session>&client){
			if (client->post_file_status(status) < 0)
			{
				loerror("agv_shell") << "failed to send file lock mutex to client,then close the session";
				client->close();
			}
		});
	}
}

void agv_shell_server::post_shell_version(const std::string &version)
{
	if (service_) {
		service_->notify_all([&](const std::shared_ptr<agv_shell_session>&client) {
			if (client->post_shell_version(version) < 0) {
				loerror("agv_shell") << "failed to send the package of shell version,then close the session";
				client->close();
			}
		});
	}
}

void agv_shell_server::add_client_lnk(const uint32_t link){
	auto iter = std::find_if(vct_lnk_.begin(), vct_lnk_.end(), [&](const uint32_t lk)->bool{
		return link == lk ? true : false;
	});
	if (iter == vct_lnk_.end()){
		vct_lnk_.push_back(link);
	}
	return;
}

void agv_shell_server::reduce_client_lnk(const uint32_t link){
	auto iter = std::find_if(vct_lnk_.begin(), vct_lnk_.end(), [&](const uint32_t lk)->bool{
		return link == lk ? true : false;
	});
	if (iter == vct_lnk_.end()){
		loerror("agv_shell") << "can not find link:" << link << " in the client link collection.";
		return;
	}
	vct_lnk_.erase(iter);
	return;
}

void agv_shell_server::post_tar_backups(const uint32_t link, const std::string& des_file)
{
	std::shared_ptr<agv_shell_session> client_session_ = nullptr;
	if (service_ && (service_->search_client_by_link(link, client_session_) >= 0)){
		if (client_session_){
			client_session_->post_tar_backups(des_file);
		}
	}
}
