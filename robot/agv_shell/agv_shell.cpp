#include "agv_shell_common.h"
#include "agv_shell_define.h"
#include "agv_shell_server.h"
#include "cmd_parse.h"
#include "dhcp_udp_session.h"
#include "keepalive.h"
#include "log.h"
#include "logger.h"
#include "udp_client_manager.h"
#include "argv.h"

extern
int mclog__init(const char *ip, uint16_t port);
extern
void mclog__uninit();
extern
int syslog__init(const char *ip, uint16_t port);
extern
void syslog__uninit();

void upgrade_program() {
    
	if (nsp::toolkit::singleton<global_parameter>::instance()->get_program_name().empty()) {
    	loerror("agv_shell") << "upgrandProgram new program name:" << nsp::toolkit::singleton<global_parameter>::instance()->get_program_name();
    	return;
    }
	std::string proc_path;
#ifdef _WIN32
	if (!nsp::toolkit::singleton<global_parameter>::instance()->get_program_path().empty()) {
		proc_path = nsp::toolkit::singleton<global_parameter>::instance()->get_program_path() + "agv_shell.exe";
	}
	else
	{
		proc_path = nsp::os::get_module_directory<char>() + "/agv_shell.exe";
	}
#else
	if (!nsp::toolkit::singleton<global_parameter>::instance()->get_program_path().empty()) {
		proc_path = nsp::toolkit::singleton<global_parameter>::instance()->get_program_path() + "agv_shell";
	}
	else
	{
		proc_path = "/agvshell/agv_shell";
	}
#endif

	loinfo("agv_shell") << "upgrand begin, new program name:" << nsp::toolkit::singleton<global_parameter>::instance()->get_program_name() << ", path:" << proc_path;
	std::this_thread::sleep_for(std::chrono::seconds(3));
	
#ifdef _WIN32
	//move文件
	std::string m_file = nsp::toolkit::singleton<global_parameter>::instance()->get_program_path() + "agv_shell.old.exe";
	if(!MoveFileExA(proc_path.c_str(),m_file.c_str(),MOVEFILE_REPLACE_EXISTING))
	{
		loerror("agv_shell") << "failed to rename old agv_shell file.";
		return;
	}
	//copy文件
	std::string src_file = nsp::toolkit::singleton<global_parameter>::instance()->get_program_name().c_str();
	if (!CopyFileExA(src_file.c_str(), proc_path.c_str(), NULL, NULL, NULL, COPY_FILE_OPEN_SOURCE_FOR_WRITE)) {
		int err = GetLastError();
		loerror("operation_net") << "failed to copy file:" << src_file << " to destination:" << proc_path << " ,the error code is " << err;
		return;
	}
	//启动进程
	if (start_process(proc_path, "-s") < 0)
	{
		loerror("agv_shell") << "failed to create new agv_shell process,the errno:" << errno;
		exit(-1);
	}
	else
	{
		exit(0);
	}

#else
	// 1. mv 老程序 
    char command[64];
    sprintf(command, "mv %s %s.old", proc_path.c_str(), proc_path.c_str());
	if(!nsp::toolkit::singleton<global_parameter>::instance()->bash_command(command,"r")){
		return ;
	}

    // 2. cp 新可执行程序为 agv_shell 
    memset(command, 0, 64);
    sprintf(command, "cp -f %s %s", nsp::toolkit::singleton<global_parameter>::instance()->get_program_name().c_str(), proc_path.c_str());
    lotrace("agv_shell") << "new program start execute cmd:" << command;
	if(!nsp::toolkit::singleton<global_parameter>::instance()->bash_command(command,"r")){
		return ;
	}
    // 4. fork 启动 agv_shell -s 成功后退出 
    pid_t pid = fork();
    int ret = 0;
    switch (pid) {
        case -1:
            loerror("agv_shell") << "fork failed";
            return;
            break;
        case 0:
            //这是在子进程中，调用execlp切换为ps进程 
            ret = execl(proc_path.c_str(), "agv_shell", "-s", NULL);
            if (ret < 0) {
                loerror("agv_shell") << "execl failure, errno:" << errno;
                perror(proc_path.c_str());
                exit(1);
            }
            
            break;
        default:
            //这是在父进程中，等待子进程结束并输出相关提示信息 
            int r = waitpid(pid, NULL, WNOHANG);
            if (r == 0) {
                loinfo("agv_shell") << "new agv_shell 2222 start success, pid=" << pid;
                exit(0);
                return;
            }
            break;
    }
#endif
}

void start_fts_server(){
	//创建短连接监听
	fts_create_receiver("0.0.0.0", nsp::toolkit::singleton<global_parameter>::instance()->get_fts_port());
	//创建长连接监听
	fts_create_receiver("0.0.0.0", nsp::toolkit::singleton<global_parameter>::instance()->get_fts_long_port(), true);
	std::string file_path = nsp::os::get_module_directory<char>() + "/standard";
	//创建文件夹
	nsp::os::mkdir_s(file_path);

	nsp::toolkit::singleton<agv_shell_server>::instance()->set_workpath(file_path);
	fts_parameter tmp;
	strcpy(tmp.file_search_root, file_path.c_str());
	tmp.timeout_cancel_transfer = 10000;
	fts_change_configure(&tmp);
}

