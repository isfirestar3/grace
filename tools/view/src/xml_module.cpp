#define _CRT_SECURE_NO_WARNINGS
#include "xml_module.h"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include <stdio.h>
#include "os_util.hpp"
#include <fstream>
#include <iostream>
#include "shlwapi.h"
#include <io.h>

#pragma comment(lib,"shlwapi.lib")
extern
std::string EncodeUtf8fromString(std::string in);

bool __stdcall is_file_exist(const char* path){
	if (!path) return false;
	return _access(path, 0) < 0 ? false : true;
}

void __stdcall add_xml_declaration(rapidxml::xml_document<>& doc){
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	rapidxml::xml_attribute<>* decl_ver = doc.allocate_attribute("version", "1.0");
	rapidxml::xml_attribute<>* decl_encode = doc.allocate_attribute("encoding", "UTF-8");
	decl->append_attribute(decl_ver);
	decl->append_attribute(decl_encode);
	doc.prepend_node(decl);
}

int __stdcall add_canbus_element(can_bus& can_bus_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	std::basic_string<char> path = SaveResult + "\\etc\\dev";
	SaveResult += "\\etc\\dev\\canbus.xml";
	if (!is_file_exist(path.c_str())){
		nsp::os::mkdir<char>(path);
	}
	char id[10];
	char canid[10];
	char type[10];
	char baud[16];

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<> *xml_root;
	if (!is_file_exist(SaveResult.c_str())){
		//文件不存在
		add_xml_declaration(doc);
		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "can", "information");
		doc.append_node(node);
		
		rapidxml::xml_node<>* canbus = doc.allocate_node(rapidxml::node_element, "bus", NULL);
		node->append_node(canbus);
		canbus->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(can_bus_ele.id_, id, 10)));
		canbus->append_node(doc.allocate_node(rapidxml::node_element, "name", can_bus_ele.name_));
		canbus->append_node(doc.allocate_node(rapidxml::node_element, "canid", _itoa(can_bus_ele.canbus_id_, canid, 10)));
		canbus->append_node(doc.allocate_node(rapidxml::node_element, "type", _itoa(can_bus_ele.canbus_type_, type, 10)));
		canbus->append_node(doc.allocate_node(rapidxml::node_element, "baud", _itoa(can_bus_ele.baud_rate_, baud, 10)));

		std::ofstream out(SaveResult);
		out << doc;
		out.close();
	}
	else{
		//文件存在
		rapidxml::file<> file_xml(SaveResult.c_str());
		doc.parse<0>(file_xml.data());
		add_xml_declaration(doc);

		for (xml_root = doc.first_node(); xml_root != NULL; xml_root = xml_root->next_sibling()){
			if (strcmp(xml_root->name(), "can") == 0){
				rapidxml::xml_node<>* canbus = doc.allocate_node(rapidxml::node_element, "bus", NULL);
				canbus->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(can_bus_ele.id_, id, 10)));
				canbus->append_node(doc.allocate_node(rapidxml::node_element, "name", can_bus_ele.name_));
				canbus->append_node(doc.allocate_node(rapidxml::node_element, "canid", _itoa(can_bus_ele.canbus_id_, canid, 10)));
				canbus->append_node(doc.allocate_node(rapidxml::node_element, "type", _itoa(can_bus_ele.canbus_type_, type, 10)));
				canbus->append_node(doc.allocate_node(rapidxml::node_element, "baud", _itoa(can_bus_ele.baud_rate_, baud, 10)));
				xml_root->append_node(canbus);
			}
		}

		std::ofstream out(SaveResult);
		out << doc;
		out.close();
	}
	return 0;

}

int __stdcall add_elmo_element(elmo& elmo_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\elmo.xml";

	char id[10];
	char can[10];
	char port[10];
	char elmonode[10];
	char latency[32];
	char merge[10];
	char profile_speed[16];	
	char profile_acc[16];	
	char profile_dec[16];	
	sprintf(latency, "%lld", elmo_ele.var_elmo_info.candev_head_.latency_);
	sprintf(profile_speed, "%f", elmo_ele.var_elmo_info.profile_speed_);
	sprintf(profile_acc, "%f", elmo_ele.var_elmo_info.profile_acc_);
	sprintf(profile_dec, "%f", elmo_ele.var_elmo_info.profile_dec_);

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* xml_root = doc.first_node();
		rapidxml::xml_node<>* mynode = xml_root->first_node();
		rapidxml::xml_node<>* elmo = doc.allocate_node(rapidxml::node_element, "device", NULL);

		elmo->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(elmo_ele.id_, id, 10)));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "name", elmo_ele.name_));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "can", _itoa(elmo_ele.var_elmo_info.candev_head_.canbus_, can, 10)));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "port", _itoa(elmo_ele.var_elmo_info.candev_head_.canport_, port, 10)));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "node", _itoa(elmo_ele.var_elmo_info.candev_head_.cannode_, elmonode, 10)));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "latency", latency));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "merge", _itoa(elmo_ele.var_elmo_info.candev_head_.merge_, merge, 10)));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "profile_speed", profile_speed));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "profile_acc", profile_acc));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "profile_dec", profile_dec));

		xml_root->insert_node(mynode, elmo);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "elmo", "information");
		doc.append_node(node);	
		node->append_node(doc.allocate_node(rapidxml::node_element,"mnt","elmo.dll" ));
		rapidxml::xml_node<>* elmo = doc.allocate_node(rapidxml::node_element, "device", NULL);
		node->append_node(elmo);
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(elmo_ele.id_, id, 10)));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "name", elmo_ele.name_));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "can", _itoa(elmo_ele.var_elmo_info.candev_head_.canbus_, can, 10)));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "port", _itoa(elmo_ele.var_elmo_info.candev_head_.canport_, port, 10)));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "node", _itoa(elmo_ele.var_elmo_info.candev_head_.cannode_, elmonode, 10)));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "latency", latency));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "merge", _itoa(elmo_ele.var_elmo_info.candev_head_.merge_, merge, 10)));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "profile_speed", profile_speed));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "profile_acc", profile_acc));
		elmo->append_node(doc.allocate_node(rapidxml::node_element, "profile_dec", profile_dec));
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
	}
	return 0;
}

int __stdcall add_copley_element(copley& copley_ele)
{
	return 0;
}

