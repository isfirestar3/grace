#pragma once

#include "singleton.hpp"
#include <map>
#include <string>
#include "data_type.h"
#include"wheel.h"
#include"elmo.h"
#include"moons.h"
#include"angle_encoder.h"
#include "proto_typedef.h"

class config_load {

	config_load();
	~config_load();
	friend class nsp::toolkit::singleton<config_load>;

public:
	void get_struct_offset(std::map<std::string, offsetinfo>& map_elmo_var,
		std::map<std::string, offsetinfo>& map_dwheel_var,
		std::map<std::string, offsetinfo>& map_swheel_var,
		std::map<std::string, offsetinfo>& map_moos_var,
		std::map<std::string, offsetinfo>& map_angle_encoder_var);

	void get_navigation_struct(std::vector<std::string>& vct_str);
	void get_vehicle_struct(std::vector<std::string>& vct_str);
	void get_elmo_struct(std::vector<std::string>& vct_str);
	void get_moos_struct(std::vector<std::string>& vct_str);
	void get_angle_encoder_struct(std::vector<std::string>& vct_str);
	void get_dwheel_struct(std::vector<std::string>& vct_str);
	void get_swheel_struct(std::vector<std::string>& vct_str);
	void get_sddex_struct(std::vector<std::string>& vct_str);
	void get_operation_struct(std::vector<std::string>&vct_str);
	void get_optpar_struct(std::vector<std::string>&vct_str);
	void get_dio_struct(std::vector<std::string>&vct_str);
};