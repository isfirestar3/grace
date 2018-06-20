#pragma once

#include <stdio.h>
#include <string>

#define FILE_BLOCK_SIZE 1024

class file_write_handler {
public:
	file_write_handler();
	~file_write_handler();

public:
	int create_file(const std::string& file_name, const unsigned long file_size);
	int write_file(const std::string& file_name, const unsigned long offset,const std::string& data);
	int close_file();

private:
	FILE* file_handler_;
};