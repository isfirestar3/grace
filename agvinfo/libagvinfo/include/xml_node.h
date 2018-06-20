#pragma once
#include "singleton.hpp"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "inner_agvinfo.h"
#include <set>

class xml_node
{
private:
	xml_node();
	~xml_node();
	
	friend class nsp::toolkit::singleton<xml_node>;
private:
	rapidxml::xml_node<>* find_first_child_node(rapidxml::xml_node<> * root, std::string str_search);

	void refresh_file(rapidxml::xml_document<> &doc, std::string filepath);

	void detatch_file(rapidxml::file<> *file);

	int attach_file(rapidxml::xml_document<> &doc, rapidxml::file<> *&file);

	rapidxml::xml_node<>* find_child_node_by_attr(rapidxml::xml_node<> * parent, std::string strkey, std::string strValue);
public:
	int load_agvinfo(std::vector<agv_info_inner> & vec_agvinfo);

	int conver_agvinfo(std::vector<agv_info_inner> agvs);

	int get_detail(uint32_t vhid, std::vector<agv_attribute_inner> &vec_attr);

	int set_detail(uint32_t vhid, std::vector<agv_attribute_inner> detail);
public:

	std::string file_path_ = "";
	std::set<std::string> set_agvinfo_;
};

