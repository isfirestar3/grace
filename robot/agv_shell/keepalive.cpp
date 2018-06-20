#include "keepalive.h"
#include "proto_agv_msg.h"
#include "agv_shell_common.h"
#include "agv_shell_define.h"
#include "agv_shell_session.h"
#include "agv_shell_server.h"
#include "dhcp_udp_session.h"
#include "udp_client_manager.h"

#ifdef _WIN32
#include <Windows.h>
#include <TLHELP32.H>
#include <iostream>
#endif


agv_shell_keepalive::agv_shell_keepalive() : keepalive_waiter_(0) {

}



agv_shell_keepalive::~agv_shell_keepalive() {
    end_keepalive();
}

int agv_shell_keepalive::init_keepalive() {
    try {
        if (!th_keepalive_) {
            th_keepalive_ = new std::thread( std::bind( &agv_shell_keepalive::keepalive, this ) );
        }
    } catch (...) {
        loerror("agv_shell") << "keepalive create failure ";
        return -1;
    }
    
    return 0;
}

void agv_shell_keepalive::keepalive() {
	nsp::proto::proto_process_status pkt(kAgvShellProto_Status);
	pkt.agv_shell_reply_ = kAgvShellReply_OK;
	int alive_count = 0;
    while (keepalive_waiter_.wait(1000) > 0) {
		pkt.status_ = get_progress_status();
		time_t now = time(NULL);
		pkt.robot_time_ = now;
		alive_count++;
		// 向每一个 app 发送状态 
		nsp::toolkit::singleton<agv_shell_server>::instance()->post_notify_all(pkt);

		//每3秒钟向vcu发送一次心跳包；同时向客户端发送白名单文件是否被锁住状态
		if (alive_count % 3 == 0)
		{
			alive_count = 0;
			keep_alive_vcu();
			
			// send local info to fix ip when dhcp start 
			if (nsp::toolkit::singleton<dhcp_udp_client_manager>::instance()->post_local_info_request() < 0) {
				loerror("agv_shell") << "keep alive send local info to fix ip failed.";
			}

			nsp::toolkit::singleton<agv_shell_server>::instance()->post_file_mutex(nsp::toolkit::singleton<global_parameter>::instance()->query_file_lock());
		}
    }
    keepalive_waiter_.reset();
}

void agv_shell_keepalive::end_keepalive() {
    keepalive_waiter_.sig();
    if (th_keepalive_) {
        if (th_keepalive_->joinable()) {
            th_keepalive_->join();
            delete th_keepalive_;
            th_keepalive_ = nullptr;
        }
    }
    keepalive_waiter_.reset();
}


void agv_shell_keepalive::keep_alive_vcu() {
	nsp::tcpip::endpoint vcu_endpoint = nsp::toolkit::singleton<udp_client_manager>::instance()->get_m_core_endpoint();
	int res = nsp::toolkit::singleton<udp_client_manager>::instance()->post_vcu_keep_alive_request(vcu_endpoint);
	if (res < 0) {
		loerror("agv_shell") << "keep alive send package to vcu failed.";
    }
}

