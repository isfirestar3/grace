#pragma once

#include <string>

#define VERSION "1.0.0.1"
#define CONFIGE_FILE_NAME "agv_config.xml"
#define AGVINFO_FILE_NAME "agv_info.xml"
#define MODULE_NAME "agvinfo_server"

#define DEFAULT_IPADDR "0.0.0.0"
#define DEFAULT_PORT 9100
#define DEFAULT_XML_SYNC_INTERVAL 3

// 运行参数定义 
struct parament_run{
	std::string inet;
	uint16_t port;
	uint32_t periods;	// specify interval for agvinfo-server synchronous configures on disk xml file to memory cache, in second
						//  MAXDWORD means no need to perceive changes in disk files 
};

int check_argv(int argc, char** argv);
