#pragma once

#include <string>
#include <stdint.h>
#include "file_shell_handler.h"

#if _WIN32
#include <windows.h>
#include <Shlwapi.h>
#else
#if defined _LARGEFILE64_SOURCE
#undef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 
#else
#define _LARGEFILE64_SOURCE 
#endif
#include <unistd.h>
#include <sys/types.h>
#include <utime.h>
#include <fcntl.h>
#endif


class file_write_handler{

public:
	file_write_handler();
	~file_write_handler();

public:
	void* create_file(const std::string& file_path, const uint64_t& create_time, const uint64_t& modify_time, const uint64_t& access_time, int & error_code);
	int create_folder(const std::string& file_path, int & error_code);
	int write_file_block(const void* handler, const uint64_t offset, const std::string& stream);
	bool close_handler(const void* handler);
	int delete_file(const std::string&file_path);
	int reset_file_time(const std::string& file_name, const uint64_t& create_time, const uint64_t& modify_time, const uint64_t& access_time);

private:
	file_shell_handler shell_handler_;
};