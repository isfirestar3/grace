#ifndef __GRACE_ROBOT_AGVSHELL_COMMON_H__
#define __GRACE_ROBOT_AGVSHELL_COMMON_H__

#include "gzfts_api.h"
#include "posix_ifos.h"
#include "singleton.hpp"
#include <atomic>
#include <map>
#include <set>
#include <vector>
#include <thread>
#ifdef _WIN32
#include <windows.h>
#include<tlhelp32.h>
#else
#include <unistd.h>  
#include <sys/types.h> 
#include <sys/wait.h>
#endif

#pragma pack(push, 1)
typedef struct __agv_process_info {
    std::string name_; // full path
    std::string process_name_; // process name
    std::string cmd_;
    std::string ori_cmd_;
    int delay_start_;
    void* hdl_;
    unsigned pid_;
	std::thread *th_hd_;
} agv_process_info;

typedef struct __xml_parameters {
	uint16_t m_core_port_;
	uint16_t camera_port_;
	uint16_t local_udp_port_;
	uint16_t dhcp_fix_port_;
	uint16_t local_mclog_port_;
	uint16_t local_syslog_port_;
	int reset_wait_time_;
	int frimware_version_control_;
	std::string m_core_ipv4_;
	std::string camera_ipv4_;
	std::string local_ipv4_;
	std::string local_mclog_ipv4_;
	std::string local_syslog_ipv4_;
	int auto_startup_ = 0;
	int server_port_ = 4410;
	std::string dhcp_fix_ipv4_;
} xml_parameters;

typedef struct __startup_parameters {
	int upgrade_flag_ = 0;
	std::string program_name_;
	std::string program_path_;
} startup_parameters;
#pragma pack(pop)

enum thread_block_t {
    non_block_thread = 0,
    block_thread = 1,
};

typedef struct white_item_info_t
{
	std::string file_name;
#ifdef _WIN32
	void* file_handler = INVALID_HANDLE_VALUE;
#else
	void* file_handler;
#endif
}white_item_info;

class global_parameter
{
public:
	global_parameter();
	~global_parameter();
	friend class nsp::toolkit::singleton<global_parameter>;

	//process list in configure file
	static std::vector<agv_process_info > agv_process_; // key:process_name_
	//autostart scripts
	static std::vector<std::string> vct_auto_run_list_;
	
public:
	int loadXml();
	int load_processes_fxml(std::vector<agv_process_info >& process_list);
	bool init_white_list();
	int start_auto_script();
	
	void set_run_cmd(int cmd) {
		run_cmd_ = cmd;
	}
	void set_run_param(std::string& param) {
		run_param_ = param;
	}
	void set_fts_port(int port) {
		fts_port_ = port;
	}
	void set_fts_long_port(int port)
	{
		fts_long_port_ = port;
	}
	
	// set xml_parameters_
	void set_m_core_port(uint16_t port) {
		xml_parameters_.m_core_port_ = port;
	}
	void set_camera_port(uint16_t port) {
		xml_parameters_.camera_port_ = port;
	}
	void set_local_udp_port(uint16_t port) {
		xml_parameters_.local_udp_port_ = port;
	}
	void set_dhcp_fix_port(uint16_t port) {
		xml_parameters_.dhcp_fix_port_ = port;
	}
	void set_local_mclog_port(uint16_t port) {
		xml_parameters_.local_mclog_port_ = port;
	}
	void set_local_syslog_port(uint16_t port) {
		xml_parameters_.local_syslog_port_ = port;
	}
	void set_reset_wait_time(int time) {
		xml_parameters_.reset_wait_time_ = time;
	}
	void set_frimware_version_control(int frimware_version_control) {
		xml_parameters_.frimware_version_control_ = frimware_version_control;
	}
	void set_m_core_ipv4(char* ipv4) {
		xml_parameters_.m_core_ipv4_ = ipv4;
	}
	void set_camera_ipv4(char* ipv4) {
		xml_parameters_.camera_ipv4_ = ipv4;
	}
	void set_local_ipv4(char* ipv4) {
		xml_parameters_.local_ipv4_ = ipv4;
	}
	void set_local_mclog_ipv4(char* ipv4) {
		xml_parameters_.local_mclog_ipv4_ = ipv4;
	}
	void set_local_syslog_ipv4(char* ipv4) {
		xml_parameters_.local_syslog_ipv4_ = ipv4;
	}
	void set_dhcp_fix_ipv4(char* ipv4) {
		xml_parameters_.dhcp_fix_ipv4_ = ipv4;
	}
	void set_auto_startup(int flag) {
		xml_parameters_.auto_startup_ = flag;
	}
	void set_server_port(int port) {
		xml_parameters_.server_port_ = port;
	}
	// set startup_parameters_
	void set_upgrade_flag(int flag) {
		startup_parameters_.upgrade_flag_ = flag;
	}
	void set_program_name(const char* program_name) {
		startup_parameters_.program_name_ = program_name;
	}
	void set_program_path(const char* program_path) {
		startup_parameters_.program_path_ = program_path;
	}
	