int __stdcall get_canbus_element(std::vector<can_bus>&vct_can_bus)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\canbus.xml";

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try{
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (NULL == root){
			delete file;
			return -1;
		}
		rapidxml::xml_node<>*anode = root->first_node();
		if (anode != NULL)
		{
			rapidxml::xml_node<>*cnode = anode->first_node();
			if (root)
			{
				can_bus tmp;
				for (anode = root->first_node(); anode != NULL; anode = anode->next_sibling())
				{
					bool elment_exist = true;
					cnode = anode->first_node();
					if (NULL == cnode){ 
						elment_exist = false;
					}
					while (cnode){
						if (0 == strcmp("id", cnode->name())){
							tmp.id_ = atoi(cnode->value());
						}else if (0 == strcmp("name", cnode->name())){
							strcpy(tmp.name_, cnode->value());
						}else if (0 == strcmp("canid", cnode->name())){
							tmp.canbus_id_ = atoi(cnode->value());
						}else if (0 == strcmp("type", cnode->name())){
							tmp.canbus_type_ = atoi(cnode->value());
						}else if (0 == strcmp("baud", cnode->name())){
							tmp.baud_rate_ = atoi(cnode->value());
						}
						cnode = cnode->next_sibling();
					}
					if (elment_exist){
						vct_can_bus.push_back(tmp);
					}
				}
			}
		}
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall get_elmo_element(std::vector<elmo>& elmo_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\elmo.xml";

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try{
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (root == NULL){
			delete file;
			return -1;
		}
		rapidxml::xml_node<>*anode = root->first_node();
		if (anode != NULL)
		{
			rapidxml::xml_node<>*cnode = anode->first_node();
			if (root)
			{
				elmo tmp;
				for (rapidxml::xml_node<>*anode = root->first_node(); anode != NULL; anode = anode->next_sibling())
				{
					bool element_exist = true;
					cnode = anode->first_node();
					if (NULL == cnode){
						element_exist = false;
					}
					while (cnode){
						if (0 == strcmp("id", cnode->name())){
							tmp.id_ = atoi(cnode->value());
						}
						else if (0 == strcmp("name", cnode->name())){
							strcpy(tmp.name_, cnode->value());
						}
						else if (0 == strcmp("can", cnode->name())){
							tmp.var_elmo_info.candev_head_.canbus_ = atoi(cnode->value());
						}
						else if (0 == strcmp("port", cnode->name())){
							tmp.var_elmo_info.candev_head_.canport_ = atoi(cnode->value());
						}
						else if (0 == strcmp("node", cnode->name())){
							tmp.var_elmo_info.candev_head_.cannode_ = atoi(cnode->value());
						}
						else if (0 == strcmp("latency", cnode->name())){
							sscanf_s(cnode->value(), "%lld", &tmp.var_elmo_info.candev_head_.latency_);
						}
						else if (0 == strcmp("merge", cnode->name())){
							tmp.var_elmo_info.candev_head_.merge_ = atoi(cnode->value());
						}
						else if (0 == strcmp("profile_speed", cnode->name())){
							tmp.var_elmo_info.profile_speed_ = atof(cnode->value());
						}
						else if (0 == strcmp("profile_acc", cnode->name())){
							tmp.var_elmo_info.profile_acc_ = atof(cnode->value());
						}
						else if (0 == strcmp("profile_dec", cnode->name())){
							tmp.var_elmo_info.profile_dec_ = atof(cnode->value());
						}
						cnode = cnode->next_sibling();
					}
					if (element_exist){
						elmo_ele.push_back(tmp);
					}
				}
			}
		}
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall add_driveunit_element(driveunit& driveunit_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	std::basic_string<char> path = SaveResult + "\\etc\\var";
	SaveResult += "\\etc\\var\\driveunit.xml";
	std::fstream _file;
	_file.open(path, std::ios::in);
	if (!_file){
		int res = nsp::os::mkdir<char>(path);
	}
	_file.close();
	char id[10];
	char type[10];
	char install_x[16];
	char install_y[16];
	char install_theta[16];
	char calibrated_x[16];
	char calibrated_y[16];
	char calibrated_theta[16];

	sprintf(install_x, "%f", driveunit_ele.var__driveunit_info.install_.x_);
	sprintf(install_y, "%f", driveunit_ele.var__driveunit_info.install_.y_);
	sprintf(install_theta, "%f", driveunit_ele.var__driveunit_info.install_.theta_);
	sprintf(calibrated_x, "%f", driveunit_ele.var__driveunit_info.calibrated_.x_);
	sprintf(calibrated_y, "%f", driveunit_ele.var__driveunit_info.calibrated_.y_);
	sprintf(calibrated_theta, "%f", driveunit_ele.var__driveunit_info.calibrated_.theta_);

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* xml_root = doc.first_node();
		rapidxml::xml_node<>* mynode = xml_root->first_node();
		rapidxml::xml_node<>* driveunit = doc.allocate_node(rapidxml::node_element, "unit", NULL);

		driveunit->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(driveunit_ele.id_, id, 10)));
		driveunit->append_node(doc.allocate_node(rapidxml::node_element, "name", driveunit_ele.name_));
		driveunit->append_node(doc.allocate_node(rapidxml::node_element, "type", _itoa(driveunit_ele.var__driveunit_info.unit_type_, type, 10)));
		rapidxml::xml_node<>*install = doc.allocate_node(rapidxml::node_element, "install");
		driveunit->append_node(install);
		install->append_attribute(doc.allocate_attribute("x", install_x));
		install->append_attribute(doc.allocate_attribute("y", install_y));
		install->append_attribute(doc.allocate_attribute("theta", install_theta));
		rapidxml::xml_node<>*calibrated = doc.allocate_node(rapidxml::node_element, "calibrated");
		driveunit->append_node(calibrated);
		calibrated->append_attribute(doc.allocate_attribute("x", calibrated_x));
		calibrated->append_attribute(doc.allocate_attribute("y", calibrated_y));
		calibrated->append_attribute(doc.allocate_attribute("theta", calibrated_theta));
		rapidxml::xml_node<>*ele = doc.allocate_node(rapidxml::node_element, "wheels", NULL);
		driveunit->append_node(ele);

		char tmp[100][10];
		for (unsigned int i = 0; i < driveunit_ele.wheel_.size(); i++){
			//char tmp[10];
			ele->append_node(doc.allocate_node(rapidxml::node_element, "wheel", _itoa(driveunit_ele.wheel_[i], tmp[i], 10)));
		}

		xml_root->insert_node(mynode, driveunit);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "drives", "information");
		doc.append_node(node);
		rapidxml::xml_node<>* driveunit = doc.allocate_node(rapidxml::node_element, "unit", NULL);
		node->append_node(driveunit);
		driveunit->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(driveunit_ele.id_, id, 10)));
		driveunit->append_node(doc.allocate_node(rapidxml::node_element, "name", driveunit_ele.name_));
		driveunit->append_node(doc.allocate_node(rapidxml::node_element, "type", _itoa(driveunit_ele.var__driveunit_info.unit_type_, type, 10)));
		rapidxml::xml_node<>*install = doc.allocate_node(rapidxml::node_element, "install");
		driveunit->append_node(install);
		install->append_attribute(doc.allocate_attribute("x", install_x));
		install->append_attribute(doc.allocate_attribute("y", install_y));
		install->append_attribute(doc.allocate_attribute("theta", install_theta));
		rapidxml::xml_node<>*calibrated = doc.allocate_node(rapidxml::node_element, "calibrated");
		driveunit->append_node(calibrated);
		calibrated->append_attribute(doc.allocate_attribute("x", calibrated_x));
		calibrated->append_attribute(doc.allocate_attribute("y", calibrated_y));
		calibrated->append_attribute(doc.allocate_attribute("theta", calibrated_theta));
		rapidxml::xml_node<>*ele = doc.allocate_node(rapidxml::node_element, "wheels", NULL);
		driveunit->append_node(ele);
		
		char tmp[100][10];
		for (unsigned int i = 0; i < driveunit_ele.wheel_.size(); i++){
			//char tmp[10];
			ele->append_node(doc.allocate_node(rapidxml::node_element, "wheel", _itoa(driveunit_ele.wheel_[i], tmp[i], 10)));
		}
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
	}
	return 0;
}

int __stdcall get_driveunit_element(std::vector<driveunit>& driveunit_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\driveunit.xml";
	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try{
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (root)
		{
			rapidxml::xml_node<>*anode = root->first_node();
			if (anode != NULL){
				rapidxml::xml_node<>*bnode = anode->first_node();
				if (NULL == bnode){
					delete file;
					return -1;
				}
				/*rapidxml::xml_attribute<char> * attr = bnode->first_attribute();
				rapidxml::xml_node<>*cnode = bnode->first_node();*/
				for (anode = root->first_node(); anode != NULL; anode = anode->next_sibling())
				{
					driveunit tmp;
					bool element_exist = true;
					bnode = anode->first_node();
					if (NULL == bnode){
						element_exist = false;
					}
					while (bnode)
					{
						if (0 == strcmp("id", bnode->name())){
							tmp.id_ = atoi(bnode->value());
						}
						else if (0 == strcmp("name", bnode->name())){
							strcpy(tmp.name_, bnode->value());
						}
						else if (0 == strcmp("type", bnode->name())){
							tmp.var__driveunit_info.unit_type_ = (drive_unit_type_t)atoi(bnode->value());
						}
						else if (0 == strcmp("install", bnode->name())){
							rapidxml::xml_attribute<char> *attr = bnode->first_attribute();
							while (attr){
								if (0 == strcmp("x", attr->name())){
									tmp.var__driveunit_info.install_.x_ = atof(attr->value());
								}
								else if (0 == strcmp("y", attr->name())){
									tmp.var__driveunit_info.install_.y_ = atof(attr->value());
								}
								else if (0 == strcmp("theta", attr->name())){
									tmp.var__driveunit_info.install_.theta_ = atof(attr->value());
								}
								attr = attr->next_attribute();
							}
						}
						else if (0 == strcmp("calibrated", bnode->name())){
							rapidxml::xml_attribute<char> *attr = bnode->first_attribute();
							while (attr){
								if (0 == strcmp("x", attr->name())){
									tmp.var__driveunit_info.calibrated_.x_ = atof(attr->value());
								}
								else if (0 == strcmp("y", attr->name())){
									tmp.var__driveunit_info.calibrated_.y_ = atof(attr->value());
								}
								else if (0 == strcmp("theta", attr->name())){
									tmp.var__driveunit_info.calibrated_.theta_ = atof(attr->value());
								}
								attr = attr->next_attribute();
							}
						}
						else if (0 == strcmp("wheels", bnode->name())){
							rapidxml::xml_node<>*cnode = bnode->first_node();
							while (cnode){
								if (0 == strcmp("wheel", cnode->name())){
									tmp.wheel_.push_back(atoi(cnode->value()));
								}
								cnode = cnode->next_sibling();
							}
						}
						bnode = bnode->next_sibling();
					}
					if (element_exist){
						driveunit_ele.push_back(tmp);
					}
				}
			}
		}
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall add_dwheel_element(dwheel& dwheel_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	std::basic_string<char> path = SaveResult + "\\etc\\var";
	SaveResult += "\\etc\\var\\dwheel.xml";
	std::fstream _file;
	_file.open(path, std::ios::in);
	if (!_file){
		int res = nsp::os::mkdir<char>(path);
	}
	_file.close();
	char id[10];
	char max_speed[16];
	char max_acc[16];
	char max_dec[16];
	char enable[10];
	char control_mode[10];
	char scale_control[16];		
	char scale_feedback[16];			
	char roll_weight[16];	
	char slide_weight[16];	

	sprintf(max_speed, "%f", dwheel_ele.var__dwheel_info.max_speed_);
	sprintf(max_acc, "%f", dwheel_ele.var__dwheel_info.max_acc_);
	sprintf(max_dec, "%f", dwheel_ele.var__dwheel_info.max_dec_);
	sprintf(scale_control, "%f", dwheel_ele.var__dwheel_info.scale_control_);
	sprintf(scale_feedback, "%f", dwheel_ele.var__dwheel_info.scale_feedback_);
	sprintf(roll_weight, "%f", dwheel_ele.var__dwheel_info.roll_weight_);
	sprintf(slide_weight, "%f", dwheel_ele.var__dwheel_info.slide_weight_);

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* xml_root = doc.first_node();
		rapidxml::xml_node<>* mynode = xml_root->first_node();
		rapidxml::xml_node<>* dwheel = doc.allocate_node(rapidxml::node_element, "wheel", NULL);

		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(dwheel_ele.id_, id, 10)));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "name", dwheel_ele.name_));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "max_speed", max_speed));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "max_acc",max_acc));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "max_dec", max_dec));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "enable", _itoa(dwheel_ele.var__dwheel_info.i.enable_, enable, 10)));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "control_mode", _itoa(dwheel_ele.var__dwheel_info.control_mode_, control_mode, 10)));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "scale_control",scale_control));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "scale_feedback", scale_feedback));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "roll_weight", roll_weight));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "slide_weight", slide_weight));

		xml_root->insert_node(mynode, dwheel);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "wheels", "information");
		doc.append_node(node);
		rapidxml::xml_node<>* dwheel = doc.allocate_node(rapidxml::node_element, "wheel", NULL);
		node->append_node(dwheel);

		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(dwheel_ele.id_, id, 10)));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "name", dwheel_ele.name_));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "max_speed", max_speed));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "max_acc", max_acc));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "max_dec", max_dec));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "enable", _itoa(dwheel_ele.var__dwheel_info.i.enable_, enable, 10)));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "control_mode", _itoa(dwheel_ele.var__dwheel_info.control_mode_, control_mode, 10)));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "scale_control", scale_control));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "scale_feedback", scale_feedback));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "roll_weight", roll_weight));
		dwheel->append_node(doc.allocate_node(rapidxml::node_element, "slide_weight", slide_weight));

		std::ofstream out(SaveResult);
		out << doc;
		out.close();
	}
	return 0;
}

