#include "connect_manage.h"
#include "toolkit.h"
#include <errno.h>
#include <algorithm>

nsp::os::waitable_handle query_wait_;

int __stdcall fts_callback(const char *ipaddr, uint16_t port,
	const char *path_local, const char *path_remote,
	int status, int error, int step,
	void *user_data, int user_data_size)
{
	if (step == 100 || status<0){
		query_wait_.sig();
	}
	//if (fts_callback_){
	//	fts_callback_(step);
	//}
	return 0;
}

connect_manage::connect_manage()
{
	fts_parameter para;
	para.fts_callback = fts_callback;
	fts_change_configure(&para);
	init();
}


connect_manage::~connect_manage()
{
	uinit();
}

//std::function<void(const int)>fts_callback_;//fts进度回包
nsp::os::waitable_handle connected_(1);

void connect_manage::init()
{
	try
	{
		if (!alive_th_)
		{
			alive_th_ = new std::thread(std::bind(&connect_manage::alive_agvs_check_thread, this));
		}
	}
	catch (...)
	{

	}
}

void connect_manage::uinit()
{
	is_exits = true;
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
//单车连接
int connect_manage::init_network(std::string&ip, std::string&port)
{
	if (con_session_){
		con_session_->close();
		con_session_ = nullptr;
	}
	if (!agv_ip_.empty() && agv_port_ != 0)
	{
		fts_cancel(agv_ip_.c_str(), agv_port_, true);
	}
	agv_ip_ = ip;
	//is_login_ = true;
	con_session_ = std::make_shared<connect_session>();
	std::string ep = ip + ":" + port;
	if (nsp::tcpip::endpoint::build(ep, target_ep_) < 0){
		return -1;
	}
	if (!con_session_){
		return -1;
	}
	con_session_->create();
	if (con_session_->connect2(target_ep_) < 0){
		return -1;
	}
	if (connected_.wait(5000) > 0)
	{
		return -1;

	}
	return 0;
}
//连接多车
int connect_manage::init_networks(std::map<std::string, id_ep>&select_ep_)
{
	//is_login_ = true;
	back_count_ = 0;
	map_link_session_.clear();
	unconnect_agvs_.clear();
	//connected_ep_.clear();
	map_ip_session_.clear();
	map_ip_se_connected_.clear();
	for (auto&iter : select_ep_){
		if (agv_port_ != 0){
			fts_cancel(iter.first.c_str(), agv_port_, true);
		}
		nsp::tcpip::endpoint ep(iter.first.c_str(), iter.second.port_);
		id_session id_se;
		id_se.car_id_ = iter.second.car_id_;
		std::shared_ptr<connect_session>agcv_session = create_session(ep);
		if (agcv_session){
			//connected_ep_.insert(std::make_pair(agcv_session->get_link(),iter.first));//根据link可找到Ip
			id_se.session_ = agcv_session;
			map_ip_session_.insert(std::make_pair(iter.first, id_se));
			//map_session_ip_.insert(std::make_pair(agcv_session,iter.first));
			//insert_session(agcv_session->get_link(), iter.first, agcv_session);
			//return -ENETDOWN;
		}
		/*else{
			return -1;
		}*/
		/*else{
			unconnect_agvs_.insert(std::make_pair(iter.first,iter.second));
		}*/

	}
	//if (unconnect_agvs_.size() < 0) return -1;
	//for (auto &iter : unconnect_agvs_){
	//	auto eiter = select_ep_.find(iter.first);
	//	if (eiter != select_ep_.end()){
	//		select_ep_.erase(eiter);//把选择连接的确没连接的车辆从map中删除
	//	}
	//}
	//if (select_ep_.size() == 0){
	//	return -1;
	//}
	return 0;
}
void connect_manage::insert_session(const uint32_t link,const std::string&agv_ip, const std::shared_ptr<connect_session>& session){
	std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
	ip_session tmp;
	tmp.agv_ip_ = agv_ip;
	tmp.session_ = session;
	map_link_session_.insert(std::make_pair(link, tmp));
}
std::shared_ptr<connect_session> connect_manage::create_session(const nsp::tcpip::endpoint& ep)
{
	std::shared_ptr<connect_session> session_ptr;
	try{
		session_ptr = std::make_shared<connect_session>();
		if (!session_ptr) {
			loerror("black_box") << "can not make session share ptr.";
			throw - 1;
		}
		if (session_ptr->create() < 0){
			loerror("black_box") << "the session can not create.";
			throw - 1;
		}
		if (session_ptr->connect2(ep) < 0){
			return nullptr;
		}
		
		//if (session_ptr->connect(ep) < 0){
		//	loerror("black_box") << "failed to connect to server session.";
		//	session_ptr->close();
		//	throw - 1;
		//}
	}
	catch (...){
		loerror("black_box") << "failed to make session share ptr,the endpoint is " << ep.to_string();
		return nullptr;
	}
	return session_ptr;
}

int connect_manage::delete_map_link_session(const uint32_t link)
{	
	std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
	auto iter = map_link_session_.find(link);
	if (iter == map_link_session_.end()){
		return -1;
	}
	map_link_session_.erase(iter);
	return 0;
}
void connect_manage::on_connect(std::string&ip,const uint32_t link, bool state)
{
	std::lock_guard<decltype(map_ip_mutex_)> lock(map_ip_mutex_);
	if (state){
		//connected_.sig();
		auto iter = map_ip_session_.find(ip);
		if (iter != map_ip_session_.end()){
			map_ip_se_connected_.insert(std::make_pair(iter->first,iter->second));
			ip_session tmp;
			tmp.agv_ip_ = ip;
			tmp.session_ = iter->second.session_;
			insert_session(link, ip, iter->second.session_);
			if (disconnect_callback_){
				disconnect_callback_(0,ip);
			}
			
		}
		loinfo("black_box")  << "connect to "<< ip.c_str() <<" successful.";
	}
	else{
		//if (is_login_){//没登陆上，不是断连
		//	return; 
		//}
		auto iter = map_ip_session_.find(ip);
		if (iter != map_ip_session_.end()){
			if (disconnect_callback_){
				disconnect_callback_(-1,ip);
			}
			map_ip_session_.erase(iter);
			auto con_iter = map_ip_se_connected_.find(ip);
			if (con_iter != map_ip_se_connected_.end())
			{
				map_ip_se_connected_.erase(con_iter);
			}
		}
		loerror("black_box") << ip.c_str() << "disconnect.";
	}
	//std::vector<std::pair<std::string, id_session>>tVector;
	//for (auto&iter : map_ip_se_connected_){
	//	tVector.push_back(iter);
	//}     
	//if (tVector.size() > 1){
	//	std::sort(tVector.begin(), tVector.end(), [&](
	//		const std::pair<std::string, id_session>&first, const std::pair<std::string, id_session>&second)->bool{
	//		return first.second.car_id_ < second.second.car_id_; });
	//	map_ip_se_connected_.clear();
	//	for (auto&iter : tVector){
	//		map_ip_se_connected_.insert(std::make_pair(iter.first, iter.second));
	//	}
	//}

}

int connect_manage::query_log_types(std::map<std::string, int>&use_ep)
{
	//多车
	for (auto&iter : map_link_session_){
		if (iter.first){
			auto use_iter = use_ep.find(iter.second.agv_ip_);
			if (use_iter != use_ep.end())
			{
				int ret = iter.second.session_->post_log_type_request();
			}
			
		}
	}
	return 0;
	//单车
	//if (con_session_){
	//	if (con_session_->post_log_type_request() < 0){
	//		return -1;
	//	}
	//	else return 0;
	//}
	//else return -1;
}

int connect_manage::query_log_info(std::vector<std::string>&ip_vct,uint16_t fts_port, std::string&start_time, std::string&end_time, std::vector<std::string>&log_types, std::string& local_path)
{
	map_ip_logs_.clear();
	back_count_ = 0;
	total_count_ = 0;
	agv_port_ = fts_port;
	local_path_ = local_path;
	agv_count_ = ip_vct.size();
	int count = 0;
	for (auto&iter : ip_vct){
		auto link_iter = map_ip_se_connected_.find(iter);
		if (link_iter != map_ip_se_connected_.end()){
			if (link_iter->second.session_){
				link_iter->second.session_->post_log_info_request(start_time, end_time, log_types);
				count++;
			}
		}
	}
	agv_count_ = count;
	return 0;
	//单车
	//if (con_session_){
	//	if (con_session_->post_log_info_request(start_time, end_time, log_types) < 0){
	//		return -1;
	//	}
	//	else return 0;
	//}
	//else return -1;

}
//多车断连
int connect_manage::disconnect_network(const uint32_t link)
{
	//is_login_ = false;
	auto iter = map_link_session_.find(link);
	if (iter == map_link_session_.end()){
		return -1;
	}
	iter->second.session_->close();
	return 0;
}
int connect_manage::disconnect_network(const std::map<std::string, id_ep>&map_agv)
{
	//is_login_ = false;
	for (auto&iter : map_agv){
		auto session_iter = map_ip_se_connected_.find(iter.first);
		if (session_iter != map_ip_se_connected_.end() && session_iter->second.session_){
			session_iter->second.session_->close();
		}
		continue;
	}
	map_ip_session_.clear();
	map_ip_se_connected_.clear();
	map_link_session_.clear();
	return 0;
}

//单车断连
int connect_manage::disconnect_network()
{
	//is_login_ = false;
	if (con_session_){
		con_session_->close();
		return 0;
	}
	else return -1;
}
void connect_manage::disconnect_regiset_callback(const std::function<void(const int,const std::string&)>&func)
{
	if (func){
		disconnect_callback_=func;
	}
}

void connect_manage::recv_log_type_vct(std::vector<std::string>&logtype_vct)
{
	if (function_callback_){
		function_callback_(logtype_vct);
	}
}

void connect_manage::regiset_callback(const std::function<void(const std::vector<std::string>&log_vct)>&func)
{
	if (func){
		function_callback_ = func;
	}
}

void connect_manage::query_logs_file(int link, const std::vector<std::string>&logs_path)
{
	std::string agv_ip;
	auto iter_ip = map_link_session_.find(link);
	if (iter_ip == map_link_session_.end()){
		return;
	}
	back_count_++;
	agv_ip = iter_ip->second.agv_ip_;
	is_cancel_ = false;
	total_count_ += logs_path.size();
	std::thread th_get_logs(&connect_manage::start_to_get_logs, this, agv_ip, logs_path);
	th_get_logs.join();

}

void connect_manage::start_to_get_logs(std::string&agv_ip,std::vector<std::string>&log_vct)
{
	std::lock_guard<decltype(map_logs_mutex_)>lock (map_logs_mutex_);
	map_ip_logs_.insert(std::make_pair(agv_ip, log_vct));
	if (back_count_ == agv_count_){
		if (total_count_ == 0){
			get_zip_schedule(0, 0);
		}
		int finish_count = 0;
		int finish_agv_count=0;
		for (auto&iter : map_ip_logs_){
			for (auto&vct_iter : iter.second){
				if (is_cancel_){
					return;
				}
				auto eiter = map_ip_se_connected_.find(iter.first);//当一台车断连的时候，继续拉其他车，拉此车时
				if (eiter == map_ip_se_connected_.end()){
					get_zip_schedule(total_count_, finish_agv_count + iter.second.size());
					break;
				}
				char id[8];
				itoa(eiter->second.car_id_, id, 10);
				std::string tmp = convert_positive(vct_iter, '\\', '/');
				size_t pos = tmp.find_last_of('/');
				size_t file_pos=tmp.substr(0, pos - 1).find_last_of('/');
				std::string local_file = local_path_ + "/" + id + "_" + iter.first + "/" + tmp.substr(file_pos, pos - file_pos) + tmp.substr(pos, tmp.length() - pos);
				//std::string local_file = local_path_+"/"+iter.first+"/" + tmp.substr(pos, tmp.length() - pos);
				error_fts_port_ = false;

				int ret = fts_pull_long(iter.first.c_str(), agv_port_, local_file.c_str(), vct_iter.c_str(), 0);
				if (ret >= 0){
					query_wait_.wait();
					/*if (error_fts_port_){
						get_zip_schedule(-1, -1);
						return;
					}*/
					finish_count++;
					get_zip_schedule(total_count_, finish_count);
				}
				else if (ret <0 /*-ENETDOWN*/){//发送失败，可能端口不对
					loerror("black_box") << iter.first.c_str() << " fts pull long failed.";
					get_zip_schedule(-1, -1);
					break;
				}
			}
			finish_agv_count += iter.second.size();
		}

	}
}

std::string connect_manage::convert_positive(const std::string& str, const char preview_cr, const  char new_cr){
	std::string res = str;
	for (size_t i = 0; i < res.size(); i++){
		if (res.at(i) == preview_cr){
			res.at(i) = new_cr;
		}
	}
	return res;
}

void connect_manage::cancel_query_logs()
{
	is_cancel_ = true;
}
int connect_manage::post_cancel_command()
{
	if (con_session_){
		if (con_session_->post_cancel_command() < 0){
			return -1;
		}
		else return 0;
	}
	else return -1;
}
void connect_manage::get_zip_schedule(int total_count,int finish_count)
{
	//total_count_ += total_count;

	if(schedule_callback_){
		schedule_callback_(total_count, finish_count);
	}
}

void connect_manage::schedule_regiset_callback(const std::function<void(const int, const int)>&func)
{
	if (func){
		schedule_callback_ = func;
	}
}

//void connect_manage::fts_regiset_callback(const std::function<void(const int)>&func)
//{
//	if (func){
//		fts_callback_ = func;
//	}
//}

void connect_manage::alive_check_thread()
{
	static int ALIVE_TIME_INTERAL = 2000;
	static int ALIVE_COUNT = 3;
	while (time_wait_.wait(ALIVE_TIME_INTERAL))
	{
		if (is_exits)break;

		if (con_session_)
		{
			if (con_session_->get_alive_count() > ALIVE_COUNT)
			{
				disconnect_network();
				//关闭fts长连接
				if (agv_port_ != 0)
				{
					fts_cancel(agv_ip_.c_str(), agv_port_, true);
				}
				con_session_ = nullptr;
				error_fts_port_ = true;
				query_wait_.sig();
			}
			else
			{
				con_session_->add_alive_count();
			}
		}
	}
}

void connect_manage::alive_agvs_check_thread()
{
	loinfo("black_box") << "alive thread.";
	static int ALIVE_TIME_INTERAL = 2000;
	static int ALIVE_COUNT = 3;
	bool is_delete;
	int link=-1;
	while (time_wait_.wait(ALIVE_TIME_INTERAL))
	{
		is_delete = false;
		if (is_exits)break;

		for (auto&iter : map_link_session_){
			if (iter.second.session_){
				//is_delete = false;
				if (iter.second.session_->get_alive_count() > ALIVE_COUNT)
				{
					disconnect_network(iter.first);
					//关闭fts长连接
					if (agv_port_ != 0)
					{
						fts_cancel(iter.second.agv_ip_.c_str(), agv_port_, true);					
					}
					auto eiter = map_ip_se_connected_.find(iter.second.agv_ip_);
					if (eiter != map_ip_se_connected_.end()){
						map_ip_se_connected_.erase(eiter);
					}
					is_delete = true;
					link = iter.first;
					//delete_map_link_session(iter.first);
					error_fts_port_ = true;
					query_wait_.sig();
				}
				else
				{
					iter.second.session_->add_alive_count();
				}
			}			
			
		}
		if (is_delete)
		{
			delete_map_link_session(link);
			if (map_link_session_.size() == 0){
				if (disconnect_callback_){
					disconnect_callback_(-2,"");
				}
			}
		}
	}
}