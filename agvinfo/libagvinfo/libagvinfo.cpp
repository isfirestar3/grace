#include "libagvinfo.h"
#include "malloc.h"
#include "agv_manager.h"


interface_format(int) connect_agvinfo_server(const char *inet, uint16_t port)
{
	return nsp::toolkit::singleton<agv_manager>::instance()->connect_server(inet, port);;
}

interface_format(void) disconnect_from_agvinfo()
{
	return nsp::toolkit::singleton<agv_manager>::instance()->disconnect();
}

interface_format(int) load_agvinfo(struct agv_info **agvs, enum load_agvinfo_method method)
{
	return nsp::toolkit::singleton<agv_manager>::instance()->load_agvinfo(agvs, method);;
}

interface_format(int) cover_agvinfo(const struct agv_info *agvs, enum load_agvinfo_method method)
{
	return nsp::toolkit::singleton<agv_manager>::instance()->cover_agvinfo(agvs, method);
}

interface_format(int) get_agvdetail(uint32_t vhid, struct agv_detail **detail, enum load_agvinfo_method method)
{
	return nsp::toolkit::singleton<agv_manager>::instance()->get_agvdetail(vhid, detail, method);
}

interface_format(int) set_agvdetail(uint32_t vhid, const struct agv_detail *detail, enum load_agvinfo_method method)
{
	return nsp::toolkit::singleton<agv_manager>::instance()->set_agvdetail(vhid, detail, method);
}

interface_format(void) release_agvinfo(struct agv_info *agvs)
{
	return nsp::toolkit::singleton<agv_manager>::instance()->release_agvinfo(agvs);
}

interface_format(void) release_agvdetail(struct agv_detail *detail)
{
	return nsp::toolkit::singleton<agv_manager>::instance()->release_agvdetail(detail);
}

interface_format(void) about_something_changed(void(*notify)())
{
	return nsp::toolkit::singleton<agv_manager>::instance()->about_something_changed(notify);
}