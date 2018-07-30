#ifndef SYS_INFO_20180103
#define SYS_INFO_20180103

#include "posix_ifos.h"
#include "nisdef.h"
#include <vector>
#include <string>

enum NET_ADAPTER_TYPE{
	NCF_Unkown = 0,
	NCF_VIRTUAL = 0x01,				//组件是个虚拟适配器
	NCF_SOFTWARE_ENUMERATED = 0x02,	//组件是一个软件模拟的适配器
	NCF_PHYSICAL = 0x4,				//组件是一个物理适配器
	NCF_HIDDEN  = 0x08,             //组件不显示用户接口
	NCF_NO_SERVICE = 0x10,			//组件没有相关的服务(设备驱动程序)
	NCF_NOT_USER_REMOVABLE = 0x20,	//不能被用户删除(例如，通过控制面板或设备管理器)
	NCF_MULTIPORT_INSTANCED_ADAPTER = 0x40//说明组件有多个端口
};

#pragma pack(push, 1)
typedef struct cpu_stat_ {  
    char name[32];      
    unsigned int user; 
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
    unsigned int lowait; 
    unsigned int irq;
    unsigned int softirq;
} cpu_stat;  

struct cpu_info {
	uint64_t hz;
	std::string name;
};

struct disk_info {
	uint64_t total_size;
	uint64_t used_size;
};

struct net_iostat {
	char net_name[32];
	uint64_t rcv_bytes;
	uint64_t rcv_packets;
	uint32_t rcv_errs;
	uint32_t rcv_drop;
	uint32_t rcv_fifo;
	uint64_t send_bytes;
	uint64_t send_packets;
	uint32_t send_errs;
	uint32_t send_drop;
	uint32_t send_fifo;
};
#pragma pack(pop)

class sys_info {
	std::string mac_addr_;
	sys_memory_t sys_mem_;
	std::vector<cpu_info> cpu_info_;
	uint32_t cpu_used_percentage_;
	disk_info disk_info_;
	uint64_t uptime_;
	std::string uname_info_;
	std::string host_name_;
	cpu_stat last_cpu_stat_;
	net_iostat *net_iostat_;
	std::string network_name_;
	
public:
	sys_info();
	~sys_info();

public:
	int init();
	
	inline const std::string& get_mac_addr() { return mac_addr_;};
	int post_sysinfo_changed(const uint32_t link, const unsigned char *buffer, int cb);
	int post_sysinfo_fixed(const uint32_t link, const unsigned char *buffer, int cb);

private:
	int is_virtual_adapter(const char *pAdapterName, NET_ADAPTER_TYPE& ty);
	
	void get_mac();
	void get_devmem();
	void get_cpu_info();
	void get_cpuoccupy(cpu_stat &cpust);
	void get_cpu_usage();
	void get_iostat();
	void get_version();
	
	void print_statinfo();
	
};

#endif
