#include "agv_shell_session.h"
#include "net_manager.h"
#include "agv_shell_proto.hpp"
#include <limits.h>

agv_shell_session::agv_shell_session()
{

}

agv_shell_session::agv_shell_session(HTCPLINK link) : tcp_application_client(link)
{

}

agv_shell_session::~agv_shell_session()
{
	this->close();
}

void agv_shell_session::on_disconnected(const HTCPLINK previous)
{
	this->net_status_ = mn::kNetworkStatus_Closed;
	loinfo("agv_shell_interface") << "agv_shell session disconnect from " << target_ep_.to_string() << ", the link is " << previous;
}

void agv_shell_session::on_recvdata(const std::string &pkt_data)
{
	nsp::proto::proto_head head;
    int cb = pkt_data.size();
    if (!head.build((const unsigned char *) pkt_data.data(), cb)) {
		loerror("agv_shell_interface") << "agv shell build package failed,then close " << target_ep_.to_string() << " session.";
        this->close();
        return;
    }
	
	uint32_t ty = head.type_;
	if (dispatch_package(ty, pkt_data)<0) {
		loerror("agv_shell_interface") << "dispatch_package return err.";
		close();
	}

}

int agv_shell_session::dispatch_package(const uint32_t package_type, const std::string &pkt_data)
{
	//表示心跳计数
	alive_count_ = 0;

	int retval = 0;
	switch (package_type)
	{
	case PKTTYPE_AGV_SHELL_PRE_LOGIN_ACK:
		retval = recv_pre_login(nsp::proto::shared_for<nsp::proto::proto_pre_login_agv_t>(pkt_data.c_str(), pkt_data.length()));
		break;
	case PKTTYPE_AGV_SHELL_LOGIN_ACK:
		retval = recv_login_ack(nsp::proto::shared_for<nsp::proto::proto_head>(pkt_data.c_str(), pkt_data.length()));
		break;
	case PKTTYPE_AGV_SHELL_GET_PROCESS_LIST_ACK:
		retval = recv_process_list(nsp::proto::shared_for<nsp::proto::proto_process_list_reponse>(pkt_data.c_str(), pkt_data.length()));
		break;
	case PKTTYPE_AGV_SHELL_KEEPALIVE_ACK:
		retval = recv_keepalive_ack(nsp::proto::shared_for<agv::proto::proto_keepalive_ack_t>(pkt_data.c_str(), pkt_data.length()));
		break;
	case PKTTYPE_AGV_SHELL_MODIFY_FILE_MUTEX_ACK:
		retval = 0;
		break;
	case PKTTYPE_AGV_SHELL_PROCESS_COMMAND_ACK:
		retval = 0;
		break;
	default:
		loerror("agv_shell_interface") << "get an invaild type:" << package_type << " package on agv shell session.";
		retval = 0;
		break;
	}
	return retval;
}

void agv_shell_session::on_connected()
{
	link_ = this->lnk_;
	
	mn::net_status_t exp = mn::kNetworkStatus_Connecting;
	if ( net_status_.compare_exchange_strong( exp, mn::kNetworkStatus_Connected ) ) {
		loinfo("agv_shell_interface") << "target " << target_ep_.to_string() << " network status is kNetworkStatus_Connected the lnk is:" << this->lnk_;
	}else if (mn::kNetworkStatus_Ready != net_status_) {
		loinfo("agv_shell_interface") << "session wrong status when connected completed.status=" << net_status_;
	}
}

