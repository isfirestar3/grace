#include <string>
#include <iostream>
#include <stdio.h>
#include <stdint.h>

#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

#include "posix_string.h"
#include "os_util.hpp"
#include "log.h"

#include "libagvinfo.h"
#include "argvs.h"
#include "getopt.h"
#include "errno.h"

// 运行参数 
parament_run p_run;
#define FLAG_SET_IPADDR			(0x01)
#define FLAG_SET_PORT			(0x02)
#define FLAG_SET_DHCP_SUPPORT	(0x04)
#define FLAG_SET_XML_PERIODS	(0x08)
static int para_xml_set = 0;
static int para_cmd_set = 0;

// 命令行长参数定义 
static const struct option long_options[] = {
	{ "help",				no_argument, NULL, 'h' },
	{ "version",			no_argument, NULL, 'v' },
	{ "ipaddr",				required_argument, NULL, 'i' },
	{ "port",				required_argument, NULL, 'p' },
	{ "dhcp-support",		required_argument, NULL, 'd' },
	{ "xml-sync-interval",	required_argument, NULL, 'x' },
	{ NULL, 0, NULL, 0 }
};

/* 显示命令行参数及其详情 */
static void dispay_usage(char *proc_name) {
	static const char *usage_context =
		"Usage: %s [OPTION]...\n"
		"  [-h|--help]         display usage context and help informations\n"
		"                        when no options are speciied, file option.xml is used.\n"
		"                        when no options are speciied, and file option.xml is'nt\n"
		"                        speciied, default value will be used.\n"
		"  [-v|--version]      display versions of executable archive\n"
		"  [-i|--ipaddr]       specify IPv4 address for local TCP network service.\n"
		"                        default value is 0.0.0.0\n"
		"  [-p|--port]         specify IPv4 address for local TCP network service.\n"
		"                        default value is %d\n"
		"  [-x|--xml-periods] specify interval for agvinfo-server synchronous configures\n"
		"                       on disk xml file to memory cache, in second.\n"
		"                       MAXDWORD means no need to perceive changes in disk files.\n"
		"                       default value is %d millisecond\n"
		;
	printf(usage_context, proc_name, DEFAULT_IPADDR, DEFAULT_PORT, DEFAULT_XML_SYNC_INTERVAL);
}

/* 打印版权信息和版本信息 */
static void display_author_information(char *proc_name) {
	static const char *author_context =
		"%s %s\n"
		"Copyright (C) 2017 Zhejiang Guozi RoboticsCo., Ltd.\n"
		"For bug reporting instructions, please see:\n"
		"<http://www.gzrobot.com/>.\n"
		"Find the AGV manual and other documentation resources online at:\n"
		"<http://www.gzrobot.com/productCenter/>.\n"
		"For help, type \"help\".\n"
		;
	printf(author_context, proc_name, VERSION);
}

