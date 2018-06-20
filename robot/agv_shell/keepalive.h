#ifndef __GRACE_ROBOT_AGVSHELL_KEEPALIVE_H__
#define __GRACE_ROBOT_AGVSHELL_KEEPALIVE_H__

#include <atomic>
#include <map>
#include <mutex>
#include <thread>
#include "application_network_framwork.hpp"
#include "os_util.hpp"
#include "singleton.hpp"
#include "icom/nisdef.h"
class agv_shell_session;
class agv_shell_keepalive {
private:
    agv_shell_keepalive();
    ~agv_shell_keepalive();
    friend class nsp::toolkit::singleton<agv_shell_keepalive>;
    
private:
    nsp::os::waitable_handle keepalive_waiter_;
    std::thread * th_keepalive_ = nullptr;
    
private:
    void keepalive();
    void end_keepalive();
	void keep_alive_vcu();
public:
    int init_keepalive();
    
private:
	//所有进程集合
	std::vector<std::string> vct_process_all_;
};
#endif
