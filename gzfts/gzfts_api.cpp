#include "gzfts_api.h"
#include "receiver_manager.h"
#include "sender_manager.h"
#include "long_lnk_sender.h"
#include "long_lnk_receiver.h"

FTS_EXP(int) fts_create_receiver(const char *ipaddr_local, uint16_t port_local, const bool is_long_lnk){
	if (!is_long_lnk)
	{
		return nsp::toolkit::singleton<receiver_manager>::instance()->fts_create_receiver(ipaddr_local, port_local);
	}
	else
	{
		return nsp::toolkit::singleton<long_lnk_receiver>::instance()->fts_create_receiver(ipaddr_local, port_local);
	}
}

FTS_EXP(void) fts_destory_receiver(const char *ipaddr_local, uint16_t port_local, const bool is_long_lnk){
	if (!is_long_lnk)
	{
		nsp::toolkit::singleton<receiver_manager>::instance()->fts_destory_receiver(ipaddr_local, port_local);
	}
	else
	{
		nsp::toolkit::singleton<long_lnk_receiver>::instance()->fts_destory_receiver(ipaddr_local, port_local);
	}
}


FTS_EXP(int) fts_change_configure(const struct fts_parameter *param){
	nsp::toolkit::singleton<receiver_manager>::instance()->fts_change_configure(param);
	nsp::toolkit::singleton<sender_manager>::instance()->fts_change_configure(param);
	nsp::toolkit::singleton<long_lnk_sender>::instance()->fts_change_configure(param);
	nsp::toolkit::singleton<long_lnk_receiver>::instance()->fts_change_configure(param);
	return 0;
}

FTS_EXP(int) fts_listdir(const char *ipaddr_to, uint16_t port_to, const char*input_catalog, char **dir, int *dircbs){
	return nsp::toolkit::singleton<sender_manager>::instance()->fts_listdir(ipaddr_to, port_to, input_catalog, dir, dircbs);
}

FTS_EXP(void) fts_freedir(char *dir){
	nsp::toolkit::singleton<sender_manager>::instance()->fts_freedir(dir);
}

FTS_EXP(int) fts_push(const char *ipaddr_to, uint16_t port_to, const char *path_local, const char *path_remote, int overwrite)
{
	return nsp::toolkit::singleton<sender_manager>::instance()->fts_push(ipaddr_to, port_to, path_local, path_remote, overwrite);
}

FTS_EXP(int) fts_pull(const char *ipaddr_from, uint16_t port_from, const char *path_local, const char *path_remote, int overwrite)
{
	return nsp::toolkit::singleton<sender_manager>::instance()->fts_pull_file(ipaddr_from, port_from, path_local, path_remote, overwrite);
}

FTS_EXP(int) fts_pull_long(const char *ipaddr_from, uint16_t port_from, const char *path_local, const char *path_remote, int overwrite)
{
	return nsp::toolkit::singleton<long_lnk_sender>::instance()->fts_pull_long(ipaddr_from, port_from, path_local, path_remote, overwrite);
}

FTS_EXP(void) fts_cancel(const char *ipaddr, uint16_t port, const bool is_long_lnk)
{
	if (!is_long_lnk)
	{
		nsp::toolkit::singleton<sender_manager>::instance()->fts_cancel(ipaddr, port);
	}
	else
	{
		nsp::toolkit::singleton<long_lnk_sender>::instance()->fts_cancel(ipaddr, port);
	}
}

FTS_EXP(int) fts_delete_file(const char *ipaddr_to, uint16_t port_to, const char *path_remote)
{
	return nsp::toolkit::singleton<sender_manager>::instance()->fts_delete_file(ipaddr_to,port_to,path_remote);
}

FTS_EXP(int) fts_catalog_info(const char* ipaddr_to, uint16_t port_to, const char*input_catalog, char **output_info, int *output_cb)
{
	return nsp::toolkit::singleton<sender_manager>::instance()->fts_catalog_info_req(ipaddr_to, port_to, input_catalog, output_info, output_cb);
}