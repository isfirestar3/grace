#pragma once

#include "asio.hpp"
#include <vector>

//定义所有传输状态转换
#define		NORMAL					"正常"
#define		FAILED_READ			"读取失败"
#define		FAILED_WRITE			"写入失败"
#define		FAILED_GET				"获取失败"

struct process_info
{
	uint32_t process_id;
	std::string process_name;
};

struct process_list_info : motion::asio_data
{
	std::vector<process_info> vct_process_;
};

struct common_msg : motion::asio_data
{
	std::string msg_;
};
