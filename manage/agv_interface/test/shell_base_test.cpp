#include "agv_shell_base.h"
#include "os_util.hpp"

int main() {
	int ret = -1;
	std::string ipv4 = "127.0.0.1";
	std::shared_ptr<agv_shell_base> shell_base;
	try
	{
		shell_base = std::make_shared<agv_shell_base>();
		ret = shell_base->login_agv_shell(ipv4, 4410);
		if(ret < 0) {
			printf("login to agvshell failure!\n");
		} else {
			printf("login agvshell success!\n");
		}
	
	} catch(...) {
		printf("create agv_shell_base failure!!!, errno:%d\n", errno);
		return -1;
	}
	
	std::vector<process_info> vct_process;
	ret = shell_base->get_agv_shell_process_list(vct_process);
	if(ret >= 0) {
		printf("get shell process list success\n");
		for(auto& it : vct_process) {
			printf("process_id:%u, process_name:%s \n", it.process_id, it.process_name.c_str());
		}
	}
	
	nsp::os::waitable_handle wait(0);
	std::vector<process_status> vct_p;
	for(int i=0; i < 5; ++i) {
		ret = shell_base->get_agv_shell_process_status(vct_p);
		if(ret >= 0) {
			printf("get shell process status success\n");
			for(auto& it : vct_p) {
				printf("process_id:%u, status:%d \n", it.process_id, it.status);
			}
		}
		wait.wait(1000);
	}
	
	std::vector<process_command> vct_p_command;
	process_command pc;
	for(auto& it : vct_process) {
		pc.p_id_ = it.process_id;
		pc.p_command_ = "-s --service-tcp-port=5409";
		vct_p_command.push_back(pc);
	}
	ret = shell_base->post_agv_shell_process_cmd(vct_p_command, command_agv_shell::cmd_restart);
	if(ret >= 0) {
		printf("post_agv_shell_process_cmd success\n");
	}
	
	wait.wait();
	
	return 0;
}
