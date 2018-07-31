#include "agv_shell_session.h"
#include "net_manager.h"
#include "agv_shell_proto.hpp"

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
	std::shared_ptr<nsp::proto::agv_shell_proto_head> head = nsp::proto::shared_for<nsp::proto::agv_shell_proto_head>(pkt_data.c_str(),
		nsp::proto::agv_shell_proto_head::type_length());

	if (!head) {
		loerror("agv_shell_interface") << "agv shell session ger an error head package.";
		close();
		return;
	}
	uint8_t ty = head->type;
	if (dispatch_package(ty, pkt_data)<0) {
		close();
	}

}

int agv_shell_session::dispatch_package(const uint8_t package_type, const std::string &pkt_data)
{
	//表示心跳计数
	alive_count_ = 0;

	int retval = 0;
	switch (package_type)
	{
	case kAgvShellProto_Process_List_Ack:
		retval = recv_process_list(nsp::proto::shared_for<nsp::proto::proto_process_list_reponse>(pkt_data.c_str(), pkt_data.length()),
			pkt_data.substr(nsp::proto::agv_shell_proto_head::type_length()));
		break;
	case kAgvShellProto_Status:
		retval = recv_process_status(nsp::proto::shared_for<nsp::proto::proto_process_status>(pkt_data.c_str(), pkt_data.length()));
		break;
	case kAgvShellProto_FileMutexStatus:
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
	this->net_status_ = mn::kNetworkStatus_Connected;
	loinfo("agv_shell_interface") << "target " << target_ep_.to_string() << " network status is kNetworkStatus_Connected the lnk is:" << this->lnk_;
}

int agv_shell_session::post_get_process_list(const std::shared_ptr<motion::asio_block>&asio_ack)
{
	if( mn::kNetworkStatus_Connected !=this->net_status_)
	{
		loerror("agv_shell_interface") << "current network status isn't connected,then can not post get process list.";
		return -1;
	}
	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::proto_msg_int pkt(kAgvShellProto_Process_List);
	pkt.msg_int_ = pkt_id;
	return nsp::toolkit::singleton<net_manager>::instance()->write(pkt_id, [&]()->int{
		return psend(&pkt);
	}, asio_ack);
}

int agv_shell_session::post_agv_shell_process_cmd(const int process_id_all, const std::vector<std::string>& vct_parmanet, const command_agv_shell cmd)
{
	if( mn::kNetworkStatus_Connected !=this->net_status_)
	{
		loerror("agv_shell_interface") << "current network status isn't connected,then can not post process command to agv_shell.";
		return -1;
	}
	nsp::proto::proto_command_process pkt(kAgvShellProto_ProcessCmd);
	pkt.command_ = cmd;
	pkt.process_id_all_ = process_id_all;
	for (const auto& iter : vct_parmanet)
	{
		pkt.list_param_.push_back(iter);
	}
	return psend(&pkt);
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
			if (connect(ep) < 0)
			{
				loerror("agv_shell_interface") << "failed to call ns API connect.this link will be destory.the target endpoint is " << ep.to_string() << " the link is " << this->lnk_;
				this->close();
				return -1;
			}
			link_ = this->lnk_;
			loinfo("agv_shell_interface") << "agv shell interface success connect to target host " << ep.to_string() << " ,this link is:" << this->lnk_;

			return 0;
		}
	}
	loerror("agv_shell_interface") << "target " << ep.to_string() << " and link is " << this->lnk_ << " network status is " << net_status_;
	return -1;
}

int agv_shell_session::recv_process_list(const std::shared_ptr<nsp::proto::proto_process_list_reponse>& data, const std::string& pkt_data)
{
	if (!data)
	{
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
	return nsp::toolkit::singleton<net_manager>::instance()->exec(data->pkt_id_, (char *)&process_list_);
}

int agv_shell_session::recv_process_status(const std::shared_ptr<nsp::proto::proto_process_status>&data)
{
	nsp::proto::agv_shell_proto_head alive_pkt(kAgvShellProto_KeepAlive_ACk);
	psend(&alive_pkt);

	if (!data)
	{
		loerror("agv_shell_interface") << "failed to build recvice process status package.";
		return -1;
	}
	//解析数据
	std::lock_guard<decltype(process_mutex_)> lock_g(process_mutex_);
	vct_process_status_.clear();
	if (process_list_.vct_process_.size() == 0)
	{
		loinfo("agv_shell_interface") << "agv shell session recive process list is empty,the link is " << this->lnk_;
		return 0;
	}
	int status_data = data->status_;
	for (const auto& iter : process_list_.vct_process_)
	{
		process_status p_status;
		p_status.process_id = iter.process_id;
		p_status.status = (agv_process)((status_data & (1 << iter.process_id)) != 0 ? 1 : 0);
		vct_process_status_.push_back(p_status);
	}
	return 0;
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

int agv_shell_session::is_network_conntected()
{
	return mn::kNetworkStatus_Connected == net_status_ ? 0 : -1;
}