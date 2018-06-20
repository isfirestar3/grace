#pragma once

#include <string>

struct firmware_info
{
	std::string modules_type_;
	int firmware_length_;
	std::string firmware_name_;
	std::string firmware_path_;
	int firmware_type;
	std::string firmware_version_;
	int firmware_version_control_;
	int reset_wait_time_;
	int block_write_delay_;
	std::string firmware_cpu_type_;
};

class config{
public:
	config();
	~config();

public:
	int read_xml_file(const std::string&file_name, firmware_info& info);
};