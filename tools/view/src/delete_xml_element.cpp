#define _CRT_SECURE_NO_WARNINGS
#include "xml_module.h"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include <stdio.h>
#include "os_util.hpp"

int __stdcall delete_canbus_element(int canbus_id)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\canbus.xml";

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
					if (canbus_id == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL&&canbus_id != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) {
						anode = anode->next_sibling();
					}
				}
			}
		}
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

int __stdcall delete_elmo_element(int elmo_id)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\elmo.xml";

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
					if (elmo_id == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL&&elmo_id != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) {
						anode = anode->next_sibling();
					}
				}
			}
		}
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

int __stdcall delete_driveunit_element(int driveunit_id)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\driveunit.xml";

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
					if (driveunit_id == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL&&driveunit_id != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) {
						anode = anode->next_sibling();
					}
				}
			}
		}
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

int __stdcall delete_dwheel_element(int dwheel_id)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\dwheel.xml";

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
					if (dwheel_id == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL&&dwheel_id != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) {
						anode = anode->next_sibling();
					}
				}
			}
		}
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

int __stdcall delete_swheel_element(int swheel_id)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\swheel.xml";

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
					if (swheel_id == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL&&swheel_id != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) {
						anode = anode->next_sibling();
					}
				}
			}
		}
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

int __stdcall delete_sddex_element(int sddex_id)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\var\\sddex.xml";

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
					if (sddex_id == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL&&sddex_id != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) {
						anode = anode->next_sibling();
					}
					
				}
			}
		}
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

int __stdcall delete_mnt_element(mnt& mnt_ele)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\mnt.xml";

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
			mnt tmp;
			rapidxml::xml_node<>*anode = root->first_node(); 
			rapidxml::xml_node<char> * delnode = anode;
			rapidxml::xml_attribute<char> * attr = anode->first_attribute();
			while (anode != NULL)
			{
				delnode = anode;
				attr = anode->first_attribute();

				if (attr != NULL){
					tmp.from = atoi(attr->value());
				}attr = attr->next_attribute();
				if (attr != NULL){
					tmp.foff = atoi(attr->value());
				}attr = attr->next_attribute();
				if (attr != NULL){
					tmp.to = atoi(attr->value());
				}attr = attr->next_attribute();
				if (attr != NULL){
					tmp.toff = atoi(attr->value());
				}attr = attr->next_attribute();
				if (attr != NULL){
					tmp.len = atoi(attr->value());
				}
				if ((tmp.from == mnt_ele.from) && (tmp.foff == mnt_ele.foff) && (tmp.to == mnt_ele.to) && (tmp.toff == mnt_ele.toff) && (tmp.len == mnt_ele.len))
				{
					root->remove_node(delnode);
					break;
				}
				if (anode->next_sibling() == NULL && ((tmp.from != mnt_ele.from) || (tmp.foff != mnt_ele.foff) || (tmp.to != mnt_ele.to) || (tmp.toff != mnt_ele.toff) || (tmp.len != mnt_ele.len)))break;
				if (anode->next_sibling() != NULL) {
					anode = anode->next_sibling();
				}
			}
		}
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

int __stdcall delete_moos_element(int moos_id)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\moos.xml";

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
					if (moos_id == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL&&moos_id != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) {
						anode = anode->next_sibling();
					}
				}
			}
		}
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

int __stdcall delete_angle_encoder_element(int angle_encoder_id)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\angle_encoder.xml";

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
					if (angle_encoder_id == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL&&angle_encoder_id != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) {
						anode = anode->next_sibling();
					}
				}
			}
		}
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

int __stdcall delete_dio_element(int dio_id)
{
	std::basic_string<char> SaveResult = nsp::os::get_module_directory<char>();
	SaveResult += "\\etc\\dev\\dio.xml";

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
					if (dio_id == atoi(bnode->value())){
						root->remove_node(delnode);
						break;
					}
					if (anode->next_sibling() == NULL&&dio_id != atoi(bnode->value()))break;
					if (anode->next_sibling() != NULL) {
						anode = anode->next_sibling();
					}
				}
			}
		}
		std::ofstream out(SaveResult);
		out << doc;
		out.close();
		delete file;
	}
	catch (...){
		delete file;
		return -1;
	}
	return 0;
}