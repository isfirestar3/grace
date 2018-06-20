#include "receiver_manager.h"
#include "os_util.hpp"
#include "file_manager.h"
#include "receiver_write_manage.h"

receiver_manager::receiver_manager(){
	initlization();
}

receiver_manager::~receiver_manager(){

}

void receiver_manager::initlization(){
	file_search_root_ = nsp::os::get_module_directory<char>();

	if (timeout_thread_)return;

	try{
		timeout_thread_ = new std::thread(std::bind(&receiver_manager::timeout_check, this));
	}
	catch (...){
		loerror("fts") << "failed to create timeout check thread.";
	}
}

//超时检测
void receiver_manager::timeout_check(){
	static const int TEST_TIMEOUT_INTERVAL = 2000;
	while (test_timeout_.wait(TEST_TIMEOUT_INTERVAL) > 0){
		for (auto & iter : map_server_obj_){
			iter.second->notify_all([&](const std::shared_ptr<network_session>&client){
				auto interval = client->get_interval();
				if (interval > timeout_transfer_){
					lowarn("fts") << "receiver get timeout,the interval = " << interval;
					//发送超时信号包给对端
					if (client->post_transmit_timeout() < 0){
						lowarn("fts") << "can not send timeout package,so immediately close client session.";
						client->close();
					}
				}
			});
		}
	}
}

int receiver_manager::fts_create_receiver(const char *ipaddr_local, uint16_t port_local){

	//判断输入ip地址是否存在，如果指针为null，则默认该ip 为0.0.0.0，如果输入字符为空，则指定ip为0.0.0.0
	std::string ip_str;
	if (!ipaddr_local) {
		ip_str = "0.0.0.0";
	}
	else{
		ip_str = ipaddr_local;
		ip_str = ip_str.empty() ? "0.0.0.0" : ip_str;
	}
	nsp::tcpip::endpoint ep(ip_str.c_str(), port_local);

	{
		std::lock_guard<decltype(lock_server_obj_)> lock(lock_server_obj_);
		if (map_server_obj_.find(ep) != map_server_obj_.end()) return -EEXIST;
	}
	
	std::shared_ptr<nsp::tcpip::tcp_application_service<network_session>> server_manager;
	try{
		server_manager = std::make_shared<nsp::tcpip::tcp_application_service<network_session>>();
	}
	catch (std::bad_alloc&){
		loerror("fts") << "failed to create server object";
		return -ENOMEM;
	}
	int res;
	if ((res = server_manager->begin(ep)) < 0){
		loerror("fts") << "failed to start server listen,the server endpoint:" << ep.to_string();
		return -ENETDOWN;
	}

	//如果输入端口为0，则在建立服务之后，需要获取监听的随机端口
	if (port_local == 0){
		ep = server_manager->local();
	}
	loinfo("fts") << "start server listen successfully,the endpoint is " << ep.to_string();
	//保存创建的服务对象
	push_server_obj(ep, server_manager);
	return res;
}

std::shared_ptr<nsp::tcpip::tcp_application_service<network_session>> receiver_manager::search_server_obj(const nsp::tcpip::endpoint& ep){
	std::lock_guard<decltype(lock_server_obj_)> lock(lock_server_obj_);
	auto iter = map_server_obj_.find(ep);
	if (iter == map_server_obj_.end()) return nullptr;
	else return iter->second;
}

void receiver_manager::push_server_obj(const nsp::tcpip::endpoint&ep, 
	const std::shared_ptr<nsp::tcpip::tcp_application_service<network_session>>& server){
	std::lock_guard<decltype(lock_server_obj_)> lock(lock_server_obj_);
	map_server_obj_.insert(std::make_pair(ep, server));
}

void receiver_manager::fts_destory_receiver(const char *ipaddr_local, uint16_t port_local){
	std::lock_guard<decltype(lock_server_obj_)> lock(lock_server_obj_);
	nsp::tcpip::endpoint ep(ipaddr_local, port_local);
	auto iter = map_server_obj_.find(ep);
	if (iter != map_server_obj_.end()){
		//关闭该服务端下连接的所有客户端
		iter->second->notify_all([&](const std::shared_ptr<network_session>&client){
			client->close();
		});
		//关闭服务端
		iter->second->close();
		//移除服务端
		map_server_obj_.erase(iter);
	}
}

int receiver_manager::fts_change_configure(const struct fts_parameter *param){
	if (!param)return -EINVAL;
	timeout_transfer_ = param->timeout_cancel_transfer;
	maximum_iops_global_ = param->maximum_iops_global;
	if (strcmp(param->file_search_root, "") != 0)file_search_root_ = param->file_search_root;
	return 0;
}