// 从XML文件中读取运行参数
static void load_agvconf_xml() {
	std::string file_path = nsp::os::get_module_directory<char>();
	std::string dock_path = file_path +POSIX__DIR_SYMBOL + CONFIGE_FILE_NAME;
	rapidxml::xml_document<> doc;
	rapidxml::file<char> *file = nullptr;
	unsigned long ret = 0;
	char *endptr = NULL;

	loinfo(MODULE_NAME) << "loadXml " << dock_path;
	try {
		file = new rapidxml::file<char>(dock_path.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node(MODULE_NAME);
		if (root) {
			rapidxml::xml_attribute<>* att_ipaddr = root->first_attribute("ipv4");
			if (att_ipaddr && !(para_cmd_set & FLAG_SET_IPADDR)) {
				// 命令行没有设置时以xml文件设置为准 
				p_run.inet = att_ipaddr->value();
				para_xml_set |= FLAG_SET_IPADDR;
				loinfo(MODULE_NAME) << "loadXml ipv4:" << att_ipaddr->value();
			}
			rapidxml::xml_attribute<>* att_port = root->first_attribute("port");
			if (att_port && !(para_cmd_set & FLAG_SET_PORT)) {
				// 命令行没有设置时以xml文件设置为准 
				ret = strtoul(att_port->value(), &endptr, 10);
				if (errno || ret < 1 || ret > 65535 || (endptr && *endptr != '\0')) {
					// 有错误忽略(为缺省值)
					loinfo(MODULE_NAME) << "loadXml port error:" << att_port->value();
				}
				else {
					p_run.port = (uint16_t)ret;
					para_xml_set |= FLAG_SET_PORT;
					loinfo(MODULE_NAME) << "loadXml port:" << att_port->value();
				}
			}
			rapidxml::xml_attribute<>* att_periods = root->first_attribute("periods");
			if (att_periods && !(para_cmd_set & FLAG_SET_XML_PERIODS)) {
				// 命令行没有设置时以xml文件设置为准 
				ret = strtoul(att_periods->value(), &endptr, 10);
				if (errno || (endptr && *endptr != '\0')) {
					// 有错误忽略(为缺省值)
					loinfo(MODULE_NAME) << "loadXml periods error:" << att_periods->value();
				} else {
					p_run.periods = (uint32_t)ret;
					para_xml_set |= FLAG_SET_XML_PERIODS;
					loinfo(MODULE_NAME) << "loadXml periods:" << att_periods->value();
				}
			}
		}
		delete file;
	}
	catch (...) {
		delete file;
		loinfo(MODULE_NAME) << "loadXml err";
	}

	loinfo(MODULE_NAME) << "loadXml end";
}

// 当命令行和XML文件中都未指定时，运行参数的值设置为缺省值 
static void load_agvconf_default()
{
	if (!(para_cmd_set & FLAG_SET_IPADDR) && !(para_xml_set & FLAG_SET_IPADDR)) {
		p_run.inet = DEFAULT_IPADDR;
	}

	if (!(para_cmd_set & FLAG_SET_PORT) && !(para_xml_set & FLAG_SET_PORT)) {
		p_run.port = DEFAULT_PORT;
	}

	if (!(para_cmd_set & FLAG_SET_XML_PERIODS) && !(para_xml_set & FLAG_SET_XML_PERIODS)) {
		p_run.periods = DEFAULT_XML_SYNC_INTERVAL;
	}
}

int check_argv(int argc, char** argv)
{
	int opt_index;
	int opt;
	int retval = 0;
	unsigned long ul = 0;
	char *endptr = NULL;

	while (1) {
		opt = getopt_long(argc, argv, "hvi:p:x:", long_options, &opt_index);
		if (opt == -1) {
			break;
		}
		switch (opt) {
		case 'h':
			retval = -1;
			dispay_usage(argv[0]);
			break;
		case 'v':
			retval = -1;
			display_author_information(argv[0]);
			break;
		case 'i':
			if (optarg){
				p_run.inet = optarg;
				para_cmd_set |= FLAG_SET_IPADDR;
			}
			break;
		case 'p':
			if (optarg){
				ul = strtoul(optarg, &endptr, 10);
				if (errno || ul < 1 || ul > 65535 || (endptr && *endptr != '\0')) {
					// 有错误忽略(为缺省值)
					loinfo(MODULE_NAME) << "command line invalid port: " << optarg;
				}
				else {
					p_run.port = (uint16_t)ul;
					para_cmd_set |= FLAG_SET_PORT;
				}
			}
			break;
		case 'x':
			if (optarg){
				ul = strtoul(optarg, &endptr, 10);
				if (errno || ul == 0 || (endptr && *endptr != '\0')) {
					// 有错误忽略(为缺省值)
					loinfo(MODULE_NAME) << "command line invalid periods: " << optarg;
				} else {
					p_run.periods = (uint32_t)ul;
					para_cmd_set |= FLAG_SET_XML_PERIODS;
				}
			}
			break;
		default:
			retval = -1;
			dispay_usage(argv[0]);
			return retval;
			break;
		}
	}

	load_agvconf_xml();
	load_agvconf_default();

	return retval;
}
