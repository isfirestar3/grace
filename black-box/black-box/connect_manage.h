#pragma once
#include "connect_session.h"
#include "singleton.hpp"
#include <memory>
#include "os_util.hpp"
#include <functional>
#include <thread>

struct ip_session{
	std::shared_ptr<connect_session>session_;
	std::string agv_ip_;
};
struct id_ep{
	int port_;
	int car_id_;
};
struct id_session{
	int car_id_;
	std::shared_ptr<connect_session>session_;
};

class connect_manage
{
public:
	connect_manage();
	~connect_manage();
	friend class nsp::toolkit::singleton<connect_manage>;
	int init_network(std::string&ip, std::string&port);
	int init_networks(std::map<std::string, id_ep>&select_ep_);
	void on_connect(std::string&ip, const uint32_t link, bool state);
	int disconnect_network();
	int disconnect_network(const uint32_t link);
	int disconnect_network(const std::map<std::string, id_ep>&map_agv);
	void recv_log_type_vct(std::vector<std::string>&logtype_vct);
	void regiset_callback(const std::function<void(const std::vector<std::string>&log_vct)>&func);
	void schedule_regiset_callback(const std::function<void(const int, const int)>&func);
	//void fts_regiset_callback(const std::function<void(const int)>&func);
	void disconnect_regiset_callback(const std::function<void(const int,const std::string&)>&func);
	int query_log_types(std::map<std::string, int>&use_ep);
	int query_log_info(std::vector<std::string>&ip_vct, uint16_t fts_port, std::string&start_time, std::string&end_time, std::vector<std::string>&log_types, std::string& local_path);
	void query_logs_file(int link, const std::vector<std::string>&data);
	void get_zip_schedule(int total_count, int finish_count);
	int post_cancel_command();
	void cancel_query_logs();
	std::string convert_positive(const std::string& str, const char preview_cr, const  char new_cr);
private:
	/*int __stdcall fts_callback(const char *ipaddr, uint16_t port,
		const char *path_local, const char *path_remote,
		int status, int error, int step,
		void *user_data, int user_data_size)
	{
		if (fts_callback_){
			fts_callback_(step);
		}
	}*/
	void init();
	void uinit();
	void alive_check_thread();
	void alive_agvs_check_thread();
	std::shared_ptr<connect_session> create_session(const nsp::tcpip::endpoint& ep);
	void insert_session(const uint32_t link,const std::string&agv_ip, const std::shared_ptr<connect_session>& session);
	int delete_map_link_session(const uint32_t link);
	void start_to_get_logs(std::string&agv_ip, std::vector<std::string>&log_vct);
private:
	std::shared_ptr<connect_session>con_session_;
	//存放所有的clinet session对象，使用link作为索引
	std::map<uint32_t, ip_session>map_link_session_;//实际已经连上的车辆
	std::map <std::string, int>unconnect_agvs_;//选择连接，但未连接上的车辆
	//std::map<int, std::string>connected_ep_;//已经连接上的车辆link-ip

	//std::map<std::shared_ptr<connect_session>, std::string>map_session_ip_;//已经连上的车辆session-ip
	nsp::tcpip::endpoint target_ep_;
	std::function<void(const std::vector<std::string>&)>function_callback_;//日志类型回包
	std::function<void(const int, const int)>schedule_callback_;//压缩进度回包
	std::function<void(const int,const std::string&)>disconnect_callback_;//断连通知页面
	
	std::string agv_ip_;
	uint16_t agv_port_ = 0;
	std::string local_path_;
	int total_count_=0;
	bool is_cancel_=false;//拉去日志中途取消
	bool is_login_ = true;
	bool error_fts_port_ = false;

	nsp::os::waitable_handle time_wait_;
	std::thread* alive_th_ = nullptr;
	std::atomic<bool> is_exits{ false };

	std::recursive_mutex session_mutex_;
	std::recursive_mutex map_ip_mutex_;

	nsp::os::waitable_handle getlogs_wait_;
	int agv_count_ = 0;
	int back_count_ = 0;
	//std::thread*start_get_logs_=nullptr;

	std::map<std::string, std::vector<std::string>>map_ip_logs_;//存放所有agv要拉取的日志列表
	std::recursive_mutex map_logs_mutex_;

public:
	std::map<std::string, id_session>map_ip_session_;//要连接的车辆ip-session
	std::map<std::string, id_session>map_ip_se_connected_;//已经连上的车辆ip-session
};