int __stdcall get_dwheel_element(std::vector<dwheel>& dwheel_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\dwheel.xml";

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try{
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (root)
		{
			rapidxml::xml_node<>*anode = root->first_node();
			if (anode != NULL)
			{
				rapidxml::xml_node<>*cnode = anode->first_node();
				dwheel tmp;
				for (anode = root->first_node(); anode != NULL; anode = anode->next_sibling())
				{
					bool element_exist = true;
					cnode = anode->first_node();
					if (NULL == cnode){
						element_exist = false;
					}
					while (cnode)
					{
						if (0 == strcmp("id", cnode->name())){
							tmp.id_ = atoi(cnode->value());
						}else if (0 == strcmp("name", cnode->name())){
							strcpy(tmp.name_, cnode->value());
						}else if (0 == strcmp("max_speed", cnode->name())){
							tmp.var__dwheel_info.max_speed_=atof(cnode->value());
						}else if (0 == strcmp("max_acc", cnode->name())){
							tmp.var__dwheel_info.max_acc_ = atof(cnode->value());
						}else if (0 == strcmp("max_dec", cnode->name())){
							tmp.var__dwheel_info.max_dec_ = atof(cnode->value());
						}else if (0 == strcmp("enable", cnode->name())){
							tmp.var__dwheel_info.i.enable_ = atoi(cnode->value());
						}else if (0 == strcmp("control_mode", cnode->name())){
							tmp.var__dwheel_info.control_mode_ = (dwheel_ctrlmod_t)atoi(cnode->value());
						}else if (0 == strcmp("scale_control", cnode->name())){
							tmp.var__dwheel_info.scale_control_ = atof(cnode->value());
						}else if (0 == strcmp("scale_feedback", cnode->name())){
							tmp.var__dwheel_info.scale_feedback_ = atof(cnode->value());
						}else if (0 == strcmp("roll_weight", cnode->name())){
							tmp.var__dwheel_info.roll_weight_ = atof(cnode->value());
						}else if (0 == strcmp("slide_weight", cnode->name())){
							tmp.var__dwheel_info.slide_weight_ = atof(cnode->value());
						}
						cnode = cnode->next_sibling();
					}
					if (element_exist){
						dwheel_ele.push_back(tmp);
					}
				}
			}
			
		}
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall add_swheel_element(swheel& swheel_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	std::basic_string<char> path = SaveResult + "\\etc\\var";
	SaveResult += "\\etc\\var\\swheel.xml";
	std::fstream _file;
	_file.open(path, std::ios::in);
	if (!_file){
		int res = nsp::os::mkdir<char>(path);
	}
	_file.close();
	char id[10];
	char min_angle[64];
	char max_angle[64];
	char max_w[64];
	char enable[10];
	char control_mode[10];
	char scale_control[64];
	char scale_feedback[64];
	char control_cp[64];
	char control_ci[64];
	char control_cd[64];
	char zero_angle[64];
	char zero_angle_enc[64];

	sprintf(min_angle, "%f", swheel_ele.var__swheel_info.min_angle_);
	sprintf(max_angle, "%f", swheel_ele.var__swheel_info.max_angle_);
	sprintf(max_w, "%f", swheel_ele.var__swheel_info.max_w_);
	sprintf(scale_control, "%f", swheel_ele.var__swheel_info.scale_control_);
	sprintf(scale_feedback, "%f", swheel_ele.var__swheel_info.scale_feedback_);
	sprintf(control_cp, "%.4f", swheel_ele.var__swheel_info.control_cp_);
	sprintf(control_ci, "%.4f", swheel_ele.var__swheel_info.control_ci_);
	sprintf(control_cd, "%.4f", swheel_ele.var__swheel_info.control_cd_);
	sprintf(zero_angle, "%.4f", swheel_ele.var__swheel_info.zero_angle_);
	sprintf(zero_angle_enc, "%.4f", swheel_ele.var__swheel_info.zero_angle_enc_);

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* xml_root = doc.first_node();
		rapidxml::xml_node<>* mynode = xml_root->first_node();
		rapidxml::xml_node<>* swheel = doc.allocate_node(rapidxml::node_element, "wheel", NULL);

		swheel->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(swheel_ele.id_, id, 10)));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "name", swheel_ele.name_));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "min_angle", min_angle));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "max_angle", max_angle));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "zero_angle", zero_angle));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "zero_angle_enc", zero_angle_enc));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "max_w", max_w));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "enable", _itoa(swheel_ele.var__swheel_info.i.enable_, enable, 10)));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "control_mode", _itoa(swheel_ele.var__swheel_info.control_mode_, control_mode, 10)));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "scale_control", scale_control));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "scale_feedback", scale_feedback));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "control_cp", control_cp));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "control_ci", control_ci));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "control_cd", control_cd));

		xml_root->insert_node(mynode, swheel);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "wheels", "information");
		doc.append_node(node);
		rapidxml::xml_node<>* swheel = doc.allocate_node(rapidxml::node_element, "wheel", NULL);
		node->append_node(swheel);

		swheel->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(swheel_ele.id_, id, 10)));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "name", swheel_ele.name_));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "min_angle", min_angle));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "max_angle", max_angle));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "zero_angle", zero_angle));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "zero_angle_enc", zero_angle_enc));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "max_w", max_w));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "enable", _itoa(swheel_ele.var__swheel_info.i.enable_, enable, 10)));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "control_mode", _itoa(swheel_ele.var__swheel_info.control_mode_, control_mode, 10)));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "scale_control", scale_control));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "scale_feedback", scale_feedback));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "control_cp", control_cp));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "control_ci", control_ci));
		swheel->append_node(doc.allocate_node(rapidxml::node_element, "control_cd", control_cd));

		std::ofstream out(SaveResult);
		out << doc;
		out.close();
	}
	return 0;
}

