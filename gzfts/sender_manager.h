#pragma once

#include "network_session.h"
#include "singleton.hpp"
#include "file_manager.h"
#include "gzfts_api.h"

#ifdef _WIN32
#include <errno.h>
#else

#endif

typedef int(STDCALL *fts_callback)(const char *ipaddr, uint16_t port,
	const char *path_local, const char *path_remote,
	int status, int error, int step,
	void *user_data, int user_data_size);

class sender_manager{
	sender_manager();
	~sender_manager();
	friend class nsp::toolkit::singleton<sender_manager>;

public:
	int fts_listdir(const char *ipaddr_to, uint16_t port_to, const char*input_catalog, char **dir, int *dircbs);
	void fts_freedir(char *dir);
	int fts_pull_file(const char *ipaddr_from, uint16_t port_from, const char *path_local, const char *path_remote, int overwrite);
	int fts_push(const char *ipaddr_to, uint16_t port_to, const char *path_local, const char *path_remote, int overwrite);
	int fts_push_send_file_block_stream(const uint32_t link, const uint64_t offset, const std::string&);
	int fts_change_configure(const struct fts_parameter *param);
	int creat_file(const uint32_t link, nsp::file::file_head& filehead, int& error_code);
	int write_file(const uint32_t link, const nsp::file::file_block& f_blcok);
	int fts_get_file_block(const uint32_t link, const uint64_t offset,const uint32_t read_size);
	int delete_map_link_session(const uint32_t link);
	int fts_pull_next_block(const uint32_t link, const uint64_t offset);
	void fts_file_complete(file_data& data);
	int fts_delete_file(const char *ipaddr_to, uint16_t port_to, const char *path_remote);
	int fts_catalog_info_req(const char *ipaddr_to, uint16_t port_to, const char *path_remote, char **output_info, int *output_cb);

	int fts_pull_write_file_error(const uint32_t link, const nsp::proto::file_status status, const int error_code);
	int fts_push_read_file_error(const uint32_t link, const nsp::proto::file_status status, const int error_code);

	void fts_cancel(const char* ipaddr, const uint16_t port);

private:
	std::shared_ptr<network_session> create_session(const nsp::tcpip::endpoint& ep);
	std::shared_ptr<network_session> search_session(const uint32_t link);
	void insert_session(const uint32_t link, const std::shared_ptr<network_session>& session);

private:
	std::recursive_mutex session_mutex_;
	//存放所有的clinet session对象，使用link作为索引
	std::map<uint32_t, std::shared_ptr<network_session>>map_link_session_;

	fts_callback callback_fun_;//用户回调函数，通知用户当前进度，对端ip，port等信息

	//每一片默认的传输的数据块大小
	uint32_t block_size_pre_transfer_ = FILE_BLOCK_SIZE;
	//传输窗口大小
	uint32_t window_size_;
	//当前窗口数
	std::atomic<uint32_t> current_window_size_{ 0 };
};