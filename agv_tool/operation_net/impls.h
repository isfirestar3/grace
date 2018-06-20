#pragma once

#include <stdint.h>

#ifdef _USRDLL
#define METH dllexport
#else
#define METH dllimport
#endif


#ifdef WIN32
#define EXP(type) extern "C" _declspec (METH) type __stdcall
#else
#define EXP(type) extern "C"
#endif

//文件状态
enum file_status{
	network_error = -4,				//网络错误
	file_not_exist = -3,			//文件不存在
	file_read_err = -2,				//读失败
	file_write_err = -1,			//写失败
	transmitting = 0,				//传输中
	complete = 1,					//完成
	wait = 2						//待上传
};

enum transfer_block_size {
	block_4kb = 1,					//块大小
	block_8kb,
	block_16kb,
	block_32kb,
	block_64kb
};

//上传文件时，任务组进度回调的结构体
struct task_group_info{
	uint64_t	task_id_;
	char		target_ep_[24];
	uint32_t	task_status_;
	uint32_t	task_step_;
};

//上传文件时，任务组中文件具体详细信息
struct task_file_detail_info {
	char file_name_[512];
	char file_modify_time_[32];
	uint64_t file_size_;
	uint32_t file_status_;
	uint32_t file_step_;
};

enum FIRMWARE_SOFTWARE_TYPE
{
	BOOT_SOFTWARE = 0,
	MAIN_SOFTWARE,
	USER_SOFTWARE,
	CAMERA_SOFTWARE,
	COMPARE_SOFTWARE,		//此项表示读取刚刚下发的文件
	CAN_CUSTOM_SOFTWARE		//can协议自主驱动
};

enum VCU_MESSAGE
{
	VCU_Type = 0,
	VCU_Version,
	VCU_CPU
};

//与界面通信的type类型枚举
enum Interface_Package{
	Proto_Agv_Process_List,				//获取agv_shell进程表
	Proto_Agv_Process_Status,			//获取agv_shell进程状态
	Proto_A_Core_Info,					//更新A核信息（系统时间，VCU使能状态）
	Proto_UpdateNTPServer,				//更新NTP服务地址
	Proto_RunScript,					//执行脚本
	Proto_Cmd_List,						//获取进程管理详细信息
	Proto_UpdateCmd,					//更新agv_shell各进程启动属性
	Proto_VCUEnable,					//使能VCU控制板
	Proto_GetDHCPList,					//获取DHCP列表
	Proto_NotifyDHCP,					//DHCP更新通知
	Proto_ModifyMutex,					//修改文件锁状态
	Proto_Update_Progress,				//进度更新
	Proto_BackupDemarcate				//修改标定文件时备份该文件
};

//agv_shell升级，获取M核信息状态
enum FRIMWARE_STATUS 
{
	kNormal = 0,			//正常
	kBusy,					//操作中
	kRestarting,			//正在重启
	kTranfmiting,			//正在下发文件
	kTranfmited,			//下发文件成功

	kFailReadVCUInfo = -100, //读取VCU信息失败
	kFailRestart,			//重启失败
	kFailUpdate,			//升级失败
	kFailDownload,			//下载文件失败
	kFailCompare,			//文件比对失败
	kRequestTimeout,		//请求超时
	kFailStartForward,		//can协议启用转发失败
	kFailCanInitRom			//can协议下初始化ROM失败
};

enum ProcessStatus
{
	kStop=0,
	kRunning=1
};

enum file_type
{
	folder = 0,				//为文件夹
	file =1					//为文件
};

//该枚举用于业务层区分fts函数调用类型
enum Fts_Function_Type
{
	TY_PULL_FILE,			//拉文件
	TY_PUSH_FILE,			//推文件
	TY_KEY_TO_UPGRADE,		//一键升级
	TY_M_CORE_FILE,			//发送M核文件
	TY_KEY_BACKUP			//一键备份
};

//初始化网络
//第一个回调函数中参数为robot_id
//第二个回调函数中参数一：robot_id,参数二：状态(0:正常，-1已经存在)，参数三：主机名
EXP(int)  init_net(void(__stdcall *callback_link_closed)(int32_t), 
	void(__stdcall *callback_link_connect)(int32_t, int32_t, const char* host_name),
	const char* ip_port);

//连接网络
EXP(int)  connect_host(uint32_t robot_id, const char* str_ep);

//断开连接
EXP(int)  disconnect(uint32_t robot_id);

//获取目标端当前指定根目录下所有的文件夹
EXP(int)  get_listdir(const char* ip_port, const char* data_stream, void** output_stream, int& output_cb);

