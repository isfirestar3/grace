#ifndef SYS_INFO_20180103
#define SYS_INFO_20180103

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

class sys_info
{
public:
	sys_info();
	~sys_info();

public:
	std::string get_mac_addr();

private:
	bool is_virtual_adapter(const char *pAdapterName, NET_ADAPTER_TYPE& ty);
	
};

#endif
