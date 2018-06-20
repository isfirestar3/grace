#pragma once

#include "application_network_framwork.hpp"
#include "proto_msg.h"
#include "proto_typedef.h"
#include "data_type.h"
#include "sender_pull_long_task.h"
#include <memory>

class long_lnk_session : public nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>
{
public:
	long_lnk_session();
	long_lnk_session(HTCPLINK lnk);
	~long_lnk_session();

public:
	//pull长连接操作
	int post_pull_file_long(const std::string& path_remote, const std::string& path_local, const int overwrite, const uint64_t pull_id);

	void set_remote_endpoin(const nsp::tcpip::endpoint& ep){ remote_ep_ = ep; }
	nsp::tcpip::endpoint get_remote_endpoint(){ return remote_ep_; }
	uint32_t get_link(){ return this->lnk_; }
	void set_transfer_block_size(uint32_t size){ block_size_pre_transfer_ = size; }

	int post_pull_file_block_response(const uint32_t pkt_id, const uint64_t file_id, const uint64_t offset, const std::string& data);
	int post_pull_next_block_request(const uint64_t file_id, const uint64_t offset);
	int post_pull_complete(const uint64_t file_id);
	int post_pull_file_error(const uint64_t file_id, const int status, const int err_code);

	int post_alive_pkt();

	void get_file_status(const uint64_t fd,file_callabck_data& data);
	
	int get_alive_count(){ return alive_count; }
	void add_alive_count(){ alive_count++; }

protected:
	virtual void on_disconnected(const HTCPLINK previous) override final;
	virtual void on_recvdata(const std::string &pkt) override final;

private:
	int dispatch_package(int package_type, const std::string &pkt_data, const std::shared_ptr<nsp::proto::proto_head> &head);

	/////////////////////////以下为长连接操作/////////////////////////////
	//pull长连接操作，receive session收到获取文件头请求操作
	int recv_request_file_long(const std::shared_ptr<nsp::proto::_proto_request_file_long_lnk>&pkt_data);
	//pull长连接操作，sender session收到文件头信息
	int recv_fts_file_head_long(const std::shared_ptr<nsp::proto::_proto_write_head_long_lnk> &data);
	int recv_fts_requst_file_data(const std::shared_ptr<nsp::proto::_proto_long_lnk_request_file_data> &data);
	int recv_fts_requst_file_data_ack(const std::shared_ptr<nsp::proto::_proto_file_long_lnk_data> &data);
	int recv_fts_file_complete(const std::shared_ptr<nsp::proto::_proto_long_lnk_identify> &data);
	int recv_fts_long_alive();


	void file_rw_handler(const nsp::file::current_identify identify, const int pkt_type,
		const int step, const uint64_t file_id,
		const nsp::proto::file_status status, const int error_code);

	void close_file_handler_and_remove_fd(const uint32_t lnk, const uint64_t fd);

private:
	std::atomic<uint64_t> refresh_tick_{ 0 };//记录当前时间点
	//每一片默认的传输的数据块大小
	uint32_t block_size_pre_transfer_;

	nsp::tcpip::endpoint remote_ep_;
	std::shared_ptr<sender_pull_long_task> pull_task_ = nullptr;

	std::atomic<int> alive_count{ 0 };
};