static 
int agvi_init_network() {
	//start agv_shell ftp service
	int port_listen = nsp::toolkit::singleton<global_parameter>::instance()->get_server_port();
	if (nsp::toolkit::singleton<agv_shell_server>::instance()->begin_shell_server(port_listen) < 0) {
        loinfo("agv_shell") << "agvshell svr->begin failed";
        return -1;
    }
    
    loinfo("agv_shell") << "agvshell start listening " << nsp::toolkit::singleton<global_parameter>::instance()->get_server_port() << " ...";

	nsp::toolkit::singleton<udp_client_manager>::instance()->set_local_endpoint(nsp::toolkit::singleton<global_parameter>::instance()->get_local_ipv4(), nsp::toolkit::singleton<global_parameter>::instance()->get_local_udp_port());
	nsp::toolkit::singleton<udp_client_manager>::instance()->set_m_core_endpoint(nsp::toolkit::singleton<global_parameter>::instance()->get_m_core_ipv4(), nsp::toolkit::singleton<global_parameter>::instance()->get_m_core_port());
	nsp::toolkit::singleton<udp_client_manager>::instance()->set_camera_endpoint(nsp::toolkit::singleton<global_parameter>::instance()->get_camera_ipv4(), nsp::toolkit::singleton<global_parameter>::instance()->get_camera_port());
	nsp::toolkit::singleton<udp_client_manager>::instance()->set_reset_wait_time(nsp::toolkit::singleton<global_parameter>::instance()->get_reset_wait_time());
	nsp::toolkit::singleton<udp_client_manager>::instance()->set_version_control(nsp::toolkit::singleton<global_parameter>::instance()->get_frimware_version_control());
	//start udp network
	nsp::toolkit::singleton<udp_client_manager>::instance()->init_network();
	
	//start fts service 
	start_fts_server();
	
	//start dhcp udp service, send local info to fix ip
	nsp::toolkit::singleton<dhcp_udp_client_manager>::instance()->set_dhcp_fix_ep(nsp::toolkit::singleton<global_parameter>::instance()->get_dhcp_fix_ipv4(), nsp::toolkit::singleton<global_parameter>::instance()->get_dhcp_fix_port());
	nsp::toolkit::singleton<dhcp_udp_client_manager>::instance()->init_network();
	
	//start mclog && vcu syslog msg udp network
	std::string mclog_ip = nsp::toolkit::singleton<global_parameter>::instance()->get_local_mclog_ipv4();
	std::string syslog_ip = nsp::toolkit::singleton<global_parameter>::instance()->get_local_syslog_ipv4();
	mclog__init(mclog_ip.c_str(), nsp::toolkit::singleton<global_parameter>::instance()->get_local_mclog_port());
	syslog__init(syslog_ip.c_str(), nsp::toolkit::singleton<global_parameter>::instance()->get_local_syslog_port());
	
	return 0;
}


int main(int argc, char* argv[]) {
	log__write("agv_shell", kLogLevel_Info, kLogTarget_Stdout | kLogTarget_Filesystem, "agv_shell starting... uptime[%lld]", nsp::os::gettick());

	std::string file_name = "";
	std::string file_config_name = "";
#if WIN32
	file_name = nsp::os::get_module_directory<char>() + "/version.txt";
	file_config_name = nsp::os::get_module_directory<char>() + "/version_config.txt";
#else
	file_name = "/gzrobot/version.txt";
	file_config_name = "/gzrobot/version_config.txt";
#endif

	nsp::toolkit::singleton<global_parameter>::instance()->loadXml();
	//设置白名单
	nsp::toolkit::singleton<global_parameter>::instance()->init_white_list();
    //载入版本shell版本信息
	nsp::toolkit::singleton<global_parameter>::instance()->set_shell_version(nsp::toolkit::singleton<global_parameter>::instance()->load_shell_version(file_name));
	nsp::toolkit::singleton<global_parameter>::instance()->set_config_version(nsp::toolkit::singleton<global_parameter>::instance()->load_shell_version(file_config_name));

	std::string cmd;
	for (int i = 0; i < argc; i++)
	{
		cmd += argv[i];
	}
	if (cmd.find(long_options[2].name) != std::string::npos)
	{
		if (check_argv(argc, argv) < 0)
		{
			return 0;
		}
	}
	else
	{
		if (check_args(argc, argv) < 0) {
			loerror("agv_shell") << "agv_shell command parament error.";
			return -1;
		}
		loinfo("agv_shell") << "agvshell auto startup flag: " << nsp::toolkit::singleton<global_parameter>::instance()->get_auto_startup()
			<< " , upgrade flag:" << nsp::toolkit::singleton<global_parameter>::instance()->get_upgrade_flag();
		if (nsp::toolkit::singleton<global_parameter>::instance()->get_upgrade_flag() > 0) {
			upgrade_program();
			exit(0);
			return 0;
		}
	}

	if (agvi_init_network() < 0) {
		log__save("agv_shell", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "failed to startup network service.");
		return 1;
	}

	nsp::toolkit::singleton<agv_shell_keepalive>::instance()->init_keepalive();
	
	//run script 
	nsp::toolkit::singleton<global_parameter>::instance()->start_auto_script();
	
	//start processes 
	if (nsp::toolkit::singleton<global_parameter>::instance()->get_auto_startup() > 0) {
        start_agv_processes();
    }

    nsp::os::waitable_handle time_wait;
    time_wait.wait((uint32_t)(~0));

    return 0;
}