int __stdcall get_swheel_element(std::vector<swheel>& swheel_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\swheel.xml";

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try{
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (root)
		{
			rapidxml::xml_node<>*anode = root->first_node();
			if (anode != NULL)
			{
				for (anode = root->first_node(); anode != NULL; anode = anode->next_sibling())
				{
					swheel tmp;
					bool element_exist = true;
					rapidxml::xml_node<>* cnode = anode->first_node();
					if (NULL == cnode){
						element_exist = false;
					}
					while (cnode)
					{
						if (0 == strcmp("id", cnode->name())){
							tmp.id_ = atoi(cnode->value());
						}else if (0 == strcmp("name", cnode->name())){
							strcpy(tmp.name_, cnode->value());
						}else if (0 == strcmp("min_angle", cnode->name())){
							tmp.var__swheel_info.min_angle_ = atof(cnode->value());
						}else if (0 == strcmp("max_angle", cnode->name())){
							tmp.var__swheel_info.max_angle_ = atof(cnode->value());
						}else if (0 == strcmp("zero_angle", cnode->name())){
							tmp.var__swheel_info.zero_angle_ = atof(cnode->value());
						}else if (0 == strcmp("zero_angle_enc", cnode->name())){
							tmp.var__swheel_info.zero_angle_enc_ = atof(cnode->value());
						}else if (0 == strcmp("max_w", cnode->name())){
							tmp.var__swheel_info.max_w_ = atof(cnode->value());
						}else if (0 == strcmp("enable", cnode->name())){
							tmp.var__swheel_info.i.enable_ = atoi(cnode->value());
						}else if (0 == strcmp("control_mode", cnode->name())){
							if (strcmp(cnode->value(), "") == 0){
								tmp.var__swheel_info.control_mode_ = (swheel_ctrlmod_t)-1;
							}else {
								tmp.var__swheel_info.control_mode_ = (swheel_ctrlmod_t)atoi(cnode->value());
							}
						}else if (0 == strcmp("scale_control", cnode->name())){
							tmp.var__swheel_info.scale_control_ = atof(cnode->value());
						}else if (0 == strcmp("scale_feedback", cnode->name())){
							tmp.var__swheel_info.scale_feedback_ = atof(cnode->value());
						}else if (0 == strcmp("control_cp", cnode->name())){
							tmp.var__swheel_info.control_cp_ = atof(cnode->value());
						}else if (0 == strcmp("control_ci", cnode->name())){
							tmp.var__swheel_info.control_ci_ = atof(cnode->value());
						}else if (0 == strcmp("control_cd", cnode->name())){
							tmp.var__swheel_info.control_cd_ = atof(cnode->value());
						}
						cnode = cnode->next_sibling();
					}
					if (element_exist){
						swheel_ele.push_back(tmp);
					}
				}
			}
		}
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall add_sddex_element(sddex& sddex_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();

	std::basic_string<char> path = SaveResult + "\\etc\\var";
	SaveResult += "\\etc\\var\\sddex.xml";
	std::fstream _file;
	_file.open(path, std::ios::in);
	if (!_file){
		int res = nsp::os::mkdir<char>(path);
	}
	_file.close();
	char id[10];
	char gauge[16];
	sprintf(gauge, "%f", sddex_ele.var__sdd_extra_info.gauge_);
	
	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* xml_root = doc.first_node();
		rapidxml::xml_node<>* mynode = xml_root->first_node();
		rapidxml::xml_node<>* sddex = doc.allocate_node(rapidxml::node_element, "wheel", NULL);

		sddex->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(sddex_ele.id_, id, 10)));
		sddex->append_node(doc.allocate_node(rapidxml::node_element, "name", sddex_ele.name_));
		sddex->append_node(doc.allocate_node(rapidxml::node_element, "gauge", gauge));
		
		xml_root->insert_node(mynode, sddex);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "wheels", "information");
		doc.append_node(node);
		rapidxml::xml_node<>* sddex = doc.allocate_node(rapidxml::node_element, "wheel", NULL);
		node->append_node(sddex);

		sddex->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(sddex_ele.id_, id, 10)));
		sddex->append_node(doc.allocate_node(rapidxml::node_element, "name", sddex_ele.name_));
		sddex->append_node(doc.allocate_node(rapidxml::node_element, "gauge", gauge));

		std::ofstream out(SaveResult);
		out << doc;
		out.close();
	}
	return 0;
}

int __stdcall get_sddex_element(std::vector<sddex>& sddex_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\sddex.xml";

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try{
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (root)
		{
			sddex tmp;
			rapidxml::xml_node<>*anode = root->first_node();
			if (anode != NULL)
			{
				//rapidxml::xml_node<>*cnode = anode->first_node();
				for (rapidxml::xml_node<>*anode = root->first_node(); anode != NULL; anode = anode->next_sibling())
				{
					bool element_exist = true;
					rapidxml::xml_node<>*cnode = anode->first_node();
					if (NULL == cnode){
						element_exist = false;
					}
					while (cnode)
					{
						if (0 == strcmp("id", cnode->name())){
							tmp.id_ = atoi(cnode->value());
						}else if (0 == strcmp("name", cnode->name())){
							strcpy(tmp.name_, cnode->value());
						}else if (0 == strcmp("gauge", cnode->name())){
							tmp.var__sdd_extra_info.gauge_ = atof(cnode->value());
						}
						cnode = cnode->next_sibling();
					}
					if (element_exist){
						sddex_ele.push_back(tmp);
					}
				}
			}
		}
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall add_navigation_element(navigation& navigation_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();

	std::basic_string<char> path = SaveResult + "\\etc\\var";
	SaveResult += "\\etc\\var\\navigation.xml";
	std::fstream _file;
	_file.open(path, std::ios::in);
	if (!_file){
		int res = nsp::os::mkdir<char>(path);
	}
	_file.close();
	char id[10];
	char max_speed[16];
	char creep_speed[16];
	char max_w[16];
	char creep_w[16];
	char slow_down_speed[16];
	char acc[16];
	char dec[16];
	char dec_estop[16];
	char acc_w[16];
	char dec_w[16];
	char creep_distance[16];
	char creep_theta[16];
	char upl_mapping_angle_tolerance[16];
	char upl_mapping_dist_tolerance[16];
	char upl_mapping_angle_weight[16];
	char upl_mapping_dist_weight[16];
	char tracking_error_tolerance_dist[16];
	char tracking_error_tolerance_angle[16];
	char aim_dist[16];
	char predict_time[16];
	char aim_angle_p[16];
	char aim_angle_i[16];
	char aim_angle_d[16];
	char stop_tolerance[16];
	char stop_tolerance_angle[16];
	char stop_point_trim[16];

	char aim_ey_p[16];
	char aim_ey_i[16];
	char aim_ey_d[16];

	sprintf(max_speed, "%f", navigation_ele.var__navigation_info.max_speed_);
	sprintf(creep_speed, "%f", navigation_ele.var__navigation_info.creep_speed_);
	sprintf(max_w, "%f", navigation_ele.var__navigation_info.max_w_);
	sprintf(creep_w, "%f", navigation_ele.var__navigation_info.creep_w_);
	sprintf(slow_down_speed, "%f", navigation_ele.var__navigation_info.slow_down_speed_);
	sprintf(acc, "%f", navigation_ele.var__navigation_info.acc_);
	sprintf(dec, "%f", navigation_ele.var__navigation_info.dec_);
	sprintf(dec_estop, "%f", navigation_ele.var__navigation_info.dec_estop_);
	sprintf(acc_w, "%f", navigation_ele.var__navigation_info.acc_w_);
	sprintf(dec_w, "%f", navigation_ele.var__navigation_info.dec_w_);
	sprintf(creep_distance, "%f", navigation_ele.var__navigation_info.creep_distance_);
	sprintf(creep_theta, "%f", navigation_ele.var__navigation_info.creep_theta_);
	sprintf(upl_mapping_angle_tolerance, "%f", navigation_ele.var__navigation_info.upl_mapping_angle_tolerance_);
	sprintf(upl_mapping_dist_tolerance, "%f", navigation_ele.var__navigation_info.upl_mapping_dist_tolerance_);
	sprintf(upl_mapping_angle_weight, "%f", navigation_ele.var__navigation_info.upl_mapping_angle_weight_);
	sprintf(upl_mapping_dist_weight, "%f", navigation_ele.var__navigation_info.upl_mapping_dist_weight_);
	sprintf(tracking_error_tolerance_dist, "%f", navigation_ele.var__navigation_info.tracking_error_tolerance_dist_);
	sprintf(tracking_error_tolerance_angle, "%f", navigation_ele.var__navigation_info.tracking_error_tolerance_angle_);
	sprintf(aim_dist, "%f", navigation_ele.var__navigation_info.aim_dist_);
	sprintf(predict_time, "%ld", navigation_ele.var__navigation_info.predict_time_);
	sprintf(aim_angle_p, "%f", navigation_ele.var__navigation_info.aim_angle_p_);
	sprintf(aim_angle_i, "%f", navigation_ele.var__navigation_info.aim_angle_i_);
	sprintf(aim_angle_d, "%f", navigation_ele.var__navigation_info.aim_angle_d_);
	sprintf(stop_tolerance, "%f", navigation_ele.var__navigation_info.stop_tolerance_);
	sprintf(stop_tolerance_angle, "%f", navigation_ele.var__navigation_info.stop_tolerance_angle_);
	sprintf(stop_point_trim, "%f", navigation_ele.var__navigation_info.stop_point_trim_);
	sprintf(aim_ey_p, "%f", navigation_ele.var__navigation_info.aim_ey_p_);
	sprintf(aim_ey_i, "%f", navigation_ele.var__navigation_info.aim_ey_i_);
	sprintf(aim_ey_d, "%f", navigation_ele.var__navigation_info.aim_ey_d_);

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::file<> *file = nullptr;

	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* xml_root = doc.first_node();
		rapidxml::xml_node<>* navigationn = xml_root->first_node();
		rapidxml::xml_node<>*node = navigationn;
		while (navigationn != NULL){

			if (navigationn->next_sibling() != NULL){
				navigationn = navigationn->next_sibling();
			}
			xml_root->remove_node(node);
			if (navigationn->next_sibling() == NULL){
				xml_root->remove_node(navigationn);
				break;
			}
			node = navigationn;
		}
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(navigation_ele.id_, id, 10)));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "name", navigation_ele.name_));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "max_speed", max_speed));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "creep_speed", creep_speed));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "max_w", max_w));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "creep_w", creep_w));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "slow_down_speed", slow_down_speed));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "acc", acc));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "dec", dec));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "dec_estop", dec_estop));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "acc_w", acc_w));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "dec_w", dec_w));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "creep_distance", creep_distance));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "creep_theta", creep_theta));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "upl_mapping_angle_tolerance", upl_mapping_angle_tolerance));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "upl_mapping_dist_tolerance", upl_mapping_dist_tolerance));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "upl_mapping_angle_weight", upl_mapping_angle_weight));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "upl_mapping_dist_weight", upl_mapping_dist_weight));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "tracking_error_tolerance_dist", tracking_error_tolerance_dist));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "tracking_error_tolerance_angle", tracking_error_tolerance_angle));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "aim_dist", aim_dist));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "predict_time", predict_time));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "aim_angle_p", aim_angle_p));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "aim_angle_i", aim_angle_i));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "aim_angle_d", aim_angle_d));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "stop_tolerance", stop_tolerance));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "stop_tolerance_angle", stop_tolerance_angle));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "stop_point_trim", stop_point_trim));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "aim_ey_p", aim_ey_p));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "aim_ey_i", aim_ey_i));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "aim_ey_d", aim_ey_d));
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* navigation = doc.allocate_node(rapidxml::node_element, "navigation", NULL);
		doc.append_node(navigation);

		navigation->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(navigation_ele.id_, id, 10)));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "name", navigation_ele.name_));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "max_speed", max_speed));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "creep_speed", creep_speed));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "max_w", max_w));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "creep_w", creep_w));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "slow_down_speed", slow_down_speed));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "acc", acc));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "dec", dec));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "dec_estop", dec_estop));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "acc_w", acc_w));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "dec_w", dec_w));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "creep_distance", creep_distance));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "creep_theta", creep_theta));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "upl_mapping_angle_tolerance", upl_mapping_angle_tolerance));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "upl_mapping_dist_tolerance", upl_mapping_dist_tolerance));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "upl_mapping_angle_weight", upl_mapping_angle_weight));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "upl_mapping_dist_weight", upl_mapping_dist_weight));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "tracking_error_tolerance_dist", tracking_error_tolerance_dist));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "tracking_error_tolerance_angle", tracking_error_tolerance_angle));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "aim_dist", aim_dist));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "predict_time", predict_time));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "aim_angle_p", aim_angle_p));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "aim_angle_i", aim_angle_i));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "aim_angle_d", aim_angle_d));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "stop_tolerance", stop_tolerance));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "stop_tolerance_angle", stop_tolerance_angle));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "stop_point_trim", stop_point_trim));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "aim_ey_p", aim_ey_p));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "aim_ey_i", aim_ey_i));
		navigation->append_node(doc.allocate_node(rapidxml::node_element, "aim_ey_d", aim_ey_d));

		std::ofstream out(SaveResult);
		out << doc;
		out.close();
	}
	return 0;
}