//获取当前一层目录结构
EXP(int) get_current_catalog(const char* ft_endpoint, const char* input_catalog, char** data_ptr, int* cb);

//注册下拉文件进度回调函数
EXP(int)  regist_pull_callback(void(__stdcall *callback_func)(const int status, const int steps));

//注册上传文件进度回调函数
EXP(int)  regist_push_callback(void(__stdcall *callback_func)(const char* chr, const int cb));

//获取文件push上传详细列表
EXP(int) get_push_files_detail(const uint64_t task_id, void** data_ptr, int& cb);

//释放内存
EXP(void) free_detail(void* ptr);

//上传文件至远端文件夹下
EXP(int)  push_files_to_remote(const char* ip_port, const char* push_files, const int push_cb );

//获取文件或文件夹crc校验码
EXP(int)  get_file_crc(const char* input_file, unsigned int* crc_value);

//删除远端文件
EXP(int) delete_remote_files(const char* ip_port, const char* str_files, const int cb);

//对于push失败的文件，需要重新发送
EXP(int) repeat_push_files(const uint64_t, const char* str_files, const int cb);

//恢复指定机器人出厂设置
EXP(void) restore_factory_setting(const char* ip_port, void(__stdcall * callback_step)(int32_t step, int32_t status, const char* ip));

//从指定远端拉取某几个特定文件
EXP(int) pull_files_from_remote(const char* ip_port, const char* str_files, const int cb, const char* save_path);

//更新配置文件中指定文件夹内容
EXP(void) update_folder_config();

//设置发送块大小
EXP(void) set_block_size(const transfer_block_size block_size);

//发送下位机进程操作,开启进程，结束进程，重启进程，重启工控机
EXP(int) post_cmd_agv_process(const char* robot_id_all, const char* process_id_all, const int cmd);

//取消pull操作
EXP(int) cancel_pull_opreate();

//请求获取M核固件，相机的型号，版本，cpu信息 ，兼容请求获取can协议下自主驱动信号，版本信息
EXP(int) get_m_camera_info_request(const char* robot_id_all,const FIRMWARE_SOFTWARE_TYPE frimware_type, const char* target_endpoint, 
	const int node_id, const int can_bus);

//注册获取M核固件相机信息的回调函数
EXP(void) regist_m_core_info_callback(void(__stdcall * function)(const char* ip, const VCU_MESSAGE vcu_type, const char* msg));

//发送M核文件，相机文件
EXP(int) push_m_camera_file(const char* ip_port, const FIRMWARE_SOFTWARE_TYPE type, const char* file_path, const char* target_endpoint);

//注册 发送/接收 M核文件，相机文件时，文件的传输状态，传输进度
EXP(void) regist_m_core_transfer_callback(void(__stdcall* function)(const char* ip, const FRIMWARE_STATUS frimware_type, const int step));

//请求重启M核
EXP(int) restart_m_core(const char* robot_id_all,const FIRMWARE_SOFTWARE_TYPE frimware_type, const char* target_endpoint);

//请求获取M核文件,保存在当前目录下的m_core文件夹内
EXP(int) pull_m_camera_file(const char* robot_id_all, const FIRMWARE_SOFTWARE_TYPE frimware_type, const int frimware_length,
	const char* local_path, const char* target_endpoint);

//升级can 自主驱动固件文件
EXP(int) push_can_bin_file(const char* ip_port, const uint32_t node_id, const uint32_t serial_type, const char* file_path, const char* target_endpoint);

//注册进程回调
EXP(void) regist_process_status_callback(void(__stdcall* function)(const char* endpoint, ProcessStatus status));

//更新AGVShell,亦可实现一键更新功能
EXP(int) update_agv_shell(const char* robot_id_list, const char* shell_path);

//注册网络数据包回调
EXP(void) regist_netowrk_callback(void(__stdcall *callback_func)(const char* robot_ip, const int type, const int error, const char* str, const int cb));

//获取进程状态映射表
EXP(void) get_agv_process_table();

//异步发送网络字节序
EXP(int) post_async_package(const int robot_id, const int type, const void* str, const int cb);

//同步发送网络字节序
EXP(int) post_sync_package(const int robot_id, const int type, const void* str, const int cb, void **_ack_msg, int *_ack_len);

//是否打开DHCP功能
EXP(int) is_open_dhcp(bool status);

//异步发送网络字节序,不使用id
EXP(int) post_sync_without_id(const int type, const void* str, const int cb, void **_ack_msg, int *_ack_len);

//一键备份车上需要备份的配置文件
EXP(int) post_backup_file(const void* str, const int cb);