int receiver_manager::fts_create_file_head(const uint32_t session_link, nsp::file::file_head &head, int & error_code){
	//判断路径是否为相对路径，如果为相对路径，则需要补充完整路径
	if (head.path_remote.size() > 2 && (head.path_remote.substr(0, 2) == "./")){
		head.path_remote = file_search_root_ + "/" + head.path_remote.substr(2);
	}
	else if (head.path_remote.size() > 3 && (head.path_remote.substr(0, 3) == "../")){
		size_t pos = file_search_root_.find_last_of('/');
		if (pos != std::string::npos){
			head.path_remote = file_search_root_.substr(0, pos) + "/" + head.path_remote.substr(3);
		}
	}
	
	return nsp::toolkit::singleton<file_manager>::instance()->create_files(session_link, head, error_code);
}

void receiver_manager::is_file_complete(const std::string& file_path, const uint64_t file_size){
	//判断路径是否为相对路径，如果为相对路径，则需要补充完整路径
	std::string file_tmp;
	if (file_path.size() > 2 && (file_path.substr(0, 2) == "./")){
		file_tmp = file_search_root_ + "/" + file_path.substr(2);
	}
	nsp::toolkit::singleton<file_manager>::instance()->is_file_complete(file_path, file_size);
}

int receiver_manager::fts_push_save_data(const uint32_t session_link, const uint64_t offset, const std::string& data){
	nsp::file::file_block f_block;
	f_block.offset = offset;
	f_block.stream.assign(data.c_str(), data.size());
	return nsp::toolkit::singleton<file_manager>::instance()->write_file_block_stream(session_link,nsp::file::current_identify::push_identify, f_block);
}

int receiver_manager::fts_pull_get_file_info(const uint32_t session_link, const std::string& file_path,
	nsp::file::file_info& f_head, int& err_code, const bool is_long_lnk){
	//判断当前是否为相对路径
	std::string string_path;
	if (file_path.size() > 2 && (file_path.substr(0, 2) == "./")){
		string_path = file_search_root_ + "/" + file_path.substr(2);
	}
	else{
		string_path = file_path;
	}
	if (!is_long_lnk)
	{
		return nsp::toolkit::singleton<file_manager>::instance()->read_files(session_link, string_path, f_head, err_code);
	}
	else
	{
		return nsp::toolkit::singleton<file_manager>::instance()->read_files_long_lnk(session_link, string_path, f_head, err_code);
	}
}

int receiver_manager::fts_pull_get_file_block_stream(const uint32_t pkt_id, const uint32_t link, const uint64_t offset, const uint32_t read_size){
	return nsp::toolkit::singleton<file_manager>::instance()->read_file_block_stream(pkt_id,link, nsp::file::current_identify::pull_identify, offset, read_size);
}

void receiver_manager::fts_pull_send_file_block_stream(const uint32_t pkt_id, const uint32_t link, const uint64_t offset, const std::string& stream){
	for (auto&iter : map_server_obj_)
	{
		std::shared_ptr<network_session>client_session;
		if (iter.second->search_client_by_link(link, client_session) < 0)continue;
		client_session->post_pull_file_block_response(pkt_id, offset, stream);
	}
}

int receiver_manager::get_fts_listdir(const std::string& input_catalog, std::string & dir){
	file_read_headler f_read;
	std::string query_path;
	if (input_catalog.empty() || input_catalog == "./")
	{
		query_path = file_search_root_;
	}
	else if (input_catalog.size() > 2 && input_catalog.substr(0,2) == "./")
	{
		query_path = file_search_root_ + input_catalog.substr(1);
	}
	else
	{
		query_path = input_catalog;
	}
	dir.append(file_search_root_);
	char dir_tmp[2] = { '\1', '\0' };
	dir.append(dir_tmp, 2);
	return f_read.get_fts_listdir(query_path, dir, query_path);
}

void receiver_manager::delete_link_session(const unsigned int link)
{
	auto iter = map_server_obj_.begin();
	for (; iter != map_server_obj_.end();iter++)
	{
		std::shared_ptr<network_session> client_session;
		if (iter->second->search_client_by_link(link, client_session) >= 0){
			client_session->close();
			return;
		}
	}
}

int receiver_manager::fts_push_next_block(const uint32_t link, const uint64_t offset){
	for (auto&iter : map_server_obj_){
		std::shared_ptr<network_session>client_session;
		if (iter.second->search_client_by_link(link, client_session) < 0)continue;
		client_session->post_push_next_block_request(offset);
	}
	return 0;
}

