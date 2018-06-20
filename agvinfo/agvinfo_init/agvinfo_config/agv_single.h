#pragma once
#include "singleton.hpp"
#include "libagvinfo.h"
#include "functional"
#include "vector"
#include "map"

struct inner_agvinfo
{
	inner_agvinfo(){
		vhid = -1;
		vhtype = 0;
		strip = "";
		mtport = 4409;
		shport = 4410;
		ftsport = 4411;
		strmac = "";
		delete_type = false;
	};
	uint32_t vhid;
	uint32_t vhtype;		// type of this vehicle
	std::string strip;
	uint16_t mtport;		// port of motion template --fixed(4409)
	uint16_t shport;		// port of agv shell --default(4410)
	uint16_t ftsport;		// port of agv shell fts --default(4411)
	std::string strmac;
	uint16_t status;		// status of this vehicle
	std::map<std::string, std::string> map_attr;
	bool delete_type;
};

class agv_single
{
	friend class nsp::toolkit::singleton<agv_single>;
private:
	agv_single();
	~agv_single();

public:
	agv_detail* create_detail(const std::map<std::string, std::string > map_attr, int vhid) const;

	//agv_info * create_agvinfo(const inner_agvinfo &agvinfo) const;
	agv_info * create_agvinfo(const inner_agvinfo &agvinfo, const std::map<std::string, std::string > &map_attr) const;
	//int build_agvinfo(const std::vector<agv_info> &agvinfo, std::function<std::map<std::string, std::string>(int)> getattr, agv_info **agvs);

	//int build_agvinfo(const std::vector<inner_agvinfo> &agvinfo, agv_info **agvs);
	int build_agvinfo(const std::vector<inner_agvinfo> &agvinfo, std::function<std::map<std::string, std::string>(int)> getattr, agv_info **agvs);
private:
	bool create_attr(agv_attribute **attrs, const std::map<std::string, std::string > &map_attr) const;

};

