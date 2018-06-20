#pragma once
#include "singleton.hpp"
#include <map>
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

struct ep{
	std::string ip_port;
	int port_;
};

class rw_xml_file
{
public:
	rw_xml_file();
	~rw_xml_file();

	friend class nsp::toolkit::singleton<rw_xml_file>;

	int get_robot_ep(std::map<std::string, ep>&ep_list);
	int read_input_ep(std::map<std::string, std::string>&input_ep);
	int write_input_ep(std::map<std::string, std::string>&input_ep);
	int write_template_type(std::map<std::string,std::vector<std::string>>&template_type);
	int get_template_type(std::map<std::string, std::vector<std::string>>&template_type);
};