	int get_run_cmd() {
		return run_cmd_;
	}
	int get_fts_port() {
		return fts_port_;
	}
	int get_fts_long_port()
	{
		return fts_long_port_;
	}
	std::string& get_run_param() {
		return run_param_;
	}
	
	// get xml_parameters_
	uint16_t get_m_core_port() {
		return xml_parameters_.m_core_port_;
	}
	uint16_t get_camera_port() {
		return xml_parameters_.camera_port_;
	}
	uint16_t get_local_udp_port() {
		return xml_parameters_.local_udp_port_;
	}
	uint16_t get_dhcp_fix_port() {
		return xml_parameters_.dhcp_fix_port_;
	}
	uint16_t get_local_mclog_port() {
		return xml_parameters_.local_mclog_port_;
	}
	uint16_t get_local_syslog_port() {
		return xml_parameters_.local_syslog_port_;
	}
	int get_reset_wait_time() {
		return xml_parameters_.reset_wait_time_;
	}
	int get_frimware_version_control() {
		return xml_parameters_.frimware_version_control_;
	}
	std::string const get_m_core_ipv4() const {
		return xml_parameters_.m_core_ipv4_;
	}
	std::string const get_camera_ipv4() const {
		return xml_parameters_.camera_ipv4_;
	}
	std::string const get_local_ipv4() const {
		return xml_parameters_.local_ipv4_;
	}
	std::string const get_local_mclog_ipv4() const {
		return xml_parameters_.local_mclog_ipv4_;
	}
	std::string const get_local_syslog_ipv4() const {
		return xml_parameters_.local_syslog_ipv4_;
	}
	std::string const get_dhcp_fix_ipv4() const {
		return xml_parameters_.dhcp_fix_ipv4_;
	}
	int get_auto_startup() {
		return xml_parameters_.auto_startup_;
	}
	int get_server_port() {
		return xml_parameters_.server_port_;
	}
	
	// get startup_parameters_
	int get_upgrade_flag() {
		return startup_parameters_.upgrade_flag_;
	}
	std::string& get_program_name() {
		return startup_parameters_.program_name_;
	} 
	std::string& get_program_path() {
		return startup_parameters_.program_path_;
	}
	void run_progess_by_get_logtype(std::string&log_path, std::set<std::string>&log_type);

	bool modify_file_lock(const bool is_lock);
	int query_file_lock();

	//shell 版本信息相关
	std::string load_shell_version(const std::string& file_name);

	void set_shell_version(const std::string& str){
		shell_version_ = str;
	}
	std::string& get_shell_version(){
		return shell_version_;
	}

	void set_config_version(const std::string& str){
		config_version = str;
	}
	std::string get_config_version(){
		return config_version;
	}

	bool check_file(std::string strfilename, const std::string &strmodify_time, const std::string &start_time, const std::string& end_time);

	std::string get_type(std::string strname);

	bool bash_command(const std::string &command, std::string &result, int max_length );

	bool bash_command(const std::string & command, const char * type = "r");
private:
	std::atomic<int> run_cmd_ {-1};
	int fts_port_ = 4411;
	int fts_long_port_ = 4412;
	std::string run_param_;
	xml_parameters xml_parameters_;
	startup_parameters startup_parameters_;
	//白名单
	std::vector<white_item_info_t> vct_white_list_;
	//shell版本
	std::string shell_version_="";
	//配置文件版本
	std::string config_version = "";
};

std::string convert_positive(const std::string& str, const char preview_cr, const  char new_cr);
std::string CK_GetProcessName(std::string& path);
std::string CK_GetProcessRelatePath(std::string& path);
/* execute a process by popen, block currenct thread 
// param 1: process name, full path
// param 2: params to process
*/
void run_process_by_popen(void* p1, void* p2);
void run_tar_by_popen( std::string cmd, std::string cmd_parament,  std::string des_file,  std::string src_file,  uint32_t lnk);
int run_copye_file_by_popen(const std::string& src_file, const std::string& des_file);

int start_process_normal(const char* file_name, const char* param, int flag=non_block_thread);
int start_process(agv_process_info& pf);
int kill_process(agv_process_info& pf);
int reboot_os();
int shutdown_os();
uint16_t get_progress_status();
// 批量执行 
int start_agv_processes();
int kill_agv_processes();
int restart_agv_processes();
int reboot_agv_syn();
int shutdown_agv_syn();
int check_args(int argc, char **argv);
int update_config_file( std::vector<agv_process_info >& replace_cfg );
#endif
