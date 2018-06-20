#include "compiler.h"
#include "log.h"
#include <signal.h>
#include "argvs.h"
#include "os_util.hpp"
#include "tcp_manager.h"
#include "udp_manager.h"

extern
parament_run p_run;

int single_hanler(int signum)
{
	if (signum == SIGINT) {
	}
	return 0;
}

int main(int argc, char **argv)
{
	static const char startup_message[] = POSIX__EOL
		"****************************************************************************"POSIX__EOL
		"*                            application startup                           *"POSIX__EOL
		"****************************************************************************";
	loinfo(MODULE_NAME) << startup_message;

	// 命令行检查参数 
	if (check_argv(argc, argv) < 0) {
		return 1;
	}

	// 监听、运行 
	if (nsp::toolkit::singleton<tcp_manager>::instance()->tcp_listen(p_run.inet, p_run.port)) {
		return 2;
	}
	if (nsp::toolkit::singleton<udp_manager>::instance()->udp_listen(p_run.inet, p_run.port)) {
		return 3;
	}

	// 等待退出
	nsp::os::pshang();
}