int agv_shell_session::try_connect(const nsp::tcpip::endpoint& ep)
{
	mn::net_status_t exp = mn::kNetworkStatus_Closed;
	if (net_status_.compare_exchange_strong(exp, mn::kNetworkStatus_Actived))
	{
		if (this->create() < 0)
		{
			loerror("agv_shell_interface") << "failed to create socket to,the target endpoint is:" << ep.to_string();
			net_status_ = mn::kNetworkStatus_Closed;
			this->close();
			return -1;
		}

		loinfo("agv_shell_interface") << "target " << ep.to_string() << " network status is kNetworkStatus_Actived";
		exp = mn::kNetworkStatus_Actived;
		if (net_status_.compare_exchange_strong(exp, mn::kNetworkStatus_Connecting))
		{
			target_ep_ = ep;
			loinfo("agv_shell_interface") << "target " << ep.to_string() << " network status is kNetworkStatus_Connecting";
			if (connect2(ep) < 0)
			{
				loerror("agv_shell_interface") << "failed to call ns API connect.this link will be destory.the target endpoint is " << ep.to_string() << " the link is " << this->lnk_;
				this->close();
				return -1;
			}
			
			loinfo("agv_shell_interface") << "success connecting to target host " << ep.to_string();
			return 0;
		}
	}
	loerror("agv_shell_interface") << "target " << ep.to_string() << " and link is " << this->lnk_ << " network status is " << net_status_;
	return -1;
}

int agv_shell_session::try_login() {
	mn::net_status_t exp = mn::kNetworkStatus_Ready;
	if ( !this->net_status_.compare_exchange_strong( exp, mn::kNetworkStatus_Landing ) ) {
		close();
		loerror("agv_shell_interface") << "network status must be kNetworkStatus_Ready befor post login request.";
		return -1;
	}
	loinfo("agv_shell_interface") << "network status is kNetworkStatus_Landing, ep:" << target_ep_.to_string();

	//生成原文
	unsigned char original_buffer[32];
	for ( int i = 0; i < 32; ++i ) {
		original_buffer[i] = nsp::toolkit::random( 10, UCHAR_MAX );
	}

	std::string original_str( ( const char* ) original_buffer, 32 );
	std::string encrypt_str;

	if ( nsp::toolkit::encrypt( original_str, encrypt_key_, encrypt_str ) < 0 ) {
		loerror("agv_shell_interface") << " failed post login request to service. access declined.";
		close();
		return -1;
	}

	unsigned char md5_trans[16];
	nsp::toolkit::md5( encrypt_str.c_str(), encrypt_str.length(), md5_trans );
	std::string md5_str( ( const char* ) md5_trans, 16 );

	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::proto_login_agv_t pkt( PKTTYPE_AGV_SHELL_LOGIN );
	pkt.acct_ = 0;
	pkt.access_ = 0;
	pkt.original_buffer_ = original_str;
	pkt.encrypted_md5_ = md5_str;
	pkt.head_.id_ = pkt_id;
	pkt.head_.size_ = pkt.length();

	return psend( &pkt );
}

int agv_shell_session::recv_pre_login(const std::shared_ptr<nsp::proto::proto_pre_login_agv_t>& data) {
	if (!data) {
		loerror("agv_shell_interface") << "failed to build pre login package.";
		return -1;
	}
	
	encrypt_key_ = data->key_;
	net_status_ = mn::kNetworkStatus_Ready;
	loinfo("agv_shell_interface") << "network status is kNetworkStatus_Ready, ep:" << target_ep_.to_string();
	return 0;
}

int agv_shell_session::recv_login_ack(const std::shared_ptr<nsp::proto::proto_head>& data) {
	if (!data) {
		loerror("agv_shell_interface") << "failed to build login ack package.";
		return -1;
	}
	
	if(data->err_ != 0) {
		loinfo("agv_shell_interface") << "login failure, ack err:" << data->err_;
	} else {
		net_status_ = mn::kNetworkStatus_Established;
		loinfo("agv_shell_interface") << "login ack success, network status is kNetworkStatus_Established, ep:" << target_ep_.to_string();
	}
	
	return 0;
}

int agv_shell_session::post_keeplive_pkt()
{
	nsp::proto::proto_head pkt(PKTTYPE_AGV_SHELL_KEEPALIVE);
	pkt.id_ = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	pkt.size_ = pkt.length();
	return psend(&pkt);
}

