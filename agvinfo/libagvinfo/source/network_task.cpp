#include "network_task.h"
#include "network.h"
#include "agvinfo_proto.hpp"
#include "proto_unpacket.h"
#include "log.h"

network_task::network_task(std::string data, std::function<int(nsp::proto::proto_interface *)> psend)
:packet_(data),
psend_(psend)
{
}

network_task::~network_task()
{
}

int network_task::on_task()
{
	nsp::proto::unpackt unpacage(packet_);
	if (kAgvInfoProto_update_notify == unpacage.ack_type()){
		loinfo("libagvinfo") << "on recv kAgvInfoProto_update_notify";
		nsp::toolkit::singleton<network>::instance()->notify();
	}
	else if (kAgvInfoProto_heart_beat == unpacage.ack_type()){
		loinfo("libagvinfo") << "on recv kAgvInfoProto_heart_beat";
		nsp::proto::proto_head head_(kAgvInfoProto_heart_beat_ack, unpacage.pkt_id(), 0, 0, kAgvInfoProto_OK);
		if (psend_){
			psend_(&head_);
		}
	}
	else if (unpacage.ack_type() != kAgvInfoProto_heart_beat_ack){
		loinfo("libagvinfo") << "on recv kAgvInfoProto_heart_beat_ack";
		nsp::toolkit::singleton<network>::instance()->exec(unpacage.pkt_id(), (void *)packet_.c_str(), packet_.length());
	}
	
	return 0;
}