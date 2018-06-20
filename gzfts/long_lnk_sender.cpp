#include "long_lnk_sender.h"
#include "file_manager.h"

long_lnk_sender::long_lnk_sender()
{
	init();
}

long_lnk_sender::~long_lnk_sender()
{
	uinit();
}

void long_lnk_sender::init()
{
	try
	{
		if (!alive_th_)
		{
			alive_th_ = new std::thread(std::bind(&long_lnk_sender::alive_check_thread, this));
		}
	}
	catch (...)
	{

	}
}

void long_lnk_sender::uinit()
{
	is_exits_ = true;
	time_wait_.sig();
	if (alive_th_)
	{
		if (alive_th_->joinable())
		{
			alive_th_->join();
		}
		delete alive_th_;
		alive_th_ = nullptr;
	}
	time_wait_.reset();
}

void long_lnk_sender::alive_check_thread()
{
	const int ALIVE_TIME_INTERAL = 2000;
	const int ALIVE_COUNT = 3;
	while (time_wait_.wait(ALIVE_TIME_INTERAL))
	{
		if (is_exits_)break;

		{
			std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
			auto iter_begin = map_link_session_.begin();
			while (iter_begin != map_link_session_.end())
			{
				if (iter_begin->second->get_alive_count() > ALIVE_COUNT)
				{
					iter_begin->second->close();
					break;
				}
				else
				{
					iter_begin->second->add_alive_count();
				}
				iter_begin++;
			}
		}
	}
}

void long_lnk_sender::add_file_info(const uint64_t file_id, const file_callabck_data& f_data)
{
	std::lock_guard<decltype(mutex_)> lock(mutex_);
	auto iter = map_fd_.find(file_id);
	if (iter == map_fd_.end())
	{
		map_fd_.insert(std::make_pair(file_id, f_data));
	}
}

std::shared_ptr<long_lnk_session> long_lnk_sender::create_session(const nsp::tcpip::endpoint& ep){
	std::shared_ptr<long_lnk_session> session_ptr;
	try{
		session_ptr = std::make_shared<long_lnk_session>();
		if (!session_ptr) {
			loerror("fts") << "can not make session share ptr.";
			throw - 1;
		}
		if (session_ptr->create() < 0){
			loerror("fts") << "the session can not create.";
			throw - 1;
		}
		if (session_ptr->connect(ep) < 0){
			loerror("fts") << "failed to connect to server session.";
			session_ptr->close();
			throw - 1;
		}
	}
	catch (...){
		loerror("fts") << "failed to make session share ptr,the endpoint is " << ep.to_string();
		return nullptr;
	}
	return session_ptr;
}

void long_lnk_sender::insert_session(const uint32_t link, const std::shared_ptr<long_lnk_session>& session){
	std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
	map_link_session_.insert(std::make_pair(link, session));
}

std::shared_ptr<long_lnk_session> long_lnk_sender::search_session(const uint32_t link){
	std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
	auto iter = map_link_session_.find(link);
	if (iter == map_link_session_.end()){
		return nullptr;
	}
	return iter->second;
}

void long_lnk_sender::fts_cancel(const char* ipaddr, const uint16_t port){

	std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
	for (auto iter = map_link_session_.begin(); iter != map_link_session_.end(); iter++){
		if (!ipaddr){
			iter->second->close();
			continue;
		}
		if ((strcmp(iter->second->remote().ipv4(), ipaddr) == 0) && (iter->second->remote().port() == port)){
			iter->second->close();
			//map_link_session_.erase(iter);
			return;
		}
	}
}

int long_lnk_sender::delete_map_link_session(const uint32_t link)
{
	std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
	auto iter = map_link_session_.find(link);
	if (iter == map_link_session_.end()){
		return -1;
	}
	map_link_session_.erase(iter);
	return 0;
}

int long_lnk_sender::fts_change_configure(const struct fts_parameter *param){
	if (!param)return -EINVAL;
	callback_fun_ = param->fts_callback;
	if (param->block_size_pre_transfer != 0)block_size_pre_transfer_ = param->block_size_pre_transfer;
	return 0;
}

int long_lnk_sender::fts_pull_long(const char *ipaddr_from, uint16_t port_from, const char *path_local, const char *path_remote, int overwrite)
{
	//查找目标端口的链接是否已经存在
	std::shared_ptr<long_lnk_session> send_session = nullptr;
	{
		std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
		for (const auto& iter : map_link_session_)
		{
			if ((strcmp(iter.second->get_remote_endpoint().ipv4(), ipaddr_from) == 0)
				&& port_from == iter.second->get_remote_endpoint().port())
			{
				send_session = iter.second;
				break;
			}
		}
	}

	if (!send_session)
	{
		nsp::tcpip::endpoint ep(ipaddr_from, port_from);
		send_session = create_session(ep);
		if (!send_session){
			return -ENETDOWN;
		}
		send_session->set_remote_endpoin(ep);
		insert_session(send_session->get_link(), send_session);
	}

	uint64_t pull_id = allocate_pull_identify();
	file_callabck_data file_d;
	file_d.path_local = path_local;
	file_d.path_remote = path_remote;
	file_d.ipaddr = ipaddr_from;
	file_d.port = port_from;
	add_file_info(pull_id, file_d);
	send_session->set_transfer_block_size(block_size_pre_transfer_);
	return send_session->post_pull_file_long(path_remote, path_local, overwrite, pull_id);
}

