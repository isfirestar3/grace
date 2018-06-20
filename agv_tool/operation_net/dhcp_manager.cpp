#include "dhcp_manager.h"
#include <io.h>
#include "rw_xml_file.h"
#include "err_msg.h"
#include "log.h"
#include "libagvinfo.h"
#include "proto_interface.h"

#define NET_WINDOW_SIZE     1
#define DHCP_CHANGED		9

void update_notification()
{
	nsp::toolkit::singleton<dhcp_manager>::instance()->network_callback_notify();
}

dhcp_manager::dhcp_manager()
{
	about_something_changed(update_notification);
}

dhcp_manager::~dhcp_manager()
{
	
}

int dhcp_manager::connect_to_dhcp()
{

	std::string root = nsp::os::get_module_directory<char>();
	//读取xml文件配置信息
	std::string local_xml_file = root + "/AGVSetting.xml";
	std::string ipv4, port;
	if (_access(local_xml_file.c_str(), 0) != -1) {
		rw_xml_file rw;
		rw.read_attribute(local_xml_file, "dhcp_addr", "ipv4", ipv4);
		rw.read_attribute(local_xml_file, "dhcp_addr", "port", port);
	}
	int port_i = atoi(port.c_str());
	//调用接口，连接dhcp服务器
	if (connect_agvinfo_server(ipv4.c_str(), port_i) < 0)
	{
		return -1;
	}
	return 0;
}

int dhcp_manager::disconnect_dhcp()
{
	disconnect_from_agvinfo();
	return 0;
}

void dhcp_manager::set_network_callback(void(__stdcall* func)(const char* robot_ip, const int type, const int error, const char* data_str, const int cb))
{
	network_callback_ = func;
}

void dhcp_manager::network_callback_notify()
{
	if (network_callback_)
	{
		char msg[4] = { 0 };
		network_callback_("", DHCP_CHANGED, 0, msg, sizeof(msg));
	}
}

int dhcp_manager::post_sync_without_id(const int type, const void* str, const int cb, void **reponse, int *out_cb)
{
	agv_info *info = nullptr;
	if (load_agvinfo(&info, load_agvinfo_method::LAM_Server) < 0)
	{
		return -1;
	}
	nsp::api::proto_agvinfo_vec vct_agvinfo;
	agv_info *ptr_info = info;
	while (ptr_info)
	{
		if (ptr_info->status == AS_ONLINE)
		{
			nsp::api::proto_dhcp_info tmp;
			tmp.ipv4_ = ptr_info->inet;
			tmp.agv_port_ = ptr_info->shport;
			tmp.fts_port_ = ptr_info->ftsport;
			tmp.mac_addr_ = ptr_info->hwaddr;
			tmp.vhid_ = ptr_info->vhid;
			vct_agvinfo.vct_mac_.push_back(tmp);
		}
		ptr_info = ptr_info->next;
	}
	release_agvinfo(info);
	*reponse = new unsigned char[vct_agvinfo.length()];
	vct_agvinfo.serialize((unsigned char*)*reponse);
	*out_cb = (int)vct_agvinfo.length();

	return 0;
}