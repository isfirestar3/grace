#include "rw_xml_file.h"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include <Windows.h>

rw_xml_file::rw_xml_file() {

}

rw_xml_file::~rw_xml_file() {

}

char * rw_xml_file::utf82gbk(char* strutf)
{
	//utf-8转为Unicode
	int size = MultiByteToWideChar(CP_UTF8, 0, strutf, -1, NULL, 0);
	WCHAR   *strUnicode = new   WCHAR[size];
	MultiByteToWideChar(CP_UTF8, 0, strutf, -1, strUnicode, size);

	//Unicode转换成UTF-8;
	int i = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);
	char   *strGBK = new   char[i];
	WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, strGBK, i, NULL, NULL);
	return strGBK;
}


int rw_xml_file::read_xml_element(const std::string& file_path, const std::string& parent_ele, const std::string& child_ele, std::string& out_value) {
	rapidxml::file<> file_xml(file_path.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(file_xml.data());
	rapidxml::xml_node<> *xml_root = doc.first_node();
	if (!xml_root) {
		return -1;
	}

	for (rapidxml::xml_node<>*anode = xml_root->first_node(); anode != NULL; anode = anode->next_sibling()) {
		if (strcmp(anode->name(), parent_ele.c_str()) == 0) {
			rapidxml::xml_node<>* child_node = anode->first_node();
			for (; child_node != NULL; child_node = child_node->next_sibling()) {
				if (strcmp(child_node->name(), child_ele.c_str()) == 0) {
					out_value = utf82gbk(child_node->value());
				}
			}
		}
	}

	return 0;
}

int rw_xml_file::read_attribute(const std::string& file_path, const std::string& node_name, const std::string& attri, std::string& output)
{
	rapidxml::file<> file_xml(file_path.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(file_xml.data());
	rapidxml::xml_node<> *xml_root = doc.first_node();
	if (!xml_root) {
		return -1;
	}
	rapidxml::xml_node<> *node = xml_root->first_node(node_name.c_str());
	if (!node)
	{
		return -1;
	}
	rapidxml::xml_attribute<>* attr = node->first_attribute(attri.c_str());
	if (!attr)
	{
		return -1;
	}
	output = attr->value();
	return 0;
}

int rw_xml_file::read_attribute(const std::string& file_path, const std::string& father_node, const std::string& attri,
	std::vector<std::string>& vct_output)
{
	rapidxml::file<> file_xml(file_path.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(file_xml.data());
	rapidxml::xml_node<> *xml_root = doc.first_node();
	if (!xml_root) {
		return -1;
	}
	rapidxml::xml_node<> *node = xml_root->first_node(father_node.c_str());
	if (!node) {
		return -1;
	}
	rapidxml::xml_node<> *child_node = node->first_node();
	for (; child_node != NULL; child_node = child_node->next_sibling())
	{
		rapidxml::xml_attribute<>* item_attr = child_node->first_attribute(attri.c_str());
		if (item_attr)
		{
			vct_output.push_back(item_attr->value());
		}
	}
	return 0;
}