int __stdcall add_vehicle_element(vehicle& vehicle_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();

	std::basic_string<char> path = SaveResult + "\\etc\\var";
	SaveResult += "\\etc\\var\\vehicle.xml";
	std::fstream _file;
	_file.open(path, std::ios::in);
	if (!_file){
		int res = nsp::os::mkdir<char>(path);
	}
	_file.close();
	char id[10];
	char vehicle_type[16];
	char vehicle_id[10];
	char max_speed[16];
	char creep_speed[16];
	char max_acc[16];
	char max_dec[16];
	char max_w[16];
	char creep_w[16];
	char max_acc_w[16];
	char max_dec_w[16];
	char steer_angle_error_tolerance[16];

	sprintf(max_speed, "%f", vehicle_ele.var__vehicle_info.max_speed_);
	sprintf(creep_speed, "%f", vehicle_ele.var__vehicle_info.creep_speed_);
	sprintf(max_acc, "%f", vehicle_ele.var__vehicle_info.max_acc_);
	sprintf(max_dec, "%f", vehicle_ele.var__vehicle_info.max_dec_);
	sprintf(max_w, "%f", vehicle_ele.var__vehicle_info.max_w_);
	sprintf(creep_w, "%f", vehicle_ele.var__vehicle_info.creep_w_);
	sprintf(max_acc_w, "%f", vehicle_ele.var__vehicle_info.max_acc_w_);
	sprintf(max_dec_w, "%f", vehicle_ele.var__vehicle_info.max_dec_w_);
	sprintf(steer_angle_error_tolerance, "%f", vehicle_ele.var__vehicle_info.steer_angle_error_tolerance_);

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::file<> *file = nullptr;

	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* xml_root = doc.first_node();
		rapidxml::xml_node<>* vehiclee = xml_root->first_node();
		rapidxml::xml_node<>*node = vehiclee;
		while(vehiclee != NULL){
			
			if (vehiclee->next_sibling() != NULL){
				vehiclee = vehiclee->next_sibling();
			}
			xml_root->remove_node(node);
			if (vehiclee->next_sibling() == NULL){
				xml_root->remove_node(vehiclee);
				break;
			}
			node = vehiclee;	
		}
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(vehicle_ele.id_, id, 10)));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "name", vehicle_ele.name_));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "vehicle_id", _itoa(vehicle_ele.var__vehicle_info.vehicle_id_, vehicle_id, 10)));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "vehicle_name", vehicle_ele.var__vehicle_info.vehicle_name_));
		_itoa(vehicle_ele.var__vehicle_info.vehicle_type_, vehicle_type, 10);
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "vehicle_type", vehicle_type));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "chassis_type", &vehicle_ele.var__vehicle_info.chassis_type_));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "max_speed", max_speed));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "creep_speed", creep_speed));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "max_acc", max_acc));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "max_dec", max_dec));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "max_w", max_w));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "creep_w", creep_w));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "max_acc_w", max_acc_w));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "max_dec_w", max_dec_w));
		xml_root->append_node(doc.allocate_node(rapidxml::node_element, "steer_angle_error_tolerance", steer_angle_error_tolerance));
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* vehicle = doc.allocate_node(rapidxml::node_element, "vehicle", NULL);
		doc.append_node(vehicle);

		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(vehicle_ele.id_, id, 10)));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "name", vehicle_ele.name_));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "vehicle_type", _itoa(vehicle_ele.var__vehicle_info.vehicle_type_, vehicle_type, 10)));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "vehicle_id", _itoa(vehicle_ele.var__vehicle_info.vehicle_id_, vehicle_id, 10)));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "chassis_type", &vehicle_ele.var__vehicle_info.chassis_type_));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "max_speed", max_speed));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "creep_speed", creep_speed));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "max_acc", max_acc));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "max_dec", max_dec));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "max_w", max_w));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "creep_w", creep_w));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "max_acc_w", max_acc_w));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "max_dec_w", max_dec_w));
		vehicle->append_node(doc.allocate_node(rapidxml::node_element, "steer_angle_error_tolerance", steer_angle_error_tolerance));

		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	return 0;
}

