#define _CRT_SECURE_NO_WARNINGS
#include "xml_module.h"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include <stdio.h>
#include "os_util.hpp"

int __stdcall update_driveunit_element(driveunit& driveunit_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\driveunit.xml";

	char id[10];
	char type[10];
	char install_x[16];
	char install_y[16];
	char install_theta[16];
	char calibrated_x[16];
	char calibrated_y[16];
	char calibrated_theta[16];
	//char wheel[10];
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
		rapidxml::xml_node<>* root = doc.first_node();

		if (root)
		{
			rapidxml::xml_node<>*anode = root->first_node();
			rapidxml::xml_node<>*bnode = anode->first_node();
			rapidxml::xml_node<>*delnode = anode;
			while (anode != NULL)
			{
				bnode = anode->first_node();
				delnode = anode;

				if (bnode != NULL){
					if (driveunit_ele.id_ == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL) break;
					if (anode->next_sibling() != NULL) anode = anode->next_sibling();
				}
			}
		}
		root = doc.first_node();
		rapidxml::xml_node<>* mynode = root->first_node();
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
			ele->append_node(doc.allocate_node(rapidxml::node_element, "wheel", _itoa(driveunit_ele.wheel_[i], tmp[i], 10)));
		}
		root->insert_node(mynode, driveunit);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall update_canbus_element(can_bus& canbus_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\canbus.xml";

	char id[10];
	char canid[10];
	char type[10];
	char baud[16];

	if (!SaveResult.c_str()){
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();

		if (root)
		{
			rapidxml::xml_node<>*anode = root->first_node();
			rapidxml::xml_node<>*bnode = anode->first_node();
			rapidxml::xml_node<>*delnode = anode;
			while (anode != NULL)
			{
				bnode = anode->first_node();
				delnode = anode;
				

				if (bnode != NULL){
					if (canbus_ele.id_ == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL && canbus_ele.id_ != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) anode = anode->next_sibling();
				}
			}
		}
		root = doc.first_node();
		rapidxml::xml_node<>* mynode = root->first_node();
		rapidxml::xml_node<>* canbus = doc.allocate_node(rapidxml::node_element, "bus", NULL);
		canbus->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(canbus_ele.id_, id, 10)));
		canbus->append_node(doc.allocate_node(rapidxml::node_element, "name", canbus_ele.name_));
		canbus->append_node(doc.allocate_node(rapidxml::node_element, "canid", _itoa(canbus_ele.canbus_id_, canid, 10)));
		canbus->append_node(doc.allocate_node(rapidxml::node_element, "type", _itoa(canbus_ele.canbus_type_, type, 10)));
		canbus->append_node(doc.allocate_node(rapidxml::node_element, "baud", _itoa(canbus_ele.baud_rate_, baud, 10)));
		root->insert_node(mynode, canbus);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;

}

