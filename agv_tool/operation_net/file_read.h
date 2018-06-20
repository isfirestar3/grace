#pragma once

#include <string>
#include <stdint.h>
#include <Windows.h>
#include <io.h>
#include <vector>
#include <thread>
#include <deque>

#define FILE_BLOCK_SIZE		0x00010000 //64k
#define HANDLE_INVALID		-1

struct file_head_info{
	uint64_t total_size_ = 0;
	uint32_t total_block_num_ = 0;
	std::string create_time_;
	std::string last_access_time_;
	std::string last_write_time_;
};

class file_read{
public:
	file_read();
	~file_read();

public:
	HANDLE read_file_head_info(const std::string& str,file_head_info& f_info);
	int read_file_stream(const HANDLE file_handler, const uint64_t offset, const uint32_t read_size, std::string& data);
	void close_file_handler(const HANDLE file_handler);
	int get_floder_files(const std::string& input_folder,std::vector<std::string>& vct_files);
	int read_folder_info(const std::string& folder_str, file_head_info& f_info);

	void file_copy_handler(const std::string& src_file, const std::string& des_file);
	void cala_crc(const char* input_file, unsigned int& cra);
};