int __stdcall get_navigation_element(std::vector<navigation>& navigation_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\navigation.xml";

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try{
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (root)
		{
			navigation tmp;
			bool element_exist = true;
			rapidxml::xml_node<>*anode = root->first_node();
			if (NULL == anode){
				element_exist = false;
			}
			while (anode){
				if (0 == strcmp("id", anode->name())){
					tmp.id_ = atoi(anode->value());
				}else if (0 == strcmp("name", anode->name())){
					strcpy(tmp.name_, anode->value());
				}else if (0 == strcmp("max_speed", anode->name())){
					tmp.var__navigation_info.max_speed_ = atof(anode->value());
				}else if (0 == strcmp("creep_speed", anode->name())){
					tmp.var__navigation_info.creep_speed_ = atof(anode->value());
				}else if (0 == strcmp("max_w", anode->name())){
					tmp.var__navigation_info.max_w_ = atof(anode->value());
				}else if (0 == strcmp("creep_w", anode->name())){
					tmp.var__navigation_info.creep_w_ = atof(anode->value());
				}else if (0 == strcmp("slow_down_speed", anode->name())){
					tmp.var__navigation_info.slow_down_speed_ = atof(anode->value());
				}else if (0 == strcmp("acc", anode->name())){
					tmp.var__navigation_info.acc_ = atof(anode->value());
				}else if (0 == strcmp("dec", anode->name())){
					tmp.var__navigation_info.dec_ = atof(anode->value());
				}else if (0 == strcmp("dec_estop", anode->name())){
					tmp.var__navigation_info.dec_estop_ = atof(anode->value());
				}else if (0 == strcmp("acc_w", anode->name())){
					tmp.var__navigation_info.acc_w_ = atof(anode->value());
				}else if (0 == strcmp("dec_w", anode->name())){
					tmp.var__navigation_info.dec_w_ = atof(anode->value());
				}else if (0 == strcmp("creep_distance", anode->name())){
					tmp.var__navigation_info.creep_distance_ = atof(anode->value());
				}else if (0 == strcmp("creep_theta", anode->name())){
					tmp.var__navigation_info.creep_theta_ = atof(anode->value());
				}else if (0 == strcmp("upl_mapping_angle_tolerance", anode->name())){
					tmp.var__navigation_info.upl_mapping_angle_tolerance_ = atof(anode->value());
				}else if (0 == strcmp("upl_mapping_dist_tolerance", anode->name())){
					tmp.var__navigation_info.upl_mapping_dist_tolerance_ = atof(anode->value());
				}else if (0 == strcmp("upl_mapping_angle_weight", anode->name())){
					tmp.var__navigation_info.upl_mapping_angle_weight_ = atof(anode->value());
				}else if (0 == strcmp("upl_mapping_dist_weight", anode->name())){
					tmp.var__navigation_info.upl_mapping_dist_weight_ = atof(anode->value());
				}else if (0 == strcmp("tracking_error_tolerance_dist", anode->name())){
					tmp.var__navigation_info.tracking_error_tolerance_dist_ = atof(anode->value());
				}else if (0 == strcmp("tracking_error_tolerance_angle", anode->name())){
					tmp.var__navigation_info.tracking_error_tolerance_angle_ = atof(anode->value());
				}else if (0 == strcmp("aim_dist", anode->name())){
					tmp.var__navigation_info.aim_dist_ = atof(anode->value());
				}else if (0 == strcmp("predict_time", anode->name())){
					sscanf_s(anode->value(), "%lld", &tmp.var__navigation_info.predict_time_);
				}else if (0 == strcmp("aim_angle_p", anode->name())){
					tmp.var__navigation_info.aim_angle_p_ = atof(anode->value());
				}else if (0 == strcmp("aim_angle_i", anode->name())){
					tmp.var__navigation_info.aim_angle_i_ = atof(anode->value());
				}else if (0 == strcmp("aim_angle_d", anode->name())){
					tmp.var__navigation_info.aim_angle_d_ = atof(anode->value());
				}else if (0 == strcmp("stop_tolerance", anode->name())){
					tmp.var__navigation_info.stop_tolerance_ = atof(anode->value());
				}else if (0 == strcmp("stop_tolerance_angle", anode->name())){
					tmp.var__navigation_info.stop_tolerance_angle_ = atof(anode->value());
				}else if (0 == strcmp("stop_point_trim", anode->name())){
					tmp.var__navigation_info.stop_point_trim_ = atof(anode->value());
				}else if (0 == strcmp("aim_ey_p", anode->name())){
					tmp.var__navigation_info.aim_ey_p_ = atof(anode->value());
				}else if (0 == strcmp("aim_ey_i", anode->name())){
					tmp.var__navigation_info.aim_ey_i_ = atof(anode->value());
				}else if (0 == strcmp("aim_ey_d", anode->name())){
					tmp.var__navigation_info.aim_ey_d_ = atof(anode->value());
				}
				anode = anode->next_sibling();
			}
			if (element_exist){
				navigation_ele.push_back(tmp);
			}
		}
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall get_vehicle_element(std::vector<vehicle>& vehicle_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\vehicle.xml";

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try{
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (root)
		{
			vehicle tmp;
			bool element_exist = true;
			rapidxml::xml_node<>*anode = root->first_node();
			if (NULL == anode){
				element_exist = false;
			}
			while (anode){
				if (0 == strcmp("id", anode->name())){
					tmp.id_ = atoi(anode->value());
				}
				else if (0 == strcmp("name", anode->name())){
					strcpy(tmp.name_, anode->value());
				}
				else if (0 == strcmp("vehicle_id", anode->name())){
					tmp.var__vehicle_info.vehicle_id_ = atoi(anode->value());
				}
				else if (0 == strcmp("vehicle_name", anode->name())){
					strcpy(tmp.var__vehicle_info.vehicle_name_, anode->value());
				}
				else if (0 == strcmp("vehicle_type", anode->name())){
					tmp.var__vehicle_info.vehicle_type_ = (vehicle_type_t)atoi(anode->value());
				}
				else if (0 == strcmp("chassis_type", anode->name())){
					strcpy(&tmp.var__vehicle_info.chassis_type_, anode->value());
				}
				else if (0 == strcmp("max_speed", anode->name())){
					tmp.var__vehicle_info.max_speed_ = atof(anode->value());
				}
				else if (0 == strcmp("creep_speed", anode->name())){
					tmp.var__vehicle_info.creep_speed_ = atof(anode->value());
				}
				else if (0 == strcmp("max_acc", anode->name())){
					tmp.var__vehicle_info.max_acc_ = atof(anode->value());
				}
				else if (0 == strcmp("max_dec", anode->name())){
					tmp.var__vehicle_info.max_dec_ = atof(anode->value());
				}
				else if (0 == strcmp("max_w", anode->name())){
					tmp.var__vehicle_info.max_w_ = atof(anode->value());
				}
				else if (0 == strcmp("creep_w", anode->name())){
					tmp.var__vehicle_info.creep_w_ = atof(anode->value());
				}
				else if (0 == strcmp("max_acc_w", anode->name())){
					tmp.var__vehicle_info.max_acc_w_ = atof(anode->value());
				}
				else if (0 == strcmp("max_dec_w", anode->name())){
					tmp.var__vehicle_info.max_dec_w_ = atof(anode->value());
				}
				else if (0 == strcmp("steer_angle_error_tolerance", anode->name())){
					tmp.var__vehicle_info.steer_angle_error_tolerance_ = atof(anode->value());
				}
				anode = anode->next_sibling();
			}
			if (element_exist){
				vehicle_ele.push_back(tmp);
			}
		}
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall add_mnt_element(mnt& mnt_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\mnt.xml";

	char from[10];
	char foff[10];
	char to[10];
	char toff[10];
	char len[10];

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* xml_root = doc.first_node();
		rapidxml::xml_node<>*binding = doc.allocate_node(rapidxml::node_element, "binding");
		xml_root->append_node(binding);
		binding->append_attribute(doc.allocate_attribute("from", _itoa(mnt_ele.from, from, 10)));
		binding->append_attribute(doc.allocate_attribute("foff", _itoa(mnt_ele.foff, foff, 10)));
		binding->append_attribute(doc.allocate_attribute("to", _itoa(mnt_ele.to, to, 10)));
		binding->append_attribute(doc.allocate_attribute("toff", _itoa(mnt_ele.toff, toff, 10)));
		binding->append_attribute(doc.allocate_attribute("len", _itoa(mnt_ele.len, len, 10)));

		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* mnt = doc.allocate_node(rapidxml::node_element, "mnt", NULL);
		doc.append_node(mnt);
		rapidxml::xml_node<>*binding = doc.allocate_node(rapidxml::node_element, "binding");
		mnt->append_node(binding);
		binding->append_attribute(doc.allocate_attribute("from", _itoa(mnt_ele.from, from, 10)));
		binding->append_attribute(doc.allocate_attribute("foff", _itoa(mnt_ele.foff, foff, 10)));
		binding->append_attribute(doc.allocate_attribute("to", _itoa(mnt_ele.to, to, 10)));
		binding->append_attribute(doc.allocate_attribute("toff", _itoa(mnt_ele.toff, toff, 10)));
		binding->append_attribute(doc.allocate_attribute("len", _itoa(mnt_ele.len, len, 10)));
	
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
	}
	return 0;
}

int __stdcall get_mnt_element(std::vector<mnt>& mnt_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\mnt.xml";

	if (!SaveResult.c_str()){
		return -1;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try{
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (root)
		{
			mnt tmp;
			for (rapidxml::xml_node<>*anode = root->first_node(); anode != NULL; anode = anode->next_sibling())
			{
				if (0 == strcmp("binding", anode->name())){
					rapidxml::xml_attribute<char> * attr = anode->first_attribute();
					while (attr)
					{
						if (0 == strcmp("from", attr->name())){
							tmp.from = atoi(attr->value());
						}
						else if (0 == strcmp("foff", attr->name())){
							tmp.foff = atoi(attr->value());
						}
						else if (0 == strcmp("to", attr->name())){
							tmp.to = atoi(attr->value());
						}
						else if (0 == strcmp("toff", attr->name())){
							tmp.toff = atoi(attr->value());
						}
						else if (0 == strcmp("len", attr->name())){
							tmp.len = atoi(attr->value());
						}
						attr = attr->next_attribute();
					}
					mnt_ele.push_back(tmp);
				}
			}
		}
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall add_moos_element(moos& moos_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\moos.xml";

	char id[10];
	char can[10];
	char port[10];
	char elmonode[10];
	char latency[32];
	char merge[10];
	char profile_speed[16];
	char profile_acc[16];
	char profile_dec[16];

	sprintf(latency, "%lld", moos_ele.var__moos_info.candev_head_.latency_);
	sprintf(profile_speed, "%f", moos_ele.var__moos_info.profile_speed_);
	sprintf(profile_acc, "%f", moos_ele.var__moos_info.profile_acc_);
	sprintf(profile_dec, "%f", moos_ele.var__moos_info.profile_dec_);

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* xml_root = doc.first_node();
		rapidxml::xml_node<>* mynode = xml_root->first_node();
		rapidxml::xml_node<>* moos = doc.allocate_node(rapidxml::node_element, "device", NULL);

		moos->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(moos_ele.id_, id, 10)));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "name", moos_ele.name_));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "can", _itoa(moos_ele.var__moos_info.candev_head_.canbus_, can, 10)));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "port", _itoa(moos_ele.var__moos_info.candev_head_.canport_, port, 10)));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "node", _itoa(moos_ele.var__moos_info.candev_head_.cannode_, elmonode, 10)));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "latency", latency));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "merge", _itoa(moos_ele.var__moos_info.candev_head_.merge_, merge, 10)));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "profile_speed", profile_speed));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "profile_acc", profile_acc));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "profile_dec", profile_dec));

		xml_root->insert_node(mynode, moos);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "moos", "information");
		doc.append_node(node);
		rapidxml::xml_node<>* moos = doc.allocate_node(rapidxml::node_element, "device", NULL);
		node->append_node(moos);
		moos->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(moos_ele.id_, id, 10)));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "name", moos_ele.name_));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "can", _itoa(moos_ele.var__moos_info.candev_head_.canbus_, can, 10)));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "port", _itoa(moos_ele.var__moos_info.candev_head_.canport_, port, 10)));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "node", _itoa(moos_ele.var__moos_info.candev_head_.cannode_, elmonode, 10)));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "latency", latency));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "merge", _itoa(moos_ele.var__moos_info.candev_head_.merge_, merge, 10)));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "profile_speed", profile_speed));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "profile_acc", profile_acc));
		moos->append_node(doc.allocate_node(rapidxml::node_element, "profile_dec", profile_dec));

		std::ofstream out(SaveResult);
		out << doc;
		out.close();
	}
	return 0;
}