int __stdcall update_elmo_element(elmo& elmo_ele)
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

	if (!SaveResult.c_str()){
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();

		if (root)
		{
			rapidxml::xml_node<>*anode = root->first_node();
			rapidxml::xml_node<>*bnode = anode->first_node();
			rapidxml::xml_node<>*delnode = anode;
			while (anode != NULL)
			{
				bnode = anode->first_node();
				delnode = anode;


				if (bnode != NULL){
					if (elmo_ele.id_ == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL && elmo_ele.id_ != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) anode = anode->next_sibling();
				}
			}
		}
		root = doc.first_node();
		rapidxml::xml_node<>* mynode = root->first_node();
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

		root->insert_node(mynode, elmo);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall update_dwheel_element(dwheel& dwheel_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\dwheel.xml";

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

	if (!SaveResult.c_str()){
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();

		if (root)
		{
			rapidxml::xml_node<>*anode = root->first_node();
			rapidxml::xml_node<>*bnode = anode->first_node();
			rapidxml::xml_node<>*delnode = anode;
			while (anode != NULL)
			{
				bnode = anode->first_node();
				delnode = anode;


				if (bnode != NULL){
					if (dwheel_ele.id_ == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL && dwheel_ele.id_ != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) anode = anode->next_sibling();
				}
			}
		}
		root = doc.first_node();
		rapidxml::xml_node<>* mynode = root->first_node();
		rapidxml::xml_node<>* dwheel = doc.allocate_node(rapidxml::node_element, "wheel", NULL);
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
		root->insert_node(mynode, dwheel);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall update_swheel_element(swheel& swheel_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\swheel.xml";

	char id[10];
	char min_angle[16];
	char max_angle[16];
	char max_w[16];
	char enable[10];
	char control_mode[10];
	char scale_control[16];
	char scale_feedback[16];
	char control_cp[16];
	char control_ci[16];
	char control_cd[16];
	char zero_angle[16];
	char zero_angle_enc[16];

	sprintf(min_angle, "%f", swheel_ele.var__swheel_info.min_angle_);
	sprintf(max_angle, "%f", swheel_ele.var__swheel_info.max_angle_);
	sprintf(max_w, "%f", swheel_ele.var__swheel_info.max_w_);
	sprintf(scale_control, "%f", swheel_ele.var__swheel_info.scale_control_);
	sprintf(scale_feedback, "%f", swheel_ele.var__swheel_info.scale_feedback_);
	sprintf(control_cp, "%f", swheel_ele.var__swheel_info.control_cp_);
	sprintf(control_ci, "%f", swheel_ele.var__swheel_info.control_ci_);
	sprintf(control_cd, "%f", swheel_ele.var__swheel_info.control_cd_);
	sprintf(zero_angle, "%f", swheel_ele.var__swheel_info.zero_angle_);
	sprintf(zero_angle_enc, "%f", swheel_ele.var__swheel_info.zero_angle_enc_);

	if (!SaveResult.c_str()){
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();

		if (root)
		{
			rapidxml::xml_node<>*anode = root->first_node();
			rapidxml::xml_node<>*bnode = anode->first_node();
			rapidxml::xml_node<>*delnode = anode;
			while (anode != NULL)
			{
				bnode = anode->first_node();
				delnode = anode;


				if (bnode != NULL){
					if (swheel_ele.id_ == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL && swheel_ele.id_ != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) anode = anode->next_sibling();
				}
			}
		}
		root = doc.first_node();
		rapidxml::xml_node<>* mynode = root->first_node();
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

		root->insert_node(mynode, swheel);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall update_sddex_element(sddex& sddex_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\sddex.xml";

	char id[10];
	char gauge[16];
	sprintf(gauge, "%f", sddex_ele.var__sdd_extra_info.gauge_);

	if (!SaveResult.c_str()){
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();

		if (root)
		{
			rapidxml::xml_node<>*anode = root->first_node();
			rapidxml::xml_node<>*bnode = anode->first_node();
			rapidxml::xml_node<>*delnode = anode;
			while (anode != NULL)
			{
				bnode = anode->first_node();
				delnode = anode;


				if (bnode != NULL){
					if (sddex_ele.id_ == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL && sddex_ele.id_ != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) anode = anode->next_sibling();
				}
			}
		}
		root = doc.first_node();
		rapidxml::xml_node<>* mynode = root->first_node();
		rapidxml::xml_node<>* sddex = doc.allocate_node(rapidxml::node_element, "wheel", NULL);

		sddex->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(sddex_ele.id_, id, 10)));
		sddex->append_node(doc.allocate_node(rapidxml::node_element, "name", sddex_ele.name_));
		sddex->append_node(doc.allocate_node(rapidxml::node_element, "gauge", gauge));

		root->insert_node(mynode, sddex);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall update_moos_element(moos& moos_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\moos.xml";

	char id[10];
	char can[10];
	char port[10];
	char node[10];
	char latency[32];
	char merge[10];
	char profile_speed[16];
	char profile_acc[16];
	char profile_dec[16];
	sprintf(latency, "%lld", moos_ele.var__moos_info.candev_head_.latency_);
	sprintf(profile_speed, "%f", moos_ele.var__moos_info.profile_speed_);
	sprintf(profile_acc, "%f", moos_ele.var__moos_info.profile_acc_);
	sprintf(profile_dec, "%f", moos_ele.var__moos_info.profile_dec_);

	if (!SaveResult.c_str()){
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();

		if (root)
		{
			rapidxml::xml_node<>*anode = root->first_node();
			rapidxml::xml_node<>*bnode = anode->first_node();
			rapidxml::xml_node<>*delnode = anode;
			while (anode != NULL)
			{
				bnode = anode->first_node();
				delnode = anode;


				if (bnode != NULL){
					if (moos_ele.id_ == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL && moos_ele.id_ != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) anode = anode->next_sibling();
				}
			}
		}
		root = doc.first_node();
		rapidxml::xml_node<>* mynode = root->first_node();
		rapidxml::xml_node<>* mooss = doc.allocate_node(rapidxml::node_element, "device", NULL);
		mooss->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(moos_ele.id_, id, 10)));
		mooss->append_node(doc.allocate_node(rapidxml::node_element, "name", moos_ele.name_));
		mooss->append_node(doc.allocate_node(rapidxml::node_element, "can", _itoa(moos_ele.var__moos_info.candev_head_.canbus_, can, 10)));
		mooss->append_node(doc.allocate_node(rapidxml::node_element, "port", _itoa(moos_ele.var__moos_info.candev_head_.canport_, port, 10)));
		mooss->append_node(doc.allocate_node(rapidxml::node_element, "node", _itoa(moos_ele.var__moos_info.candev_head_.cannode_, node, 10)));
		mooss->append_node(doc.allocate_node(rapidxml::node_element, "latency", latency));
		mooss->append_node(doc.allocate_node(rapidxml::node_element, "merge", _itoa(moos_ele.var__moos_info.candev_head_.merge_, merge, 10)));
		mooss->append_node(doc.allocate_node(rapidxml::node_element, "profile_speed", profile_speed));
		mooss->append_node(doc.allocate_node(rapidxml::node_element, "profile_acc", profile_acc));
		mooss->append_node(doc.allocate_node(rapidxml::node_element, "profile_dec", profile_dec));

		root->insert_node(mynode, mooss);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall update_angle_encoder_element(angle_encoder& angle_encoder_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\angle_encoder.xml";

	char id[10];
	char can[10];
	char port[10];
	char node[10];
	char latency[32];
	char merge[10];
	char encoder_type[10];
	sprintf(latency, "%lld", angle_encoder_ele.var__angle_encoder_info.candev_head_.latency_);

	if (!SaveResult.c_str()){
		return -1;
	}
	rapidxml::file<> *file = nullptr;
	try{
		rapidxml::xml_document<> doc;
		file = new rapidxml::file<>(SaveResult.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();

		if (root)
		{
			rapidxml::xml_node<>*anode = root->first_node();
			rapidxml::xml_node<>*bnode = anode->first_node();
			rapidxml::xml_node<>*delnode = anode;
			while (anode != NULL)
			{
				bnode = anode->first_node();
				delnode = anode;


				if (bnode != NULL){
					if (angle_encoder_ele.id_ == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL && angle_encoder_ele.id_ != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) anode = anode->next_sibling();
				}
			}
		}
		root = doc.first_node();
		rapidxml::xml_node<>* mynode = root->first_node();
		rapidxml::xml_node<>* angleencoder = doc.allocate_node(rapidxml::node_element, "device", NULL);
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(angle_encoder_ele.id_, id, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "name", angle_encoder_ele.name_));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "can", _itoa(angle_encoder_ele.var__angle_encoder_info.candev_head_.canbus_, can, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "port", _itoa(angle_encoder_ele.var__angle_encoder_info.candev_head_.canport_, port, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "node", _itoa(angle_encoder_ele.var__angle_encoder_info.candev_head_.cannode_, node, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "latency", latency));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "merge", _itoa(angle_encoder_ele.var__angle_encoder_info.candev_head_.merge_, merge, 10)));
		angleencoder->append_node(doc.allocate_node(rapidxml::node_element, "encoder_type", _itoa(angle_encoder_ele.var__angle_encoder_info.encoder_type_, encoder_type, 10)));

		root->insert_node(mynode, angleencoder);
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		return -1;
	}
	return 0;
}

int __stdcall update_dio_element(dio&var__dio_element)
{
	//std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	//SaveResult += "\\etc\\dev\\dio.xml";

	//char id[10];
	//char can[10];
	//char port[10];
	//char node[10];
	//char latency[32];
	//char merge[10];
	//char pdocnt[10];
	//char di[10];
	//char doo[10];
	//char ai[10];
	//char ao[10];

	//sprintf(latency, "%lld", var__dio_element.var__dio_info.candev_head_.latency_);

	//if (!SaveResult.c_str()){
	//	return -1;
	//}
	//rapidxml::file<> *file = nullptr;
	//try{
	//	rapidxml::xml_document<> doc;
	//	file = new rapidxml::file<>(SaveResult.c_str());
	//	doc.parse<0>(file->data());
	//	rapidxml::xml_node<>* root = doc.first_node();

	//	if (root)
	//	{
	//		rapidxml::xml_node<>*anode = root->first_node();
	//		if (NULL == anode){
	//			delete file;
	//			return -1;
	//		}
	//		rapidxml::xml_node<>*bnode = anode->first_node();
	//		rapidxml::xml_node<>*delnode = anode;
	//		while (anode != NULL)
	//		{
	//			bnode = anode->first_node();
	//			delnode = anode;

	//			if (bnode != NULL){
	//				if (var__dio_element.id_ == atoi(bnode->value())){
	//					root->remove_node(delnode);
	//					break;
	//				}
	//				if (anode->next_sibling() == NULL && var__dio_element.id_ != atoi(bnode->value()))break;
	//				if (anode->next_sibling() != NULL) anode = anode->next_sibling();
	//			}
	//		}
	//	}
	//	root = doc.first_node();
	//	rapidxml::xml_node<>* mynode = root->first_node();
	//	rapidxml::xml_node<>* dionode = doc.allocate_node(rapidxml::node_element, "device", NULL);
	//	dionode->append_node(doc.allocate_node(rapidxml::node_element, "id", _itoa(var__dio_element.id_, id, 10)));
	//	dionode->append_node(doc.allocate_node(rapidxml::node_element, "name", var__dio_element.name_));
	//	dionode->append_node(doc.allocate_node(rapidxml::node_element, "can", _itoa(var__dio_element.var__dio_info.candev_head_.canbus_, can, 10)));
	//	dionode->append_node(doc.allocate_node(rapidxml::node_element, "port", _itoa(var__dio_element.var__dio_info.candev_head_.canport_, port, 10)));
	//	dionode->append_node(doc.allocate_node(rapidxml::node_element, "node", _itoa(var__dio_element.var__dio_info.candev_head_.cannode_, node, 10)));
	//	dionode->append_node(doc.allocate_node(rapidxml::node_element, "latency", latency));
	//	dionode->append_node(doc.allocate_node(rapidxml::node_element, "merge", _itoa(var__dio_element.var__dio_info.candev_head_.merge_, merge, 10)));
	//	dionode->append_node(doc.allocate_node(rapidxml::node_element, "pdocnt", _itoa(var__dio_element.var__dio_info.candev_head_.pdocnt_, pdocnt, 10)));
	//	rapidxml::xml_node<>*channelnum = doc.allocate_node(rapidxml::node_element, "channel_num");
	//	dionode->append_node(channelnum);
	//	channelnum->append_attribute(doc.allocate_attribute("di", _itoa(var__dio_element.var__dio_info.di_channel_num_, di, 10)));
	//	channelnum->append_attribute(doc.allocate_attribute("do", _itoa(var__dio_element.var__dio_info.do_channel_num_, doo, 10)));
	//	channelnum->append_attribute(doc.allocate_attribute("ai", _itoa(var__dio_element.var__dio_info.ai_channel_num_, ai, 10)));
	//	channelnum->append_attribute(doc.allocate_attribute("ao", _itoa(var__dio_element.var__dio_info.ao_channel_num_, ao, 10)));
	//	rapidxml::xml_node<>*ais = doc.allocate_node(rapidxml::node_element, "ais", NULL);
	//	dionode->append_node(ais);
	//	char ch_tmp[100][10];
	//	char nr_tmp[100][10];
	//	char sc_tmp[100][10];
	//	char bi_tmp[100][10];

	//	/*for (unsigned int i = 0; i < var__dio_element.var__dio_info.di_channel_num_; i++){
	//		rapidxml::xml_node<>*ai = doc.allocate_node(rapidxml::node_element, "ai");
	//		ais->append_node(ai);
	//		ai->append_attribute(doc.allocate_attribute("channel", _itoa(var__dio_element.var__dio_info.aicfg_[i].channel_, ch_tmp[i], 10)));
	//		ai->append_attribute(doc.allocate_attribute("nrom", _itoa(var__dio_element.var__dio_info.aicfg_[i].norm_, nr_tmp[i], 10)));
	//		ai->append_attribute(doc.allocate_attribute("scale_coef", _itoa(var__dio_element.var__dio_info.aicfg_[i].scale_coef_, sc_tmp[i], 10)));
	//		ai->append_attribute(doc.allocate_attribute("bias_coef", _itoa(var__dio_element.var__dio_info.aicfg_[i].bias_coef_, bi_tmp[i], 10)));
	//	}*/
	//	rapidxml::xml_node<>*aos = doc.allocate_node(rapidxml::node_element, "aos", NULL);
	//	dionode->append_node(aos);
	//	char cha_tmp[100][10];
	//	char nro_tmp[100][10];
	//	char sca_tmp[100][10];
	//	char bia_tmp[100][10];

	//	for (unsigned int i = 0; i < var__dio_element.var__dio_info.di_channel_num_; i++){
	//		rapidxml::xml_node<>*ao = doc.allocate_node(rapidxml::node_element, "ao");
	//		aos->append_node(ao);
	//		ao->append_attribute(doc.allocate_attribute("channel", _itoa(var__dio_element.var__dio_info.aocfg_[i].channel_, cha_tmp[i], 10)));
	//		ao->append_attribute(doc.allocate_attribute("nrom", _itoa(var__dio_element.var__dio_info.aocfg_[i].norm_, nro_tmp[i], 10)));
	//		ao->append_attribute(doc.allocate_attribute("scale_coef", _itoa(var__dio_element.var__dio_info.aocfg_[i].scale_coef_, sca_tmp[i], 10)));
	//		ao->append_attribute(doc.allocate_attribute("bias_coef", _itoa(var__dio_element.var__dio_info.aocfg_[i].bias_coef_, bia_tmp[i], 10)));
	//	}

	//	root->insert_node(mynode, dionode);
	//	std::ofstream out(SaveResult);
	//	out << doc;
	//	out.close();
	//	delete file;
	//}
	//catch (...){
	//	return -1;
	//}
	return 0;
}