void receiver_manager::fts_push_write_file_error(const uint32_t link, const nsp::proto::file_status status,const int err_code){
	for (auto&iter : map_server_obj_){
		std::shared_ptr<network_session> client_session;
		if (iter.second->search_client_by_link(link, client_session) < 0)continue;
		client_session->post_push_write_file_status(status, err_code);
	}
}

void receiver_manager::fts_pull_read_file_error(const uint32_t link, const nsp::proto::file_status status, const int err_code){
	for (auto&iter : map_server_obj_){
		std::shared_ptr<network_session> client_session;
		if (iter.second->search_client_by_link(link, client_session) < 0)continue;
		client_session->post_pull_read_file_error(status, err_code);
	}
}

int receiver_manager::fts_delete_files(const std::string& file_path){
	//判断当前是否为相对路径
	std::string string_path;
	if (file_path.size() >= 2 && (file_path.substr(0, 2) == "./")){
		file_path.size() == 2 ? string_path = file_search_root_ + "/" : string_path = file_search_root_ + "/" + file_path.substr(2);
	}
	else{
		string_path = file_path;
	}
	int res = nsp::toolkit::singleton<file_manager>::instance()->delete_file(string_path);
	//如果删除的是根文件目录，则需要重新建立根文件目录
	if (file_path.size() == 2 && (file_path.substr(0, 2) == "./")){
		int error_code;
		if (nsp::toolkit::singleton<file_manager>::instance()->create_directory(string_path, error_code) < 0){
		//if (nsp::os::mkdir(string_path) < 0){
			loerror("fts") << "failed to create folder : " << string_path << " the error code is " << error_code;
		}
	}
	return res;
}

void receiver_manager::fts_overwrite_file(const uint32_t link, std::string& file_path){
	for (auto&iter : map_server_obj_){
		std::shared_ptr<network_session> client_session;
		if (iter.second->search_client_by_link(link, client_session) < 0)continue;
		else{
			//删除前一个链接内数据缓存
			nsp::toolkit::singleton<file_manager>::instance()->clear_rw_task_deque(link);
			nsp::toolkit::singleton<file_manager>::instance()->remove_rw_file_handler(link);
			//通知前一个链接，文件错误，并断链
			client_session->post_push_write_file_status(nsp::proto::file_status::write_error, 80);
			nsp::toolkit::singleton<receiver_write_manage>::instance()->add_write_file(link, file_path);
			break;
		}
	}
}

void receiver_manager::fts_get_current_catalog(const std::string& f_path, const uint32_t link, const uint32_t pkt_id)
{
	std::string abs_path;
	//判断路径是否为相对路径，如果为相对路径，则需要补充完整路径
	if (f_path.size() >= 2 && (f_path.substr(0, 2) == "./")){
		abs_path = file_search_root_ + "/" + f_path.substr(2);
	}
	else if (f_path.size() >= 3 && (f_path.substr(0, 3) == "../")){
		abs_path = handler_relative_path(file_search_root_, f_path);
	}
	else
	{
		abs_path = f_path;
	}
	query_catalog_task_.add_query_task(link, pkt_id, abs_path);
}

void receiver_manager::post_catalog_reponse(const uint32_t link, const uint32_t pkt_id, const int error, const std::vector<nsp::file::file_info>& vct_info)
{
	for (auto&iter : map_server_obj_){
		std::shared_ptr<network_session> client_session;
		if (iter.second->search_client_by_link(link, client_session) < 0)continue;
		client_session->post_catalog_reponse(pkt_id, error, vct_info);
	}
}

std::string receiver_manager::handler_relative_path(const std::string& root_path, const std::string& f_path)
{
	std::string file_path = root_path;
	std::string input_path = f_path;
	size_t pos = input_path.find("../");
	while (pos != std::string::npos)
	{
		size_t pos_src = file_path.find_last_of('/');
		if (pos_src != std::string::npos)
		{
			file_path = file_path.substr(0, pos_src);
		}
		input_path = input_path.substr(pos + 2);
		pos = input_path.find("../");
	}
	return file_path + input_path;
}

void receiver_manager::reset_file_time(const std::string& file_name, const uint64_t& create_time, const uint64_t& modify_time, const uint64_t& access_time)
{
	std::string file_path;
	if (file_name.size() > 2 && (file_name.substr(0, 2) == "./"))
	{
		file_path = file_search_root_ + "/" + file_name.substr(2);
	}
	else
	{
		file_path = file_name;
	}
	nsp::toolkit::singleton<file_manager>::instance()->reset_file_time(file_path, create_time, modify_time, access_time);
}