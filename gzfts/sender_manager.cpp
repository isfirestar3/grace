#include "sender_manager.h"
#include "log.h"

sender_manager::sender_manager(){

}

sender_manager::~sender_manager(){

}

int sender_manager::fts_listdir(const char *ipaddr_to, uint16_t port_to, const char*input_catalog, char **dir, int *dircbs){
	//参数错误
	if (!ipaddr_to)return -EINVAL;
	nsp::tcpip::endpoint ep(ipaddr_to, port_to);
	std::shared_ptr<network_session> send_session = create_session(ep);
	if (!send_session){
		return -ENETDOWN;
	}
	asio_data data_asio;
	nsp::os::waitable_handle water(0);
	nsp::file::list_dir_info list_dir;
	//发送获取路径请求，同步等待数据返回
	if (send_session->post_get_listdir(input_catalog,std::make_shared<asio_block>([&](const void* data){
		if (!data){
			water.sig();
			return;
		}
		data_asio = *(asio_data*)data;
		if (data_asio.get_err() != nsp::proto::kSuccessful){
			loerror("fts") << "failed to get asio data while post get list dir package.";
			water.sig();
			return;
		}
		list_dir = *(nsp::file::list_dir_info*)data;
		water.sig();
	})) < 0){
		send_session->close();
		return -1;
	}
	water.wait();
	water.reset();
	if (send_session){
		send_session->close();
	}
	
	if (data_asio.get_err() != nsp::proto::kSuccessful){
		return -1;
	}
	char* list_cr = new char[list_dir.list_data_.size()];
#ifdef _WIN32
	memcpy_s(list_cr, list_dir.list_data_.size(), list_dir.list_data_.c_str(), list_dir.list_data_.size());
#else
    memcpy(list_cr, list_dir.list_data_.c_str(), list_dir.list_data_.size());
#endif
    
	*dir = list_cr;
	*dircbs = list_dir.list_data_.size();
	return 0;
}

void sender_manager::fts_freedir(char *dir){
	if (dir){
		delete[] dir;
	}
}

std::shared_ptr<network_session> sender_manager::create_session(const nsp::tcpip::endpoint& ep){
	std::shared_ptr<network_session> session_ptr;
	try{
		session_ptr = std::make_shared<network_session>();
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
			throw -1;
		}
	}
	catch (...){
		loerror("fts") << "failed to make session share ptr,the endpoint is " << ep.to_string();
		return nullptr;
	}
	return session_ptr;
}

std::shared_ptr<network_session> sender_manager::search_session(const uint32_t link){
	std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
	auto iter = map_link_session_.find(link);
	if (iter == map_link_session_.end()){
		return nullptr;
	}
	return iter->second;
}

void sender_manager::insert_session(const uint32_t link, const std::shared_ptr<network_session>& session){
	std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
	map_link_session_.insert(std::make_pair(link, session));
}

int sender_manager::delete_map_link_session(const uint32_t link)
{
	std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
	auto iter = map_link_session_.find(link);
	if (iter == map_link_session_.end()){
		return -1;
	}
	map_link_session_.erase(iter);
	return 0;
}

int sender_manager::fts_pull_file(const char *ipaddr_from, uint16_t port_from, const char *path_local, const char *path_remote, int overwrite)
{
	nsp::tcpip::endpoint ep(ipaddr_from, port_from);
	std::shared_ptr<network_session>send_session = create_session(ep);
	if (!send_session){
		return -ENETDOWN;
	}
	insert_session(send_session->get_link(), send_session);

	file_data f_data;
	f_data.ipaddr = ipaddr_from;
	f_data.port = port_from;
	f_data.path_local = path_local;
	f_data.path_remote = path_remote;

	send_session->set_remote_local_ep(f_data, block_size_pre_transfer_);
	return send_session->post_pull_requst_file(path_remote, path_local, overwrite);
}

int sender_manager::fts_pull_next_block(const uint32_t link, const uint64_t offset){
	std::shared_ptr<network_session> send_session = search_session(link);
	if (send_session == nullptr){
		loerror("fts") << "can not find pull send_session by link while pull send next data block stream.";
		//回调上层通知发生错误
		if (callback_fun_){
			callback_fun_(NULL, 0, NULL, NULL, nsp::proto::file_status::write_error, -1, 0, nullptr, 0);
		}
		return -1;
	}
	
	uint32_t status = -1;
	file_data data;
	send_session->get_file_total_size_and_status(data);
	data.status == -1 ? data.step = 0 : data.step = 100 * ((double)offset / data.total_size);
	status = data.status;
	if (callback_fun_){
		callback_fun_(data.ipaddr.c_str(), data.port, data.path_local.c_str(), data.path_remote.c_str(), status, 0, data.step, nullptr, 0);
	}
	if (status == 0){
		send_session->post_pull_next_block_request(offset);
	}
	else if (status == 1){
		send_session->post_pull_complete();
	}
	return 0;
}

