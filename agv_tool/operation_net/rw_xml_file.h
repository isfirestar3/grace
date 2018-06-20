#pragma once

#include <string>
#include <vector>

class rw_xml_file {
public:
	rw_xml_file();
	~rw_xml_file();

public:
	int read_xml_element(const std::string& file_path, const std::string& parent_ele, const std::string& child_ele,std::string& out_value);
	int read_attribute(const std::string& file_path, const std::string& node_name, const std::string& attri, std::string& output);
	int read_attribute(const std::string& file_path, const std::string& father_node, const std::string& attri, std::vector<std::string>& vct_output);

private:
	char * utf82gbk(char* strutf);
};
