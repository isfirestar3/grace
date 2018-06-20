#pragma once

#include "singleton.hpp"
#include "agv_session.h"
#include "agvinfo_proto.hpp"
#include "libagvinfo.h"
#include "inner_agvinfo.h"
#include "thread"
#include <condition_variable>

class agv_manager
{
private:
	agv_manager();
	~agv_manager();
	friend class nsp::toolkit::singleton<agv_manager>;
private:
	void on_disconnect(std::string ipstr);

	int build_agvinfo(const std::vector<agv_info_inner> &, agv_info **agvs);

	agv_info *new_agvinfo(const agv_info_inner&);

	int build_attr(const std::vector<agv_attribute_inner> &vec_attr, struct agv_attribute *&attr);

	int build_agvdetail(uint32_t vhid, const std::vector<agv_attribute_inner> &vec_attr, struct agv_detail **detail);

	int set_agvdetail(uint32_t vhid, const std::vector<agv_attribute_inner> &attribute);

	void build_inner_agvinfo(const struct agv_info *agvs, std::vector<agv_info_inner> &vec_agvinfo);

	int load_server_agvinfo(enum load_agvinfo_method method, std::vector<agv_info_inner> & agvinfo);

	int conver_server_agvinfo(std::vector<agv_info_inner> &, enum load_agvinfo_method method);

	int get_server_detail(uint32_t vhid, enum load_agvinfo_method method, std::vector<agv_attribute_inner> &vec_attr);

	void release_attribute(struct agv_attribute * attr);

	int check_connect();
public:
	int connect_server(std::string ip, uint16_t port);

	int load_agvinfo(struct agv_info **agvs, enum load_agvinfo_method method);

	void about_something_changed(void(*notify)());

	void release_agvinfo(struct agv_info *agvs);

	void release_agvdetail(struct agv_detail *detail);

	int cover_agvinfo(const struct agv_info *agvs, enum load_agvinfo_method method);
	
	int get_agvdetail(uint32_t vhid, struct agv_detail **detail, enum load_agvinfo_method method);

	int set_agvdetail(uint32_t vhid, const struct agv_detail *detail, enum load_agvinfo_method method);

	void disconnect();
private:
	std::shared_ptr<agv_session> agv_session_ = nullptr;
	std::condition_variable session_condition_;
	std::mutex mutex_;
	std::thread * thread_ = nullptr;
	std::string ipstr_ = "";
	uint16_t port_;
};