void sender_manager::fts_file_complete(file_data& data){
	if (callback_fun_){
		callback_fun_(data.ipaddr.c_str(), data.port, data.path_local.c_str(), data.path_remote.c_str(), data.status, data.error, data.step, nullptr, 0);
	}
}

int sender_manager::fts_push(const char *ipaddr_to, uint16_t port_to, const char *path_local, const char *path_remote, int overwrite){
	nsp::tcpip::endpoint ep(ipaddr_to, port_to);
	std::shared_ptr<network_session>send_session = create_session(ep);
	if (!send_session){
		return -ENETDOWN;
	}
	nsp::file::file_info f_info;
	int error_code;
	if (nsp::toolkit::singleton<file_manager>::instance()->read_files(send_session->get_link(), path_local, f_info, error_code) < 0){
		loerror("fts") << "[push file head info request]failed to get local file head info,the error code is " << error_code;
		//回调上去，打开文件头失败
		if (callback_fun_){
			callback_fun_(ipaddr_to, port_to, path_local, path_remote, nsp::proto::file_status::read_error, error_code, 0, nullptr, 0);
		}
		send_session->close();
		return -1;
	}
	//保存session对象
	insert_session(send_session->get_link(), send_session);

	//计算读取的文件可以切片成多少块
	f_info.file_block_num_ = f_info.file_size_ / block_size_pre_transfer_;
	f_info.file_block_num_ = (f_info.file_size_ % block_size_pre_transfer_ == 0) ? f_info.file_block_num_ : (f_info.file_block_num_ + 1);

	file_data f_data;
	f_data.ipaddr = ipaddr_to;
	f_data.port = port_to;
	f_data.path_local = path_local;
	f_data.path_remote = path_remote;
	f_data.total_size = f_info.file_size_;

	send_session->set_remote_local_ep(f_data, block_size_pre_transfer_);
	return send_session->push_file_to_remote(path_remote, f_info, overwrite);
}

int sender_manager::fts_push_send_file_block_stream(const uint32_t link, const uint64_t offset, const std::string& data){
	std::shared_ptr<network_session> send_session = search_session(link);
	if (send_session == nullptr){
		loerror("fts") << "can not find push send_session by link while push send next data block stream.";
		return -1;
	}
	return send_session->push_file_block_stream(offset, data);
}

int sender_manager::fts_get_file_block(const uint32_t link, const uint64_t offset, const uint32_t read_size){
	std::shared_ptr<network_session> send_session = search_session(link);
	if (send_session == nullptr){
		loerror("fts") << "can not find push send_session by link while request read next data block stream.";
		return -1;
	}
	if (callback_fun_){
		file_data data;

		send_session->get_file_total_size_and_status(data);
		data.status == -1 ? data.step = 0 : data.step = 100 * ((double)offset / data.total_size);
		callback_fun_(data.ipaddr.c_str(), data.port, data.path_local.c_str(), data.path_remote.c_str(), data.status, 0, data.step, nullptr, 0);
	}
	return nsp::toolkit::singleton<file_manager>::instance()->read_file_block_stream(0, link, nsp::file::current_identify::push_identify, offset, read_size);
}

int sender_manager::fts_change_configure(const struct fts_parameter *param){
	if (!param)return -EINVAL;
	callback_fun_ = param->fts_callback;
	if (param->block_size_pre_transfer != 0)block_size_pre_transfer_ = param->block_size_pre_transfer;
	return 0;
}

int sender_manager::creat_file(const uint32_t link, nsp::file::file_head& filehead, int& error_code)
{
	return nsp::toolkit::singleton<file_manager>::instance()->create_files(link, filehead, error_code);
}

int sender_manager::write_file(const uint32_t link, const nsp::file::file_block& f_blcok)
{
	return nsp::toolkit::singleton<file_manager>::instance()->file_manager::write_file_block_stream(link, nsp::file::current_identify::pull_identify, f_blcok);
}

