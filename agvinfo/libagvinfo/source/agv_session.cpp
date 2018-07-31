#include "agv_session.h"
#include "network_manager.h"
#include "network.h"
#include "libagvinfo.h"

agv_session::agv_session(){

}

agv_session::agv_session(std::function<void (std::string )> callback)
:disconnect_callback_(callback)
{
	refresh_tick();
}

agv_session::~agv_session()
{
}

void agv_session::on_recvdata(const std::string &packet)
{
	refresh_tick();
	if (packet.length() > 0){
		nsp::toolkit::singleton<network_manager>::instance()->push_task(packet, std::bind(&agv_session::psend, this, std::placeholders::_1));
	}
}

int agv_session::keepalive(){
	loinfo("libagvinfo") << "keepalive";
	nsp::proto::proto_heart_beat heart_packet;
	heart_packet.head_.id_ = nsp::toolkit::singleton<network>::instance()->pid();
	return psend(&heart_packet);
}

int agv_session::load_agvinfo(std::shared_ptr<agv_motion::motion_ack> motion,int method )
{
	loinfo("libagvinfo") << "load agvinfo";
	uint32_t pid = nsp::toolkit::singleton<network>::instance()->pid();
	proto_req_agvinfo agvinfo;
	agvinfo.head.id_ = pid;
	agvinfo.head.type_ = kAgvInfoProto_get_agvinfo;
	agvinfo.load_menthod = (method == LAM_Server) ? kAgvInfoProto_load_xml : kAgvInfoProto_load_realtime;
	return nsp::toolkit::singleton<network>::instance()->write(pid, motion,[&]()->int{
		return psend(&agvinfo);
	});
	return 0;
}

int agv_session::set_agvinfo(std::shared_ptr<agv_motion::motion_ack> motion, int method, std::vector<agv_info_inner>& vecagvinfo)
{
	loinfo("libagvinfo") << "set agvinfo";
	uint32_t pid = nsp::toolkit::singleton<network>::instance()->pid();

	proto_set_agvinfo req_agvinfo;
	for (auto iter : vecagvinfo){
		proto_agvinfo agvinfo;
		agvinfo.vhid = iter.id_;
		agvinfo.vhtype = iter.type_;
		agvinfo.mtport = iter.mtport_;
		agvinfo.shport = iter.shport_;
		agvinfo.status = iter.status_;
		agvinfo.ftsport = iter.ftsport_;
		agvinfo.inet = iter.inet_;
		agvinfo.hwaddr = iter.hwaddr_;
		for (auto attr : iter.vec_attrs_){
			proto_agvattribute proto_attr;
			proto_attr.name = attr.name_;
			proto_attr.describe = attr.describe_;
			agvinfo.attrs.push_back(proto_attr);
		}
		req_agvinfo.info.push_back(agvinfo);
	}

	req_agvinfo.head.id_ = pid;
	/*agvinfo.load_menthod = (method == LAM_Server) ? kAgvInfoProto_load_xml : kAgvInfoProto_load_realtime;*/
	return nsp::toolkit::singleton<network>::instance()->write(pid, motion, [&]()->int{
		return psend(&req_agvinfo);
	});
	return 0;
}

int agv_session::get_agvdetail(std::shared_ptr<agv_motion::motion_ack> motion, int agvid ,int method)
{
	loinfo("libagvinfo") << "get agvinfo detail";
	uint32_t pid = nsp::toolkit::singleton<network>::instance()->pid();
	proto_get_agvdetail agvdetail;
	agvdetail.head.id_ = pid;
	agvdetail.detail.vhid = agvid;
	agvdetail.load_menthod = (method == LAM_Server) ? kAgvInfoProto_load_xml : kAgvInfoProto_load_realtime;
	return nsp::toolkit::singleton<network>::instance()->write(pid, motion, [&]()->int{
		return psend(&agvdetail);
	});
	return 0;
} 

int agv_session::set_agvdetail(std::shared_ptr<agv_motion::motion_ack> motion, int agvid, const std::vector<agv_attribute_inner> & vec_attr)
{
	uint32_t pid = nsp::toolkit::singleton<network>::instance()->pid();
	proto_set_agvdetail agvdetail;
	agvdetail.head.id_ = pid;
	agvdetail.detail.vhid = agvid;
	for (auto iter : vec_attr){
		proto_agvattribute attr;
		attr.name = iter.name_;
		attr.describe = iter.describe_;
		agvdetail.detail.attrs.push_back(attr);
	}
	return nsp::toolkit::singleton<network>::instance()->write(pid, motion, [&]()->int{
		return psend(&agvdetail);
	});
	return 0;
}

void agv_session::on_disconnected(const HTCPLINK previous){
	loinfo("libagvinfo") << "disconnect link" << previous;
	std::string ipstr = remote().to_string();
	if (disconnect_callback_){
		disconnect_callback_(ipstr);
	}
}