#include "long_lnk_receiver.h"
#include "file_manager.h"

long_lnk_receiver::long_lnk_receiver()
{
	file_search_root_ = nsp::os::get_module_directory<char>();
	init();
}

long_lnk_receiver::~long_lnk_receiver()
{
	uinit();
}

void long_lnk_receiver::init()
{
	try
	{
		if (!alive_th_)
		{
			alive_th_ = new std::thread(std::bind(&long_lnk_receiver::alive_check_thread, this));
		}
	}
	catch (...)
	{

	}
}

void long_lnk_receiver::uinit()
{
	is_exits_ = 1;
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

void long_lnk_receiver::alive_check_thread()
{
	const int ALIVE_TIME_INTERAL = 2000;
	const int ALIVE_COUNT = 3;
	while (time_wait_.wait(ALIVE_TIME_INTERAL))
	{
		if (is_exits_ == 1)break;

		{
			std::lock_guard<decltype(lock_server_obj_)> lock(lock_server_obj_);
			auto iter_begin = map_server_obj_.begin();
			while (iter_begin != map_server_obj_.end())
			{
				iter_begin->second->notify_all([&](const std::shared_ptr<long_lnk_session>& client){
					//发送心跳包
					client->post_alive_pkt();

					//查看当前心跳计数是否超出临界值
					if (client->get_alive_count() > ALIVE_COUNT){
						client->close();
					}
					else{
						client->add_alive_count();
					}
				});
				iter_begin++;
			}
		}
	}

}

int long_lnk_receiver::fts_create_receiver(const char *ipaddr_local, uint16_t port_local){

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

	std::shared_ptr<nsp::tcpip::tcp_application_service<long_lnk_session>> server_manager;
	try{
		server_manager = std::make_shared<nsp::tcpip::tcp_application_service<long_lnk_session>>();
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

int long_lnk_receiver::fts_change_configure(const struct fts_parameter *param){
	if (!param)return -EINVAL;
	timeout_transfer_ = param->timeout_cancel_transfer;
	maximum_iops_global_ = param->maximum_iops_global;
	if (strcmp(param->file_search_root, "") != 0)file_search_root_ = param->file_search_root;
	return 0;
}

std::shared_ptr<nsp::tcpip::tcp_application_service<long_lnk_session>> long_lnk_receiver::search_server_obj(const nsp::tcpip::endpoint& ep){
	std::lock_guard<decltype(lock_server_obj_)> lock(lock_server_obj_);
	auto iter = map_server_obj_.find(ep);
	if (iter == map_server_obj_.end()) return nullptr;
	else return iter->second;
}

void long_lnk_receiver::push_server_obj(const nsp::tcpip::endpoint&ep,
	const std::shared_ptr<nsp::tcpip::tcp_application_service<long_lnk_session>>& server){
	std::lock_guard<decltype(lock_server_obj_)> lock(lock_server_obj_);
	map_server_obj_.insert(std::make_pair(ep, server));
}

void long_lnk_receiver::fts_destory_receiver(const char *ipaddr_local, uint16_t port_local){
	std::lock_guard<decltype(lock_server_obj_)> lock(lock_server_obj_);
	nsp::tcpip::endpoint ep(ipaddr_local, port_local);
	auto iter = map_server_obj_.find(ep);
	if (iter != map_server_obj_.end()){
		//关闭该服务端下连接的所有客户端
		iter->second->notify_all([&](const std::shared_ptr<long_lnk_session>&client){
			client->close();
		});
		//关闭服务端
		iter->second->close();
		//移除服务端
		map_server_obj_.erase(iter);
	}
}

int long_lnk_receiver::fts_pull_get_file_info(const uint32_t session_link, const std::string& file_path,
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

int long_lnk_receiver::fts_pull_get_file_block_stream(const uint32_t pkt_id, const uint32_t link, 
	const uint64_t file_id, const uint64_t offset, const uint32_t read_size){
	return nsp::toolkit::singleton<file_manager>::instance()->read_file_block_long_lnk(pkt_id, link, file_id,
		nsp::file::current_identify::pull_identify, offset, read_size);
}

void long_lnk_receiver::fts_pull_read_file_error(const uint32_t link,const uint64_t file_id, const nsp::proto::file_status status, const int err_code){
	for (auto&iter : map_server_obj_){
		std::shared_ptr<long_lnk_session> client_session;
		if (iter.second->search_client_by_link(link, client_session) < 0)continue;
		client_session->post_pull_file_error(file_id, status, err_code);
	}
}

void long_lnk_receiver::fts_pull_send_file_block_stream(const uint32_t pkt_id, const uint64_t file_id, const uint32_t link, 
	const uint64_t offset, const std::string& stream){
	for (auto&iter : map_server_obj_)
	{
		std::shared_ptr<long_lnk_session>client_session;
		if (iter.second->search_client_by_link(link, client_session) < 0)continue;
		client_session->post_pull_file_block_response(pkt_id, file_id, offset, stream);
	}
}

bool long_lnk_receiver::close_fts_file_handler(const uint32_t link, const uint64_t file_id)
{
	return nsp::toolkit::singleton<file_manager>::instance()->close_fts_file_handler(link, file_id);
}