int __stdcall get_moos_element(std::vector<moos>& moos_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\moos.xml";

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try{
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (root)
		{
			moos tmp;
			rapidxml::xml_node<>*anode = root->first_node();
			if (anode != NULL)
			{
				rapidxml::xml_node<>*cnode = anode->first_node();
				for (anode = root->first_node(); anode != NULL; anode = anode->next_sibling())
				{
					bool element_exist = true;
					cnode = anode->first_node();
					if (cnode == NULL){
						element_exist = false;
					}
					while (cnode)
					{
						if (0 == strcmp("id", cnode->name())){
							tmp.id_ = atoi(cnode->value());
						}
						else if (0 == strcmp("name", cnode->name())){
							strcpy(tmp.name_, cnode->value());
						}
						else if (0 == strcmp("can", cnode->name())){
							tmp.var__moos_info.candev_head_.canbus_ = atoi(cnode->value());
						}
						else if (0 == strcmp("port", cnode->name())){
							tmp.var__moos_info.candev_head_.canport_ = atoi(cnode->value());
						}
						else if (0 == strcmp("node", cnode->name())){
							tmp.var__moos_info.candev_head_.cannode_ = atoi(cnode->value());
						}
						else if (0 == strcmp("latency", cnode->name())){
							sscanf_s(cnode->value(), "%lld", &tmp.var__moos_info.candev_head_.latency_);
						}
						else if (0 == strcmp("merge", cnode->name())){
							tmp.var__moos_info.candev_head_.merge_ = atoi(cnode->value());
						}
						else if (0 == strcmp("profile_speed", cnode->name())){
							tmp.var__moos_info.profile_speed_ = atof(cnode->value());
						}
						else if (0 == strcmp("profile_acc", cnode->name())){
							tmp.var__moos_info.profile_acc_ = atof(cnode->value());
						}
						else if (0 == strcmp("profile_dec", cnode->name())){
							tmp.var__moos_info.profile_dec_ = atof(cnode->value());
						}
						cnode = cnode->next_sibling();
					}
					if (element_exist){
						moos_ele.push_back(tmp);
					}
				}
			}
			
		}
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall add_angle_encoder_element(angle_encoder& angle_encoder_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\angle_encoder.xml";

	char id[10];
	char can[10];
	char port[10];
	char elmonode[10];
	char latency[32];
	char merge[10];
	char encoder_type[10];

	sprintf(latency, "%lld", angle_encoder_ele.var__angle_encoder_info.candev_head_.latency_);

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* xml_root = doc.first_node();
		rapidxml::xml_node<>* mynode = xml_root->first_node();
		rapidxml::xml_node<>* angleencoder = doc.allocate_node(rapidxml::node_element, "device", NULL);

		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(angle_encoder_ele.id_, id, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "name", angle_encoder_ele.name_));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "can", _itoa(angle_encoder_ele.var__angle_encoder_info.candev_head_.canbus_, can, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "port", _itoa(angle_encoder_ele.var__angle_encoder_info.candev_head_.canport_, port, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "node", _itoa(angle_encoder_ele.var__angle_encoder_info.candev_head_.cannode_, elmonode, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "latency", latency));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "merge", _itoa(angle_encoder_ele.var__angle_encoder_info.candev_head_.merge_, merge, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "encoder_type", _itoa(angle_encoder_ele.var__angle_encoder_info.encoder_type_, encoder_type, 10)));

		xml_root->insert_node(mynode, angleencoder);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "ae", "information");
		doc.append_node(node);
		rapidxml::xml_node<>* angleencoder = doc.allocate_node(rapidxml::node_element, "device", NULL);
		node->append_node(angleencoder);
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(angle_encoder_ele.id_, id, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "name", angle_encoder_ele.name_));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "can", _itoa(angle_encoder_ele.var__angle_encoder_info.candev_head_.canbus_, can, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "port", _itoa(angle_encoder_ele.var__angle_encoder_info.candev_head_.canport_, port, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "node", _itoa(angle_encoder_ele.var__angle_encoder_info.candev_head_.cannode_, elmonode, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "latency", latency));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "merge", _itoa(angle_encoder_ele.var__angle_encoder_info.candev_head_.merge_, merge, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "encoder_type", _itoa(angle_encoder_ele.var__angle_encoder_info.encoder_type_, encoder_type, 10)));

		std::ofstream out(SaveResult);
		out << doc;
		out.close();
	}
	return 0;
}

