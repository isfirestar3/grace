#pragma once

#include <string>
#include <stdio.h>
#include "singleton.hpp"
#include <map>
#include <sys/types.h>  
#include <sys/stat.h>
#include <mutex>

#define FILE_BLOCK_SIZE 1024
#define FILE_BLOCK_SIZE_CAN		8

class file_read_handler{
	file_read_handler();
	~file_read_handler();
	friend class nsp::toolkit::singleton<file_read_handler>;

public:
	int open_file(const std::string& file_name);
	int get_file_block_num();
	std::string get_file_block_data(const int index);
	uint32_t get_file_size();
	int get_file_buffer(std::string& buffer_data);

	void set_block_size(uint32_t block_size);

	uint16_t get_check_sum(){ return check_sum_; };
private:
	int close_file();
	void check_sum(std::string buffer);
private:
	uint32_t file_size_;
	FILE* file_handler_ = nullptr;
	std::recursive_mutex res_mutex_;
	std::map<int, std::string> map_file_block_;//文件块集合，key:片号，value：每一片数据集合
	uint32_t file_block_size_;
	uint16_t check_sum_ = 0;
};