int sender_manager::fts_delete_file(const char *ipaddr_to, uint16_t port_to, const char *path_remote)
{
	nsp::tcpip::endpoint ep(ipaddr_to, port_to);
	std::shared_ptr<network_session>send_session = create_session(ep);
	if (!send_session){
		return -ENETDOWN;
	}

	asio_data data_asio;
	nsp::os::waitable_handle water(0);
	if (send_session->post_delete_file_requst(path_remote, std::make_shared<asio_block>([&](const void* data){
		if (!data){
			water.sig();
			return;
		}
		data_asio = *(asio_data*)data;
		water.sig();
		return;
	})) < 0){
		send_session->close();
		return -1;
	}
	water.wait();
	water.reset();
	if (send_session){
		send_session->close();
	}

	if (data_asio.get_err() != nsp::proto::kSuccessful){
		loerror("fts") << "failed to delete remote files.";
		return -1;
	}
	return 0;
}

int sender_manager::fts_pull_write_file_error(const uint32_t link, const nsp::proto::file_status status, const int error_code){
	//回调上层通知发生错误
	if (callback_fun_){
		callback_fun_(NULL, 0, NULL, NULL, status, error_code, 0, nullptr, 0);
	}
	//根据link找到session ，关闭session 链接
	std::shared_ptr<network_session> session = search_session(link);
	if (session == nullptr){
		loerror("fts") << "can not find session while write file error at pull operation.";
		return -1;
	}
	////回调上层通知发生错误
	//file_data data;
	//session->get_file_total_size_and_status(data);
	//data.status = status;
	//data.error = error_code;
	//if (callback_fun_){
	//	callback_fun_(data.ipaddr.c_str(), data.port, data.path_local.c_str(), data.path_remote.c_str(), data.status, data.error, data.step, nullptr, 0);
	//}
	//关闭链接
	session->close();
	////删除session对象
	//delete_map_link_session(link);
	return 0;
}

int sender_manager::fts_push_read_file_error(const uint32_t link, const nsp::proto::file_status status, const int error_code){

	//根据link找到session ，关闭session 链接
	std::shared_ptr<network_session> session = search_session(link);
	if (session == nullptr){
		loerror("fts") << "can not find session while read file error at push operation.";
		return -1;
	}
	//回调上层通知发生错误
	file_data data;
	session->get_file_total_size_and_status(data);
	if (callback_fun_){
		callback_fun_(data.ipaddr.c_str(), data.port, data.path_local.c_str(), data.path_remote.c_str(), status, error_code, data.step, nullptr, 0);
	}

	//关闭链接,receive端在收到断链之后会比较目标文件与源大小是否一致，故此处不需要发送完成包通知
	session->close();
	//删除session对象
	delete_map_link_session(link);
	return 0;
}

void sender_manager::fts_cancel(const char* ipaddr, const uint16_t port){

	std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
	for (auto& iter : map_link_session_){
		if (!ipaddr){
			iter.second->close();
			continue;
		}
		if ((strcmp(iter.second->remote().ipv4(), ipaddr) == 0) && (iter.second->remote().port() == port)){
			iter.second->close();
			return;
		}
	}
}

int sender_manager::fts_catalog_info_req(const char *ipaddr_to, uint16_t port_to, const char *path_remote, char **output_info, int *output_cb)
{
	//参数错误
	if (!ipaddr_to)return -EINVAL;
	nsp::tcpip::endpoint ep(ipaddr_to, port_to);
	std::shared_ptr<network_session> send_session = create_session(ep);
	if (!send_session){
		return -ENETDOWN;
	}
	asio_data data_asio;
	nsp::os::waitable_handle water(0);
	nsp::file::list_dir_info list_dir;

	//发送获取路径请求，同步等待数据返回
	if (send_session->post_catalog_request(path_remote, std::make_shared<asio_block>([&](const void* data){
		if (!data){
			water.sig();
			return;
		}
		data_asio = *(asio_data*)data;
		if (data_asio.get_err() != nsp::proto::kSuccessful){
			loerror("fts") << "failed to get asio data while post get current catalog package.";
			water.sig();
			return;
		}
		list_dir = *(nsp::file::list_dir_info*)data;
		water.sig();
	})) < 0){
		send_session->close();
		return -1;
	}
	water.wait();
	water.reset();
	if (send_session){
		send_session->close();
	}

	if (data_asio.get_err() != nsp::proto::kSuccessful){
		return -1;
	}

	*output_info = new char[list_dir.list_data_.size()];
	memcpy(*output_info, list_dir.list_data_.c_str(), list_dir.list_data_.size());
	*output_cb = list_dir.list_data_.size();
	return 0;
}