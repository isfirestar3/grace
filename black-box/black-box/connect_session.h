#pragma once
#include "application_network_framwork.hpp"
#include "toolkit.h"
#include "proto_agv_msg.h"
#include "gzfts_api.h"

class connect_session : public nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>
{
public:
	connect_session();
	connect_session(HTCPLINK);
	~connect_session();
	int post_log_type_request();
	int post_log_info_request(std::string&start_time, std::string&end_time, std::vector<std::string>&log_types);
	int post_cancel_command();
	int get_alive_count(){ return alive_count; }
	void add_alive_count(){ alive_count++; }
	uint32_t get_link();
protected:
	virtual void on_connected() override final;
	virtual void on_disconnected(const HTCPLINK previous) override final;
	virtual void on_recvdata(const std::string &pkt) override final;
private:
	void on_getlog_type_list(const std::shared_ptr<nsp::proto::proto_log_type_vct>&data);
	void on_get_log_info(const std::shared_ptr<nsp::proto::proto_logs_file_path>&data);
	void on_recv_dispath(const unsigned char* data, int & cb);
	//void get_compress_schedule(const std::shared_ptr<nsp::proto::proto_logs_count>&data);

	void recv_process_status();

private:
	std::atomic<int> alive_count{ 0 };
};

