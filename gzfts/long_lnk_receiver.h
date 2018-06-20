#pragma once

#include "singleton.hpp"
#include "data_type.h"
#include "endpoint.h"
#include "long_lnk_session.h"
#include "proto_definion.h"
#include "gzfts_api.h"

class long_lnk_receiver{
	long_lnk_receiver();
	~long_lnk_receiver();
	friend nsp::toolkit::singleton<long_lnk_receiver>;

public:
	int fts_pull_get_file_info(const uint32_t session_link, const std::string& file_path,
		nsp::file::file_info& f_head, int& err_code, const bool is_long_lnk);

	int fts_pull_get_file_block_stream(const uint32_t pkt_id, const uint32_t link,
		const uint64_t file_id ,const uint64_t offset, const uint32_t read_size);

	int fts_create_receiver(const char *ipaddr_local, uint16_t port_local);

	int fts_change_configure(const struct fts_parameter *param);

	std::shared_ptr<nsp::tcpip::tcp_application_service<long_lnk_session>> search_server_obj(const nsp::tcpip::endpoint& ep);

	void push_server_obj(const nsp::tcpip::endpoint&ep, const std::shared_ptr<nsp::tcpip::tcp_application_service<long_lnk_session>>& server);

	void fts_destory_receiver(const char *ipaddr_local, uint16_t port_local);

	void fts_pull_read_file_error(const uint32_t link, const uint64_t file_id, 
		const nsp::proto::file_status status, const int err_code);

	void fts_pull_send_file_block_stream(const uint32_t pkt_id, const uint64_t file_id, const uint32_t link,
		const uint64_t offset, const std::string& stream);

	bool close_fts_file_handler(const uint32_t link, const uint64_t file_id);

private:
	void init();
	void uinit();
	void alive_check_thread();

private:
	std::recursive_mutex lock_server_obj_;
	//存放receiver对象
	std::map<nsp::tcpip::endpoint, std::shared_ptr<nsp::tcpip::tcp_application_service<long_lnk_session>>> map_server_obj_;
	//文件处理根目录, 默认为执行程序所在目录
	std::string file_search_root_;
	//超时等待时间，默认为5000毫秒
	uint64_t timeout_transfer_ = 5000;
	//所有链接的IOPS量上限值；0：表示不对IOPS进行限制
	uint32_t maximum_iops_global_ = 0;

	//检测心跳线程
	nsp::os::waitable_handle time_wait_;
	std::thread* alive_th_ = nullptr;
	std::atomic<int> is_exits_{ false };
};
