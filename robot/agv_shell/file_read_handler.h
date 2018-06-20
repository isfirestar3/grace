#pragma once

#include <string>
#include <stdio.h>
#include "singleton.hpp"
#include <map>
#include <sys/types.h>  
#include <sys/stat.h>
#include <mutex>

#define FRIMWARE_FILE_BLOCK_SIZE 1024
#define FRIMWARE_FILE_CAN_CUSTOM	8

class file_read_handler{
	file_read_handler();
	~file_read_handler();
	friend class nsp::toolkit::singleton<file_read_handler>;

public:
	int open_file(const std::string& file_name);
	uint64_t get_file_size();
	void get_file_buffer(const uint32_t offset, const uint64_t file_size, std::string& buffer_data);

private:
	uint64_t file_size_;
	FILE* file_handler_ = nullptr;
	std::string file_data_;//文件数据集
};