int agv_shell_session::post_get_process_list(const std::shared_ptr<motion::asio_block>&asio_ack)
{
	if( mn::kNetworkStatus_Established !=this->net_status_)
	{
		loerror("agv_shell_interface") << "current network status isn't connected,then can not post get process list.";
		return -1;
	}
	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::proto_msg_int pkt(PKTTYPE_AGV_SHELL_GET_PROCESS_LIST);
	pkt.head_.id_ = pkt_id;
	pkt.head_.size_ = pkt.length();
	return nsp::toolkit::singleton<net_manager>::instance()->write(pkt_id, [&]()->int{
		return psend(&pkt);
	}, asio_ack);
}

int agv_shell_session::post_agv_shell_process_cmd(const int process_id_all, const std::vector<std::string>& vct_parmanet, const command_agv_shell cmd)
{
	if( mn::kNetworkStatus_Established !=this->net_status_)
	{
		loerror("agv_shell_interface") << "current network status isn't connected,then can not post process command to agv_shell.";
		return -1;
	}
	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::proto_command_process pkt(PKTTYPE_AGV_SHELL_PROCESS_COMMAND);
	pkt.command_ = cmd;
	pkt.process_id_all_ = process_id_all;
	for (const auto& iter : vct_parmanet)
	{
		pkt.list_param_.push_back(iter);
	}
	pkt.head_.id_ = pkt_id;
	pkt.head_.size_ = pkt.length();
	return psend(&pkt);
}


int agv_shell_session::recv_keepalive_ack(const std::shared_ptr<agv::proto::proto_keepalive_ack_t>& data) 
{
	if (!data) {
		loerror("agv_shell_interface") << "failed to build recvice keepalive ack package.";
		return -1;
	}
	
	// 根据进程状态信息判断进程状态 
	std::lock_guard<decltype(process_mutex_)> lock_g(process_mutex_);
	vct_process_status_.clear();
	if (process_list_.vct_process_.size() == 0)
	{
		loinfo("agv_shell_interface") << "agv shell session recive process list is empty,the link is " << this->lnk_;
		return 0;
	}
	process_status p_status;
	for (const auto& iter : process_list_.vct_process_)
	{
		p_status.process_id = iter.process_id;
		p_status.status = agv_process::stop;
		for(auto& it : data->process_list) {
			if(0 != it.pid && iter.process_name == it.name) {
				p_status.status = agv_process::running;
				break;
			}
		}
		vct_process_status_.push_back(p_status);
	}
	return 0;
}

int agv_shell_session::recv_process_list(const std::shared_ptr<nsp::proto::proto_process_list_reponse>& data)
{
	if (!data) {
		loerror("agv_shell_interface") << "failed to build recvice process list package.";
		return -1;
	}
	std::lock_guard<decltype(process_mutex_)> lock_g(process_mutex_);
	//清除当前缓存
	process_list_.vct_process_.clear();
	process_list_.set_err(nsp::proto::errorno_t::kSuccessful);
	for (const auto & iter : data->process_list_)
	{
		process_info p_info;
		p_info.process_id = iter.process_id_;
		p_info.process_name = iter.process_name_;
		process_list_.vct_process_.push_back(p_info);
	}
	return nsp::toolkit::singleton<net_manager>::instance()->exec(data->head_.id_, (char *)&process_list_);
}

void agv_shell_session::get_agv_shell_process_status(std::vector<process_status>& vct_p)
{
	std::lock_guard<decltype(process_mutex_)> lock_g(process_mutex_);
	vct_p = vct_process_status_;
}

void agv_shell_session::clear_process_list()
{
	std::lock_guard<decltype(process_mutex_)> lock_g(process_mutex_);
	vct_process_status_.clear();
}

int agv_shell_session::netstatus(mn::net_status_t flag)
{
	return flag == net_status_ ? 1 : 0;
}
