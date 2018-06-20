#pragma once
#include "application_network_framwork.hpp"
#include "motion_ack.h"
#include "agvinfo_proto.hpp"
#include "inner_agvinfo.h"

class agv_session :public nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>
{
public:
	agv_session();
	agv_session(std::function<void(std::string)> callback);
	~agv_session();

	void on_recvdata(const std::string &pkt) final;

	int load_agvinfo(std::shared_ptr<agv_motion::motion_ack> motion, int method);

	int set_agvinfo(std::shared_ptr<agv_motion::motion_ack> motion, int method, std::vector<agv_info_inner>& vecagvinfo);

	int get_agvdetail(std::shared_ptr<agv_motion::motion_ack> motion, int agvid, int method);

	int set_agvdetail(std::shared_ptr<agv_motion::motion_ack> motion, int agvid, const std::vector<agv_attribute_inner> & vec_attr);

	void on_disconnected(const HTCPLINK previous);

	void refresh_tick(){ tick_ = nsp::os::gettick(); };

	uint64_t get_tick() const { return tick_; };

	int keepalive();
private:
	std::function<void(std::string)> disconnect_callback_ = nullptr;
	uint64_t tick_;
};

