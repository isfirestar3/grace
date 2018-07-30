#include "argv.h"
#include "agv_shell_common.h"
#include "const.h"
#include "posix_string.h"
#include <string>


/* 显示命令行参数及其详情 */
static
void run__dispay_usage() {
	static const char *usage_context =
		"NAME\n"
		"\agv_shell\n"
		"\t\t[-h|--help] display usage context and help informations\n"
		"\t\t[-v|--version] display versions of executable archive\n"
		"\t\t[--shell-port][%%PORT] specify port for local AGV_SHELL TCP network service.\n"
		"\t\t[--fts-port][%%PORT] specify port for local FTS TCP network service.\n"
		"\t\t[--fts-long-port][%%PORT] specify port for local FTS LONG TCP network service.\n"
		"\t\t[--default] start server user default port.\n"
		;
	printf("%s", usage_context);
}

/* 打印版权信息和版本信息 */
static
void run__display_author_information() {
    static const char *author_context =
            "agv_shell 1,1,0,0\n"
            "Copyright (C) 2017 Zhejiang Guozi RoboticsCo., Ltd.\n"
            "For bug reporting instructions, please see:\n"
            "<http://www.gzrobot.com/>.\n"
            "Find the AGV manual and other documentation resources online at:\n"
            "<http://www.gzrobot.com/productCenter/>.\n"
            "For help, type \"help\".\n"
            ;
    printf("%s", author_context);
}

static parament_run p_run;

int check_argv(int argc,char** argv)
{
	int opt_index;
    int opt;
    int retval = 0;
    char shortopts[128];
	
	posix__sprintf(shortopts, cchof(shortopts), "hsv:%d:%d:%d:%d", kInvisibleOptIndex_ShellPort, kInvisibleOptIndex_FtsPort, \
						kInvisibleOptIndex_FtsLongPort, kInvisibleOptIndex_Default);
    opt = getopt_long(argc, argv, shortopts, long_options, &opt_index);

	if (opt == -1) {
		printf("input parament is empty.");
		return 0;
	}
	
	while (opt != -1) {
		switch(opt)
		{
			case 'h':
				retval = -1;
				run__dispay_usage();
				break;
			case 'v':
				retval = -1;
				run__display_author_information();
				break;
			case 's':
				if(nsp::toolkit::singleton<global_parameter>::instance()->get_auto_startup() > 0) {
					nsp::toolkit::singleton<global_parameter>::instance()->set_auto_startup(1);
				} else {
					nsp::toolkit::singleton<global_parameter>::instance()->set_auto_startup(0);
				}
				break;
			case kInvisibleOptIndex_ShellPort:
				if (optarg){
					nsp::toolkit::singleton<global_parameter>::instance()->set_server_port((uint16_t)strtoul(optarg, NULL, 10));
				}
				break;
			case kInvisibleOptIndex_FtsPort:
				if (optarg){
					nsp::toolkit::singleton<global_parameter>::instance()->set_fts_port((uint16_t)strtoul(optarg, NULL, 10));
				}
				break;
			case kInvisibleOptIndex_FtsLongPort:
				if (optarg){
					nsp::toolkit::singleton<global_parameter>::instance()->set_fts_long_port((uint16_t)strtoul(optarg, NULL, 10));
				}
				break;
			case kInvisibleOptIndex_Default:
				p_run.shell_port_ = SHELL_TCP_PORT;
				p_run.fts_port_ = SHELL_FTS_TCP_PORT;
				p_run.fts_long_port_ = SHELL_FTS_LONG_TCP_PORT;
				break;
			default:
				retval = -1;
				run__dispay_usage();
				break;
		}
		opt = getopt_long(argc, argv, shortopts, long_options, &opt_index);
	}
	return retval;
}
