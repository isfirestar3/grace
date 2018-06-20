#pragma  once
#include "string"
#include "stdint.h"
#include "vector"

struct agv_attribute_inner {
	std::string name_;
	std::string describe_;
	agv_attribute_inner(std::string key, std::string strvalue) 
		:name_(key), 
		describe_(strvalue)
	{};

	agv_attribute_inner(){};
};

struct agv_info_inner {
	agv_info_inner(){ id_ = -1; type_ = 0; inet_ = ""; mtport_ = 0; shport_ = 0; ftsport_ = 0; hwaddr_ = ""; status_ = 2; };
	uint32_t id_;
	uint32_t type_;		// type of this vehicle
	std::string inet_;			// real/configure ipv4 address of this vehicle, like '192.168.1.101'
	uint16_t mtport_;		// port of motion template --fixed(4409)
	uint16_t shport_;		// port of agv shell --default(4410)
	uint16_t ftsport_;		// port of agv shell fts --default(4411)
	std::string hwaddr_;		// mac address of this vehicle, like '00:16:3e:06:6c:6c'
	uint16_t status_;		// status of this vehicle
	std::vector<agv_attribute_inner> vec_attrs_;
};