#include "connect_session.h"
#include "connect_manage.h"
#include "agv_shell_proto.hpp"
#include "agv_shell_define.h"
#include "recv_handler.h"


connect_session::connect_session() : nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>()
{
}

connect_session::connect_session(HTCPLINK Ink) : nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>(Ink)
{
}
connect_session::~connect_session()
{
}
uint32_t connect_session::get_link(){
	return this->lnk_;
}
void connect_session::on_connected()
{
	std::string ip = this->remote_.ipv4();
	nsp::toolkit::singleton<connect_manage>::instance()->on_connect(ip,this->lnk_,true);
}
void connect_session::on_disconnected(const HTCPLINK previous){
	std::string ip = this->remote_.ipv4();
	loerror("black_box") << ip << "disconnect network.";
	nsp::toolkit::singleton<connect_manage>::instance()->on_connect(ip, this->lnk_, false);
}
int connect_session::post_log_type_request()
{
	nsp::proto::proto_msg pkt(kAgvShellProto_GetLogType);
	return psend(&pkt);
}
int connect_session::post_log_info_request(std::string&start_time, std::string&end_time, std::vector<std::string>&log_types)
{
	nsp::proto::proto_log_condition pkt(kAgvShellProto_TimeTypeConditon);
	pkt.start_time = start_time;
	pkt.end_time = end_time;
	nsp::proto::proto_log_type pr;
	for (auto&iter : log_types){
		pr.log_type_ = iter;
		pkt.vct_log_type_.push_back(pr);
	}
	return psend(&pkt);
}
int connect_session::post_cancel_command()
{
	nsp::proto::proto_logs_cancel pkt(kAgvShellProto_Logs_Cancel);
	return psend(&pkt);
}
void connect_session::on_recvdata(const std::string &pkt) {
	alive_count = 0;
	nsp::toolkit::singleton<net_recv_manager>::instance()->async_recv_handler(pkt,
		std::bind(&connect_session::on_recv_dispath, this, std::placeholders::_1, std::placeholders::_2));
}
void connect_session::on_recv_dispath(const unsigned char* data, int & cb)
{
	nsp::proto::agv_shell_proto_head agv_sh_head;
	int size = cb;
	if (!agv_sh_head.build(data, cb)) {
		this->close();
		return;
	}

	if (agv_sh_head.op != 0xC || agv_sh_head.fn != 'C' || agv_sh_head.ln != 'K') {
		loerror("black_box") << "invalid protocol head specify.";
		this->close();
		return;
	}

	int type = agv_sh_head.type;
	switch (type) {

	case kAgvShellProto_GetLogType_ACK:
		on_getlog_type_list(nsp::proto::shared_for<nsp::proto::proto_log_type_vct>(data, size));
		break;
	case kAgvShellProto_TimeTypeConditon_ACK:
		on_get_log_info(nsp::proto::shared_for<nsp::proto::proto_logs_file_path>(data, size));
		break;
	case kAgvShellProto_Schedule_ACK:
		//get_compress_schedule(nsp::proto::shared_for<nsp::proto::proto_logs_count>(pkt.c_str(), pkt.length()));
		break;
	case kAgvShellProto_Status:
		//进程状态通知，可用着心跳计数
		recv_process_status();
		break;
	default:
		//this->close();
		break;
	}
}
//void connect_session::on_recvdata(const std::string &pkt) {
//	nsp::proto::agv_shell_proto_head agv_sh_head;
//	int cb = pkt.size();
//	if (!agv_sh_head.build((const unsigned char *)pkt.data(), cb)) {
//		this->close();
//		return;
//	}
//
//	if (agv_sh_head.op != 0xC || agv_sh_head.fn != 'C' || agv_sh_head.ln != 'K') {
//		loerror("black_box") << "invalid protocol head specify.";
//		this->close();
//		return;
//	}
//
//	int type = agv_sh_head.type;
//	switch (type) {
//	
//	case kAgvShellProto_GetLogType_ACK:
//		on_getlog_type_list(nsp::proto::shared_for<nsp::proto::proto_log_type_vct>(pkt.c_str(), pkt.length()));
//		break;
//	case kAgvShellProto_TimeTypeConditon_ACK:
//		on_get_log_info(nsp::proto::shared_for<nsp::proto::proto_logs_file_path>(pkt.c_str(), pkt.length()));
//		break;
//	case kAgvShellProto_Schedule_ACK:
//		//get_compress_schedule(nsp::proto::shared_for<nsp::proto::proto_logs_count>(pkt.c_str(), pkt.length()));
//		break;
//	case kAgvShellProto_Status:
//		//进程状态通知，可用着心跳计数
//		alive_count = 0;
//		recv_process_status();
//		break;
//	default:
//		//this->close();
//		break;
//	}
//}

void connect_session::on_getlog_type_list(const std::shared_ptr<nsp::proto::proto_log_type_vct>&data)
{
	std::vector<std::string>vct;	
	for (auto&iter : data->vct_log_type_){
		vct.push_back(iter.log_type_);
	}
	nsp::toolkit::singleton<connect_manage>::instance()->recv_log_type_vct(vct);
}

void connect_session::on_get_log_info(const std::shared_ptr<nsp::proto::proto_logs_file_path>&data)
{
	std::vector<std::string>agv_logs;
	agv_logs.assign(data->vct_log_file_name_.begin(),data->vct_log_file_name_.end());
	nsp::toolkit::singleton<connect_manage>::instance()->query_logs_file(this->lnk_, agv_logs);
}

void connect_session::recv_process_status()
{
	nsp::proto::agv_shell_proto_head pkt(kAgvShellProto_KeepAlive_ACk);
	psend(&pkt);
}

//void connect_session::get_compress_schedule(const std::shared_ptr<nsp::proto::proto_logs_count>&data)
//{
//	nsp::toolkit::singleton<connect_manage>::instance()->get_zip_schedule(data->total_count_,data->finished_count_);
//}