int __stdcall get_angle_encoder_element(std::vector<angle_encoder>& angle_encoder_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\angle_encoder.xml";

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try{
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (root)
		{
			angle_encoder tmp;
			rapidxml::xml_node<>*anode = root->first_node();
			if (anode != NULL)
			{
				rapidxml::xml_node<>*cnode = anode->first_node();
				for (anode = root->first_node(); anode != NULL; anode = anode->next_sibling())
				{
					bool element_exist = true;
					cnode = anode->first_node();
					if (cnode == NULL){
						element_exist = false;
					}
					while (cnode)
					{
						if (0 == strcmp("id", cnode->name())){
							tmp.id_ = atoi(cnode->value());
						}
						else if (0 == strcmp("name", cnode->name())){
							strcpy(tmp.name_, cnode->value());
						}
						else if (0 == strcmp("can", cnode->name())){
							tmp.var__angle_encoder_info.candev_head_.canbus_ = atoi(cnode->value());
						}
						else if (0 == strcmp("port", cnode->name())){
							tmp.var__angle_encoder_info.candev_head_.canport_ = atoi(cnode->value());
						}
						else if (0 == strcmp("node", cnode->name())){
							tmp.var__angle_encoder_info.candev_head_.cannode_ = atoi(cnode->value());
						}
						else if (0 == strcmp("latency", cnode->name())){
							sscanf_s(cnode->value(), "%lld", &tmp.var__angle_encoder_info.candev_head_.latency_);
						}
						else if (0 == strcmp("merge", cnode->name())){
							tmp.var__angle_encoder_info.candev_head_.merge_ = atoi(cnode->value());
						}
						else if (0 == strcmp("encoder_type", cnode->name())){
							tmp.var__angle_encoder_info.encoder_type_ = (angle_encoder_type_t)atoi(cnode->value());
						}	
						cnode = cnode->next_sibling();
					}
					if (element_exist){
						angle_encoder_ele.push_back(tmp);
					}
				}
			}
		}
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall add_dio_element(dio&var__dio_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\dio.xml";

	char id[10];
	char can[10];
	char port[10];
	char elmonode[10];
	char latency[32];
	char merge[10];
	char di[10];
	char doo[10];

	sprintf(latency, "%lld", var__dio_ele.var__dio_info.candev_head_.latency_);

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* xml_root = doc.first_node();
		rapidxml::xml_node<>* mynode = xml_root->first_node();
		rapidxml::xml_node<>* dionode = doc.allocate_node(rapidxml::node_element, "device", NULL);

		dionode->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(var__dio_ele.id_, id, 10)));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "name", var__dio_ele.name_));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "can", _itoa(var__dio_ele.var__dio_info.candev_head_.canbus_, can, 10)));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "port", _itoa(var__dio_ele.var__dio_info.candev_head_.canport_, port, 10)));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "node", _itoa(var__dio_ele.var__dio_info.candev_head_.cannode_, elmonode, 10)));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "latency", latency));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "merge", _itoa(var__dio_ele.var__dio_info.candev_head_.merge_, merge, 10)));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "di_channel_num", _itoa(var__dio_ele.var__dio_info.di_channel_num_, di, 10)));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "do_channel_num", _itoa(var__dio_ele.var__dio_info.di_channel_num_, doo, 10)));

		rapidxml::xml_node<>*ais = doc.allocate_node(rapidxml::node_element, "ais", NULL);
		dionode->append_node(ais);
		char sta_tmp[10][16];
		char eff_tmp[10][16];
		char inte_tmp[10][16];
		std::string str_tmp[10];
		for (unsigned int i = 0; i < var__dio_ele.ais_block_num_; i++){
			rapidxml::xml_node<>*ais_block = doc.allocate_node(rapidxml::node_element, "block");
			ais->append_node(ais_block);
			str_tmp[i] = "0x";
			str_tmp[i] += _itoa(var__dio_ele.var__dio_info.i.ai_[i].start_address_, sta_tmp[i], 16);
			ais_block->append_node(doc.allocate_node(rapidxml::node_element, "start_address", str_tmp[i].c_str()));
			ais_block->append_node(doc.allocate_node(rapidxml::node_element, "effective", _itoa(var__dio_ele.var__dio_info.i.ai_[i].effective_count_of_index_, eff_tmp[i], 10)));
			ais_block->append_node(doc.allocate_node(rapidxml::node_element, "internel_type", _itoa(var__dio_ele.var__dio_info.i.ai_[i].internel_type_, inte_tmp[i], 10)));
		}
		rapidxml::xml_node<>*aos = doc.allocate_node(rapidxml::node_element, "aos", NULL);
		dionode->append_node(aos);
		char sta_ao[10][16];
		char eff_ao[10][16];
		char inte_ao[10][16];
		std::string str_ao[10];
		for (unsigned int i = 0; i < var__dio_ele.aos_block_num_; i++){
			rapidxml::xml_node<>*aos_block = doc.allocate_node(rapidxml::node_element, "block");
			aos->append_node(aos_block);
			str_tmp[i] = "0x";
			str_tmp[i] += _itoa(var__dio_ele.var__dio_info.ao_[i].start_address_, sta_ao[i], 16);
			aos_block->append_node(doc.allocate_node(rapidxml::node_element, "start_address", str_tmp[i].c_str()));
			aos_block->append_node(doc.allocate_node(rapidxml::node_element, "effective", _itoa(var__dio_ele.var__dio_info.ao_[i].effective_count_of_index_, eff_ao[i], 10)));
			aos_block->append_node(doc.allocate_node(rapidxml::node_element, "internel_type", _itoa(var__dio_ele.var__dio_info.ao_[i].internel_type_, inte_ao[i], 10)));
		}

		xml_root->insert_node(mynode, dionode);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "dio", "information");
		doc.append_node(node);
		rapidxml::xml_node<>* dionode = doc.allocate_node(rapidxml::node_element, "device", NULL);
		node->append_node(dionode);
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(var__dio_ele.id_, id, 10)));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "name", var__dio_ele.name_));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "can", _itoa(var__dio_ele.var__dio_info.candev_head_.canbus_, can, 10)));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "port", _itoa(var__dio_ele.var__dio_info.candev_head_.canport_, port, 10)));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "node", _itoa(var__dio_ele.var__dio_info.candev_head_.cannode_, elmonode, 10)));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "latency", latency));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "merge", _itoa(var__dio_ele.var__dio_info.candev_head_.merge_, merge, 10)));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "di_channel_num", _itoa(var__dio_ele.var__dio_info.di_channel_num_, di, 10)));
		dionode->append_node(doc.allocate_node(rapidxml::node_element, "do_channel_num", _itoa(var__dio_ele.var__dio_info.di_channel_num_, doo, 10)));

		rapidxml::xml_node<>*ais = doc.allocate_node(rapidxml::node_element, "ais", NULL);
		dionode->append_node(ais);
		char sta_tmp[10][16];
		char eff_tmp[10][16];
		char inte_tmp[10][16];
		std::string str_tmp[10];
		for (unsigned int i = 0; i < var__dio_ele.ais_block_num_; i++){
			rapidxml::xml_node<>*ais_block = doc.allocate_node(rapidxml::node_element, "block");
			ais->append_node(ais_block);
			str_tmp[i] = "0x";
			str_tmp[i] += _itoa(var__dio_ele.var__dio_info.i.ai_[i].start_address_, sta_tmp[i], 16);
			ais_block->append_node(doc.allocate_node(rapidxml::node_element, "start_address", str_tmp[i].c_str()));
			ais_block->append_node(doc.allocate_node(rapidxml::node_element, "effective", _itoa(var__dio_ele.var__dio_info.i.ai_[i].effective_count_of_index_, eff_tmp[i], 10)));
			ais_block->append_node(doc.allocate_node(rapidxml::node_element, "internel_type", _itoa(var__dio_ele.var__dio_info.i.ai_[i].internel_type_, inte_tmp[i], 10)));
		}
		rapidxml::xml_node<>*aos = doc.allocate_node(rapidxml::node_element, "aos", NULL);
		dionode->append_node(aos);
		char sta_ao[10][16];
		char eff_ao[10][16];
		char inte_ao[10][16];
		std::string str_ao[10];
		for (unsigned int i = 0; i < var__dio_ele.aos_block_num_; i++){
			rapidxml::xml_node<>*aos_block = doc.allocate_node(rapidxml::node_element, "block");
			aos->append_node(aos_block);
			str_tmp[i] = "0x";
			str_tmp[i] += _itoa(var__dio_ele.var__dio_info.ao_[i].start_address_, sta_ao[i], 16);
			aos_block->append_node(doc.allocate_node(rapidxml::node_element, "start_address", str_tmp[i].c_str()));
			aos_block->append_node(doc.allocate_node(rapidxml::node_element, "effective", _itoa(var__dio_ele.var__dio_info.ao_[i].effective_count_of_index_, eff_ao[i], 10)));
			aos_block->append_node(doc.allocate_node(rapidxml::node_element, "internel_type", _itoa(var__dio_ele.var__dio_info.ao_[i].internel_type_, inte_ao[i], 10)));
		}
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
	}
	return 0;
}

int __stdcall get_dio_element(std::vector<dio>&dio_element)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\dio.xml";

	if (!SaveResult.c_str())
	{
		return -1;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try{
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (root)
		{
			/*dio tmp;*/
			rapidxml::xml_node<>*anode = root->first_node();
			if (anode != NULL)
			{
				for (anode = root->first_node(); anode != NULL; anode = anode->next_sibling())
				{
					bool element_exist = true;
					rapidxml::xml_node<>* cnode = anode->first_node();
					if (NULL == cnode){
						element_exist = false;
					}
					dio *tmp = new dio;
					while (cnode)
					{
						if (0 == strcmp("id", cnode->name())){
							tmp->id_ = atoi(cnode->value());
						}
						else if (0 == strcmp("name", cnode->name())){
							strcpy(tmp->name_, cnode->value());
						}
						else if (0 == strcmp("can", cnode->name())){
							tmp->var__dio_info.candev_head_.canbus_ = atoi(cnode->value());
						}
						else if (0 == strcmp("port", cnode->name())){
							tmp->var__dio_info.candev_head_.canport_ = atoi(cnode->value());
						}
						else if (0 == strcmp("node", cnode->name())){
							tmp->var__dio_info.candev_head_.cannode_ = atoi(cnode->value());
						}
						else if (0 == strcmp("latency", cnode->name())){
							sscanf_s(cnode->value(), "%lld", &tmp->var__dio_info.candev_head_.latency_);
						}
						else if (0 == strcmp("merge", cnode->name())){
							tmp->var__dio_info.candev_head_.merge_ = atoi(cnode->value());
						}
						else if (0 == strcmp("di_channel_num", cnode->name())){
							tmp->var__dio_info.di_channel_num_ = atoi(cnode->value());
						}
						else if (0 == strcmp("do_channel_num", cnode->name())){
							tmp->var__dio_info.do_channel_num_ = atoi(cnode->value());
						}
						else if (0 == strcmp("ais", cnode->name())){
							rapidxml::xml_node<>*dnode = cnode->first_node();
							int i = 0;
							while (dnode){
								if (0 == strcmp("block", dnode->name())){
									rapidxml::xml_node<char> *enode = dnode->first_node();
									while (enode){
										if (0 == strcmp("start_address", enode->name())){
											tmp->var__dio_info.i.ai_[i].start_address_ = std::strtoul(enode->value(), 0, 16);
										}
										else if (0 == strcmp("effective", enode->name())){
											tmp->var__dio_info.i.ai_[i].effective_count_of_index_ = atoi(enode->value());
										}
										else if (0 == strcmp("internel_type", enode->name())){
											tmp->var__dio_info.i.ai_[i].internel_type_ = atoi(enode->value());
										}
										enode = enode->next_sibling();
									}
									i++;
									tmp->ais_block_num_ = i;
								}
								else {
									;
								}
								dnode = dnode->next_sibling();
							}
						}
						else if (0 == strcmp("aos", cnode->name())){
							rapidxml::xml_node<>*dnode = cnode->first_node();
							int i = 0;
							while (dnode){
								if (0 == strcmp("block", dnode->name())){
									rapidxml::xml_node<char> *enode = dnode->first_node();
									while (enode){
										if (0 == strcmp("start_address", enode->name())){
											tmp->var__dio_info.ao_[i].start_address_ = std::strtoul(enode->value(), 0, 16);
										}
										else if (0 == strcmp("effective", enode->name())){
											tmp->var__dio_info.ao_[i].effective_count_of_index_ = atoi(enode->value());
										}
										else if (0 == strcmp("internel_type", enode->name())){
											tmp->var__dio_info.ao_[i].internel_type_ = atoi(enode->value());
										}
										enode = enode->next_sibling();
									}
									i++;
									tmp->aos_block_num_ = i;
								}
								else {
									;
								}
								dnode = dnode->next_sibling();
							}
						}
						cnode = cnode->next_sibling();
					}
					if (element_exist){
						dio_element.push_back(*tmp);
					}
					delete tmp;
				}
			}

		}
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}