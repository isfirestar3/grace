#include "task.h"
#include "agv_shell_common.h"
#include "agv_shell_define.h"
#include "agv_shell_proto.hpp"
#include "agv_shell_server.h"
#include "log.h"
#include "public.h"
#include "udp_client_manager.h"

extern std::vector<agv_process_info > agv_process_;

/*----------------------------------------------------------------query_keepalive_status_task----------------------------------------------------------------*/
query_keepalive_status_task::query_keepalive_status_task(HTCPLINK link, uint32_t id) : base_task() {
	this->link_ = link;
	this->pkt_id_ = id;
}

query_keepalive_status_task::~query_keepalive_status_task() {
	;
}

void query_keepalive_status_task::on_task() {
	vcu_endpoint_ = nsp::toolkit::singleton<udp_client_manager>::instance()->get_m_core_endpoint();

	motion::asio_data asio_data_;
	nsp::os::waitable_handle waiter(0);
	recv_vcu_data recv_data;
    int vcu_reply_status = 0;
	int err = 0;
    int res = -1; //查询 2 次
    
    for (int i=0;i<2;++i) {
        res = nsp::toolkit::singleton<udp_client_manager>::instance()->post_query_vcu_keep_alive_request(vcu_endpoint_, std::make_shared<motion::asio_block>([&](const void* data){
            if (!data){
				err = -EFAULT;
                waiter.sig();
                return;
            }
            asio_data_ = *(motion::asio_data*)data;
            if (asio_data_.get_err() < 0){
                waiter.sig();
                return;
            }
            recv_data = *(recv_vcu_data*)data;
            waiter.sig();
        }));
        if (res < 0){
            loerror("agv_shell") << "failed to post query vcu keepalive status request.";
            err = -EIO;
			break;
        }
        waiter.wait();
		waiter.reset();
        if (asio_data_.get_err() == nsp::proto::errorno_t::kRequestTimeout) {
            loerror("agv_shell") << "time out error while query vcu keep alive status request.";
            vcu_reply_status = AgvShellKeepaliveStatus::AgvShellKeepalive_Unknow;
			err = nsp::proto::errorno_t::kRequestTimeout;
        } else if (asio_data_.get_err() != nsp::proto::errorno_t::kSuccessful) {
            loerror("agv_shell") << "vcu reply error while query vcu keep alive status request.";
            vcu_reply_status = AgvShellKeepaliveStatus::AgvShellKeepalive_Unknow;
			err = -1;
        } else {
            loinfo("agv_shell") << "vcu reply keep alive status data len:" << recv_data.data_length_;
            loinfo("agv_shell") << "vcu reply keep alive status:" << recv_data.data_context_; // TODO: 可能需要转换 
            vcu_reply_status = AgvShellKeepaliveStatus::AgvShellKeepalive_Start;
			err = 0;
            break;
        }
    }
	
	nsp::toolkit::singleton<agv_shell_server>::instance()->post_vcu_keep_alive_status(this->link_, pkt_id_, PKTTYPE_AGV_SHELL_QUERY_VCU_ALIVE_STATUS_ACK, vcu_reply_status, err);
}

/*----------------------------------------------------------------set_keepalive_status_task----------------------------------------------------------------*/
set_keepalive_status_task::set_keepalive_status_task(HTCPLINK link, uint32_t id, int status) : base_task() {
	this->link_ = link;
	this->pkt_id_ = id;
	this->status_ = status;
}

