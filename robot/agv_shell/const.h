#ifndef __GRACE_ROBOT_AGVSHELL_CONST_H__
#define __GRACE_ROBOT_AGVSHELL_CONST_H__

#define MAX_LINE 	256

#define MAXDWORD 	0xffffffff 
//threads num in agv_shell_server
#define NET_WINDOW_SIZE 	1
//tcp port
#define SHELL_TCP_PORT 	4410
//unuseful
#define SHELL_FTS_TCP_PORT 	4411
//unuseful
#define SHELL_FTS_LONG_TCP_PORT 	4412
//config file name
#define CONFIGE_FILE_NAME			"/standard/agv_shell.xml"
#define UPDATE_FILE_NAME			"graceup.sh"
#define SOFT_VERSION_FILE_NAME		"version.txt"
#define CONFIG_VERSION_FILE_NAME	"version_config.txt"
#define AUTO_START_FILE_NAME		"autostart.sh"

#define TCP_KEEPALIVE_TIME_INTERVAL 	2000
#define TCP_KEEPALIVE_MAX_COUNT 	5
#define VCU_UDP_KEEPALIVE_TIME_INT 	3000
#define KEEPALIVE_RESERVE_SIZE 	(2*1024) 
#define PROTO_SESSION_KEY_LENGTH 	32

//file transform rela
//64kb
#define FILE_BLOCK_SIZE 	0x00010000
//默认窗口大小
#define TRANSFER_WINDOW_SIZE 	1
//默认链接超时时间10000毫秒
#define TIMEOUT_TRANSFER 	10000
//read and write threads
#define FILE_OPERATOR_THREAD_SIZE 	2

#define MAX_DIR_PATH_LEN 	1024
#define FILE_CAN_BLOCK_SIZE 	1024
#define FILE_CAN_BLOCK_SIZE_CAN 	8

#endif  //__GRACE_ROBOT_AGVSHELL_CONST_H__
