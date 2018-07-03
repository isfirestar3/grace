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
	int ret_value = 0;
	static const char startup_message[] = POSIX__EOL
		"****************************************************************************"POSIX__EOL
		"*                            application startup                           *"POSIX__EOL
		"****************************************************************************";
	log__write(MODULE_NAME, kLogLevel_Info, kLogTarget_Stdout | kLogTarget_Filesystem, startup_message);
	do {
		// 命令行检查参数 
		if (check_argv(argc, argv) < 0) {
			ret_value = 1;
			break;
		}

		// 监听、运行 
		if (nsp::toolkit::singleton<tcp_manager>::instance()->tcp_listen(p_run.inet, p_run.port)) {
			ret_value = 2;
			break;
		}
		if (nsp::toolkit::singleton<udp_manager>::instance()->udp_listen(p_run.inet, p_run.port)) {
			ret_value = 3;
			break;
		}
	} while (0);
	if (ret_value) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		log__write(MODULE_NAME, kLogLevel_Info, kLogTarget_Stdout | kLogTarget_Filesystem, "quit.");
		return ret_value;
	}

	// 等待退出
	nsp::os::pshang();
}
