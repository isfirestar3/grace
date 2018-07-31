#include "config.h"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

config::config(){

}

config::~config(){

}

int config::read_xml_file(const std::string&file_name, firmware_info& info){
	//文件存在
	rapidxml::file<> file_xml(file_name.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(file_xml.data());
	rapidxml::xml_node<> *xml_root = doc.first_node();
	if (!xml_root){
		return -1;
	}
	for (rapidxml::xml_node<>*anode = xml_root->first_node(); anode != NULL; anode = anode->next_sibling()){
		if (strcmp(anode->name(), "modules_type") == 0){
			info.modules_type_ = anode->value();
		}
		else if (strcmp(anode->name(), "firmware_length") == 0){
			info.firmware_length_ = atoi(anode->value());
		}
		else if (strcmp(anode->name(), "firmware_path") == 0){
			info.firmware_path_ = anode->value();
		}
		else if (strcmp(anode->name(),"firmware_type") == 0){
			info.firmware_type = atoi(anode->value());
		}
		else if (strcmp(anode->name(), "firmware_version") == 0){
			info.firmware_version_ = anode->value();
		}
		else if (strcmp(anode->name(), "firmware_version_control") == 0){
			info.firmware_version_control_ = atoi(anode->value());
		}
		else if (strcmp(anode->name(), "reset_wait_time") == 0){
			info.reset_wait_time_ = atoi(anode->value());
		}
		else if (strcmp(anode->name(), "block_write_delay") == 0){
			info.block_write_delay_ = atoi(anode->value());
		}
		else if (strcmp(anode->name(), "firmware_cpu_type") == 0){
			info.firmware_cpu_type_ = anode->value();
		}
	}
	size_t pos = info.firmware_path_.find_last_of("/\\");
	info.firmware_name_ = info.firmware_path_.substr(pos + 1);
	if (info.firmware_path_.find(':') == std::string::npos)			//相对路径
	{
		size_t pospath = file_name.find_last_of("/\\");
		std::string strrelapath = file_name.substr(0,pospath+1);

		if (strrelapath.find_last_of("/\\") != strrelapath.length()-1)
		{
			strrelapath += "\\";
		}

		info.firmware_path_ = strrelapath + info.firmware_path_;
	}
	return 0;
}