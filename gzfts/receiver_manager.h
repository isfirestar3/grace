#pragma once

#include <map>
#include <thread>
#include "network_session.h"
#include "singleton.hpp"
#include "endpoint.h"
#include "gzfts_api.h"
#include "data_type.h"
#include "file_read_handler.h"
#include "file_query_catalog_task.h"

#ifdef _WIN32
#include <errno.h>
#else
#include "compiler.h"
#endif

#define DEFAULT_TIMEOUT		5000

typedef int(STDCALL *fts_callback)(const char *ipaddr, uint16_t port,
	const char *path_local, const char *path_remote,
	int status, int error, int step,
	void *user_data, int user_data_size);

class receiver_manager{
	receiver_manager();
	~receiver_manager();
	friend class nsp::toolkit::singleton<receiver_manager>;

public:
	int fts_create_receiver(const char *ipaddr_local, uint16_t port_local);
	void fts_destory_receiver(const char *ipaddr_local, uint16_t port_local);
	int fts_change_configure(const fts_parameter *param);
	void is_file_complete(const std::string& file_path, const uint64_t file_size);
	int fts_create_file_head(const uint32_t session_link, nsp::file::file_head &head, int & error_code);
	int fts_push_save_data(const uint32_t session_link, const uint64_t offset, const std::string& data);
	int fts_pull_get_file_info(const uint32_t session_link, const std::string& file_path, nsp::file::file_info& f_head, int& err_code, const bool is_long_lnk);
	int fts_pull_get_file_block_stream(const uint32_t pkt_id, const uint32_t link, const uint64_t offset, const uint32_t read_size);
	void fts_pull_send_file_block_stream(const uint32_t pkt_id, const uint32_t link, const uint64_t offset, const std::string& stream);

	int get_fts_listdir(const std::string& input_catalog, std::string & dir);
	int creat_file(const uint32_t link, nsp::file::file_head& filehead);
	int write_file(const uint32_t link, const nsp::file::file_block& f_blcok);
	void delete_link_session(const unsigned int link);
	int fts_push_next_block(const uint32_t link, const uint64_t offset);

	void fts_push_write_file_error(const uint32_t link, const nsp::proto::file_status status, const int err_code);
	void fts_pull_read_file_error(const uint32_t link, const nsp::proto::file_status status, const int err_code);

	int fts_delete_files(const std::string& file_path);
	void fts_overwrite_file(const uint32_t link, std::string& file_path);

	void fts_get_current_catalog(const std::string& f_path, const uint32_t link, const uint32_t pkt_id);
	void post_catalog_reponse(const uint32_t link, const uint32_t pkt_id, const int error, const std::vector<nsp::file::file_info>& vct_info);

	void reset_file_time(const std::string& file_name, const uint64_t& create_time, const uint64_t& modify_time, const uint64_t& access_time);

private:
	void initlization();
	std::shared_ptr<nsp::tcpip::tcp_application_service<network_session>> search_server_obj(const nsp::tcpip::endpoint& ep);
	void push_server_obj(const nsp::tcpip::endpoint&ep, const std::shared_ptr<nsp::tcpip::tcp_application_service<network_session>>& server);

	void timeout_check();
	std::string handler_relative_path(const std::string& root_path, const std::string& f_path);

private:

	nsp::os::waitable_handle test_timeout_;
	//该线程用于超时检测
	std::thread* timeout_thread_ = nullptr;
	//超时等待时间，默认为5000毫秒
	uint64_t timeout_transfer_ = 5000;
	//所有链接的IOPS量上限值；0：表示不对IOPS进行限制
	uint32_t maximum_iops_global_ = 0;
	//文件处理根目录, 默认为执行程序所在目录
	std::string file_search_root_;

	std::recursive_mutex lock_server_obj_;
	//存放receiver对象
	std::map<nsp::tcpip::endpoint, std::shared_ptr<nsp::tcpip::tcp_application_service<network_session>>> map_server_obj_;
	fts_callback callback_fun_;

	//查询当前目录下文件任务
	file_query_catalog_task query_catalog_task_;
};