set_keepalive_status_task::~set_keepalive_status_task() {
	;
}
void set_keepalive_status_task::on_task() {
	vcu_endpoint_ = nsp::toolkit::singleton<udp_client_manager>::instance()->get_m_core_endpoint();
	
	motion::asio_data asio_data_;
	nsp::os::waitable_handle waiter(0);
	recv_vcu_data recv_data;
    //TODO: 设置 2 次
    int vcu_reply_status = 0;
	int err = 0;
    int res = -1; //查询 2 次
    
    for (int i=0;i<2;++i) {
        res = nsp::toolkit::singleton<udp_client_manager>::instance()->post_set_vcu_keep_alive_request(vcu_endpoint_, status_, std::make_shared<motion::asio_block>([&](const void* data){
            if (!data){
				err = -EFAULT;
                waiter.sig();
                return;
            }
            asio_data_ = *(motion::asio_data*)data;
            if (asio_data_.get_err() < 0){
                waiter.sig();
                return;
            }
            recv_data = *(recv_vcu_data*)data;
            waiter.sig();
        }));
        if (res < 0){
            loerror("agv_shell") << "failed to post set vcu keepalive status request.";
            err = -EIO;
			break;
        }
        waiter.wait();
		waiter.reset();
		
        if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful) {
            loinfo("agv_shell") << "vcu reply set keep alive status data len:" << recv_data.data_length_;
            loinfo("agv_shell") << "vcu reply set keep alive status:" << recv_data.data_context_;
            vcu_reply_status = AgvShellKeepaliveReply::AgvShellKeepalive_Successful;
			err = 0;
            break;
        } else if (asio_data_.get_err() == nsp::proto::errorno_t::kRequestTimeout) {
            loerror("agv_shell") << "time out error while query vcu keep alive status request.";
            vcu_reply_status = AgvShellKeepaliveStatus::AgvShellKeepalive_Unknow;
			err = nsp::proto::errorno_t::kRequestTimeout;
        } else {
            loerror("agv_shell") << "get an error while set vcu keep alive status request.";
            vcu_reply_status = AgvShellKeepaliveReply::AgvShellKeepalive_Failure;
			err = -1;
        }
    }
    
    nsp::toolkit::singleton<agv_shell_server>::instance()->post_vcu_keep_alive_status(this->link_, pkt_id_, PKTTYPE_AGV_SHELL_SET_VCU_ALIVE_STATUS_ACK, vcu_reply_status, err);
}

/*----------------------------------------------------------------set_keepalive_status_task----------------------------------------------------------------*/
deal_process_cmd_task::deal_process_cmd_task(int cmd, int process_id) {
	this->command_ = cmd;
	this->process_id_all_ = process_id;
}

deal_process_cmd_task::~deal_process_cmd_task() {
	;
}

void deal_process_cmd_task::add_cmd_param(std::string& param) {
	this->vec_cmd_param_.push_back(param);
}

int deal_process_cmd_task::process_task() {
	int ret = 0;
	
	if (AgvShellCmd::ReBoot == command_) {
        reboot_os();
        return ret;
    } else if (AgvShellCmd::ShutDown == command_) {
        shutdown_os();
        return ret;
    }
    int process_id_list = process_id_all_;
	if (process_id_list <= 0) {
		ret = -EINVAL;
		loinfo("agv_shell")<< "deal_process_cmd_task receive process_id_all_ < 0.";
		return ret;
	}
	loinfo("agv_shell") << "deal_process_cmd_task get a task for handler process,now handler it...";
	size_t pos = 0, v_size = vec_cmd_param_.size();
	std::thread start_process_th;
	auto ap = global_parameter::agv_process_.begin();// 注意顺序 
	while( ap != global_parameter::agv_process_.end() ) {
		if ( (process_id_list & 0x01) ) {
			if (pos < v_size && !vec_cmd_param_[pos].empty()) {
				ap->cmd_ = vec_cmd_param_[pos];
			} else {
				ap->cmd_ = ap->ori_cmd_;
			}
			lotrace("agv_shell")<< "process:" << ap->process_name_ << ", receive cmd:" << command_ << ", param:" << ap->cmd_;
			switch (command_)
			{
			case AgvShellCmd::Start:
#ifndef _WIN32
                // 已经是启动状态的进程不做处理 
                if ( ap->pid_ > 0 && 0 == kill(ap->pid_, 0) ) {
                    lotrace("agv_shell")<< "the process " << ap->process_name_ << " is already started.";
                    break;
                }
#else
				if (ap->hdl_ != NULL)
				{
					loinfo("agv_shell") << "the process " << ap->process_name_ << " is already started.";
					break;
				}
#endif
                start_process( *ap );
				break;
			case AgvShellCmd::Stop:
				kill_process( *ap );
				break;
			case AgvShellCmd::ReStart:
				kill_process( *ap );
				start_process( *ap );
				break;
			default:
				loerror("agv_shell") << "receive process command unsupport, " << command_;
				break;
			}
		}
		process_id_list = (process_id_list>>1);
		++ap;
		++pos;
	}
	
	return ret;
}
