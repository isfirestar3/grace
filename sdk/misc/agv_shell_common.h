#pragma once

#include <vector>
#include <string>
#include <stdint.h>

struct process_info
{
	uint32_t process_id;//进程id
	std::string process_name;//进程名称
};

enum agv_process
{
	stop = 0,
	running = 1,
};

struct process_status
{
	uint32_t process_id;//进程id
	agv_process status;//进程状态
};

enum command_agv_shell {
	cmd_none = -1,
	cmd_start = 0,
	cmd_stop,
	cmd_restart,
	cmd_reboot,
	cmd_shutdown,
};

struct process_command
{
	uint32_t p_id_;//进程id
	std::string p_command_;//进程命令参数
};

struct mac_address_info
{
	std::string ipv4;
	std::string mac_addr;
	uint16_t shell_port;
	uint16_t fts_port;
};