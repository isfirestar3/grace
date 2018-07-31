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
	//创建文件, 返回 &fd
	void* create_file(const std::string& file_path, const uint64_t& create_time, const uint64_t& modify_time, const uint64_t& access_time, int & error_code);
	//创建文件, 返回 FILE*
	FILE* create_file(const std::string& file_name, const unsigned long file_size);
	//创建文件夹
	int create_folder(const std::string& file_path, int & error_code);
	//写入文件, 描述符为  fd 
	int write_file_block(const void* handler, const uint64_t offset, const std::string& stream);
	//写入文件, 描述符为 FILE* 
	int write_file(FILE* file_handle, const unsigned long offset,const std::string& data);
	int close_handler(const void* handler);
	inline int close_file(FILE* file_handle) {
		return (file_handle != NULL) ? fclose(file_handle) : -1;
	}
	//删除文件
	int delete_file(const std::string&file_path);
	//重置文件时间 
	int reset_file_time(const std::string& file_name, const uint64_t& create_time, const uint64_t& modify_time, const uint64_t& access_time);

private:
	file_shell_handler shell_handler_;
	uint8_t time_zone_;
};