std::string long_lnk_sender::get_local_file_path(const uint64_t pull_id)
{
	std::lock_guard<decltype(mutex_)> lock(mutex_);
	auto iter = map_fd_.find(pull_id);
	if (iter == map_fd_.end())
	{
		return "";
	}
	return iter->second.path_local;
}

void long_lnk_sender::set_pull_file_size(const uint64_t pull_id, const uint64_t size)
{
	std::lock_guard<decltype(mutex_)> lock(mutex_);
	auto iter = map_fd_.find(pull_id);
	if (iter == map_fd_.end())
	{
		return;
	}
	iter->second.total_size = size;
}

void long_lnk_sender::fts_long_lnk_callback(const uint64_t pull_id, const int status, const int step, const int error_code)
{
	std::lock_guard<decltype(mutex_)> lock(mutex_);
	auto iter = map_fd_.find(pull_id);
	if (iter == map_fd_.end())
	{
		loerror("fts") << "can not find callback file id.";
		return;
	}
	if (status < 0)
	{
		loerror("fts") << "get an error while pull remote file:" << iter->second.path_remote << " ,the error code is:" << error_code;
	}
	if (callback_fun_)
	{
		callback_fun_(iter->second.ipaddr.c_str(), iter->second.port, iter->second.path_local.c_str(), iter->second.path_remote.c_str(),
			status, error_code, step, nullptr, 0);
	}
}

int long_lnk_sender::creat_file(const uint32_t link, nsp::file::file_head& filehead, int& error_code, const uint64_t file_id)
{
	return nsp::toolkit::singleton<file_manager>::instance()->create_files(link, filehead, error_code, true, file_id);
}

int long_lnk_sender::write_file(const uint32_t link, const nsp::file::file_block& f_blcok)
{
	return nsp::toolkit::singleton<file_manager>::instance()->file_manager::write_file_block_stream(link, nsp::file::current_identify::pull_identify,
		f_blcok, true);
}

int long_lnk_sender::fts_pull_next_block(const uint32_t link, const uint32_t fd, const uint64_t offset){
	std::lock_guard<decltype(mutex_)> lock(mutex_);
	auto iter = map_fd_.find(fd);
	if (iter == map_fd_.end())
	{
		loerror("fts") << "can not find file id.";
		//回调上层通知发生错误
		if (callback_fun_){
			callback_fun_(NULL, 0, NULL, NULL, nsp::proto::file_status::write_error, -1, 0, nullptr, 0);
		}
	}
	file_callabck_data data = iter->second;
	std::shared_ptr<long_lnk_session> send_session = search_session(link);
	if (send_session == nullptr){
		loerror("fts") << "can not find pull send_session by link while pull send next data block stream.";
		//回调上层通知发生错误
		if (callback_fun_){
			callback_fun_(data.ipaddr.c_str(), data.port, data.path_local.c_str(), data.path_remote.c_str(), 
				nsp::proto::file_status::write_error, -1, 0, nullptr, 0);
		}
		return -1;
	}
	send_session->get_file_status(fd, data);
	uint32_t status = -1;
	data.status == -1 ? data.step = 0 : data.step = 100 * ((double)offset / data.total_size);
	status = data.status;
	if (callback_fun_){
		callback_fun_(data.ipaddr.c_str(), data.port, data.path_local.c_str(),
			data.path_remote.c_str(), status, 0, data.step, nullptr, 0);
	}
	if (status == 0){
		send_session->post_pull_next_block_request(fd, offset);
	}
	else if (status == 1){
		send_session->post_pull_complete(fd);
		loinfo("fts") << "success pull file:" << data.path_local;
	}
	return 0;
}

int long_lnk_sender::fts_pull_write_file_error(const uint32_t link,const uint64_t fd, const nsp::proto::file_status status, const int error_code)
{
	std::lock_guard<decltype(mutex_)> lock(mutex_);
	auto iter = map_fd_.find(fd);
	if (iter != map_fd_.end())
	{
		file_callabck_data f_data = iter->second;
		//回调上层通知发生错误
		if (callback_fun_){
			callback_fun_(f_data.ipaddr.c_str(), f_data.port, f_data.path_local.c_str(), f_data.path_remote.c_str(), status, error_code, 0, nullptr, 0);
		}
		loerror("fts") << "failed to write file while pull file"<<f_data.path_local <<" ,the error code is:" << error_code;
	}
	
	//根据link找到session ，关闭session 链接
	std::shared_ptr<long_lnk_session> session = search_session(link);
	if (session == nullptr){
		loerror("fts") << "can not find session while write file error at pull operation.";
		return -1;
	}
	//通知receiver端可以关闭句柄
	return session->post_pull_file_error(fd, nsp::proto::file_status::write_error, error_code);
}

void long_lnk_sender::close_fts_file_handler(const uint32_t lnk, const uint64_t file_id)
{
	nsp::toolkit::singleton<file_manager>::instance()->close_fts_file_handler(lnk, file_id);
}

void long_lnk_sender::remove_file_id(const uint32_t file_id)
{
	std::lock_guard<decltype(mutex_)> lock(mutex_);
	auto iter = map_fd_.find(file_id);
	if (iter == map_fd_.end())
	{
		return;
	}
	map_fd_.erase(iter);
}