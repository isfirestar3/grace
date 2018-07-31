#include "agv_shell_common.h"
#include "agv_shell_define.h"
#include "agv_shell_server.h"
#include "cmd_parse.h"
#include "dhcp_udp_session.h"
#include "log.h"
#include "logger.h"
#include "udp_client_manager.h"
#include "argv.h"

extern
int mclog__init(const char *ip, uint16_t port);
extern
void mclog__uninit();
//extern
//int syslog__init(const char *ip, uint16_t port);
//extern
//void syslog__uninit();

void load_config() {
	//load agv_shell.xml 
	nsp::toolkit::singleton<global_parameter>::instance()->loadXml();
	//lock white files
	nsp::toolkit::singleton<global_parameter>::instance()->init_white_list();

	//load version info
	nsp::toolkit::singleton<global_parameter>::instance()->load_shell_version();
	nsp::toolkit::singleton<global_parameter>::instance()->load_config_version();
	// load ntp server address
	nsp::toolkit::singleton<global_parameter>::instance()->load_ntp_server();
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
	nsp::toolkit::singleton<udp_client_manager>::instance()->set_dhcp_fix_ep(nsp::toolkit::singleton<global_parameter>::instance()->get_dhcp_fix_ipv4(), nsp::toolkit::singleton<global_parameter>::instance()->get_dhcp_fix_port());
	//start udp network
	nsp::toolkit::singleton<udp_client_manager>::instance()->init_network();
	
	//start mclog && vcu syslog msg udp network
	std::string mclog_ip = nsp::toolkit::singleton<global_parameter>::instance()->get_local_mclog_ipv4();
	//std::string syslog_ip = nsp::toolkit::singleton<global_parameter>::instance()->get_local_syslog_ipv4();
	mclog__init(mclog_ip.c_str(), nsp::toolkit::singleton<global_parameter>::instance()->get_local_mclog_port());
	//syslog__init(syslog_ip.c_str(), nsp::toolkit::singleton<global_parameter>::instance()->get_local_syslog_port());
	
	return 0;
}


int main(int argc, char* argv[]) {
	log__write("agv_shell", kLogLevel_Info, kLogTarget_Stdout | kLogTarget_Filesystem, "agv_shell starting... uptime[%lld]", nsp::os::gettick());

	load_config();
    
	if (check_argv(argc, argv) < 0) {
		loerror("agv_shell") << "agv_shell command parament error.";
		return -1;
	}
	
	if (agvi_init_network() < 0) {
		log__save("agv_shell", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "failed to startup network service.");
		return 1;
	}
	
	//run script 
	loinfo("agv_shell") << "agvshell auto startup flag: " << nsp::toolkit::singleton<global_parameter>::instance()->get_auto_startup();
	nsp::toolkit::singleton<global_parameter>::instance()->start_auto_script();
	
	//start processes 
	if (nsp::toolkit::singleton<global_parameter>::instance()->get_auto_startup() > 0) {
        start_agv_processes();
    }

    nsp::os::waitable_handle time_wait;
    time_wait.wait((uint32_t)(~0));

    return 0;
}

