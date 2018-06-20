#include "tcp_session.h"
#include "tcp_manager.h"

#include "argvs.h"
#include "../common/agvinfo_proto.hpp"
#include "os_util.hpp"
#include "common_data.h"
#include "libagvinfo.h"

#include <algorithm>

tcp_session::tcp_session() {
	heart_beat_tickt = nsp::os::gettick();
}

tcp_session::tcp_session(HTCPLINK lnk) : tcp_application_client(lnk) {
	heart_beat_tickt = nsp::os::gettick();
}

tcp_session::~tcp_session() {
}
// 心跳报文处理
int tcp_session::on_heart_beat(const std::string &pkt)
{
	nsp::proto::proto_head req;
	int cb = pkt.size();
	if (req.build((const unsigned char *)pkt.data(), cb) < 0) {
		return -1;
	}

	nsp::proto::proto_head res;
	res.type_ = kAgvInfoProto_heart_beat_ack;
	res.id_ = req.id_;

	return psend(&res);
}

// 获取全部小车信息 
int tcp_session::on_get_agvinfo(const std::string &pkt)
{
	struct proto_req_agvinfo req;
	int cb = pkt.size();
	if (req.build((const unsigned char *)pkt.data(), cb) < 0) {
		return -1;
	}

	try {
		proto_req_agvinfo res;
		res.head = req.head;

		std::recursive_mutex &r_mutex = nsp::toolkit::singleton<tcp_manager>::instance()->get_mutex_agvinfo();
		// 读写小车信息之前加锁
		std::lock_guard<std::recursive_mutex> guard(r_mutex);
		map_agvinfo& map_xml_agvinfo =
			nsp::toolkit::singleton<tcp_manager>::instance()->get_xml_agvinfo();

		res.load_menthod = req.load_menthod;
		loinfo(MODULE_NAME) << "kAgvInfoProto_get_agvinfo deal";
		for (auto iter = map_xml_agvinfo.begin(); iter != map_xml_agvinfo.end(); iter++) {
			if (req.load_menthod == LAM_Server && iter->second.status == AS_UNKNOWN) {
				// LAM_Server时只给客户端xml文件中有的车子信息,xml文件中没有的车子状态为AS_UNKNOWN 
				continue;
			}
			res.info.push_back(iter->second);
		}
		return psend(&res);
    } catch (...) {
		return -1;
	}

	return 0;
}

// 全部小车信息 
int tcp_session::on_set_agvinfo(const std::string &pkt)
{
	int result = -1;
	struct proto_req_agvinfo req;
	int cb = pkt.size();
	if (req.build((const unsigned char *)pkt.data(), cb) < 0) {
		return -1;
	}

	try {
		proto_get_agvinfo res;
		res.head = req.head;

		std::recursive_mutex &r_mutex = nsp::toolkit::singleton<tcp_manager>::instance()->get_mutex_agvinfo();
		// 读写小车信息之前加锁
		std::lock_guard<std::recursive_mutex> guard(r_mutex);
		map_agvinfo& map_xml_agvinfo =
			nsp::toolkit::singleton<tcp_manager>::instance()->get_xml_agvinfo();

		loinfo(MODULE_NAME) << "kAgvInfoProto_set_agvinfo deal";
		// 更新前先清空 
		map_agvinfo tmp_info;
		map_xml_agvinfo.swap(tmp_info);
		// 拷贝所有agvinfo至map_xml_agvinfo中
		for (auto iter = req.info.begin(); iter != req.info.end(); iter++) {
			std::transform(iter->hwaddr.begin(), iter->hwaddr.end(),
				iter->hwaddr.begin(), ::toupper);
			if (INVALID_VHID == iter->vhid) {
				// 客户端送来的信息没有ID则内存和文件都不保存 
				continue;
			} else {
				// 客户端送来的信息有ID则把状态变为OFFLINE 
				iter->status = AS_OFFLINE;
			}
			map_xml_agvinfo.insert(std::make_pair(iter->hwaddr, *iter));
		}
		// 写入xml文件 
		nsp::toolkit::singleton<tcp_manager>::instance()->save_agvinfo_xml();
		res.load_menthod = req.load_menthod;
		result = psend(&res);
	}
	catch (...) {
		return -1;
	}

	if (result == 0) {
		// 更新成功，通知其他客户端 
		nsp::toolkit::singleton<tcp_manager>::instance()->set_notify_agvinfo_update();
	}
	return result;
}

int tcp_session::on_get_agvdetail(const std::string &pkt)
{
	struct proto_req_agvdetail req;
	int cb = pkt.size();
	if (req.build((const unsigned char *)pkt.data(), cb) < 0) {
		return -1;
	}

	try {
		proto_req_agvdetail res;
		res.head = req.head;

		std::recursive_mutex &r_mutex = nsp::toolkit::singleton<tcp_manager>::instance()->get_mutex_agvinfo();
		// 读写小车信息之前加锁
		std::lock_guard<std::recursive_mutex> guard(r_mutex);
		map_agvinfo& map_xml_agvinfo =
			nsp::toolkit::singleton<tcp_manager>::instance()->get_xml_agvinfo();

		loinfo(MODULE_NAME) << "kAgvInfoProto_get_agvdetail deal";
		for (auto iter = map_xml_agvinfo.begin(); iter != map_xml_agvinfo.end(); iter++) {
			if (iter->second.vhid == req.detail.vhid) {
				res.detail.vhid = iter->second.vhid;
				// 拷贝所有agvattribute至res中
				std::copy(iter->second.attrs.begin(), iter->second.attrs.end(), std::back_inserter(res.detail.attrs));
			}
		}
		res.load_menthod = req.load_menthod;
		return psend(&res);
	}
	catch (...) {
		return -1;
	}
	return 0;
}

int tcp_session::on_set_agvdetail(const std::string &pkt)
{
	int result = -1;
	struct proto_req_agvdetail req;
	int cb = pkt.size();
	if (req.build((const unsigned char *)pkt.data(), cb) < 0) {
		return -1;
	}

	try {
		proto_req_agvdetail res;
		res.head = req.head;

		std::recursive_mutex &r_mutex = nsp::toolkit::singleton<tcp_manager>::instance()->get_mutex_agvinfo();
		// 读写小车信息之前加锁
		std::lock_guard<std::recursive_mutex> guard(r_mutex);
		map_agvinfo& map_xml_agvinfo =
			nsp::toolkit::singleton<tcp_manager>::instance()->get_xml_agvinfo();
		
		loinfo(MODULE_NAME) << "kAgvInfoProto_set_agvdetail deal";
		for (auto iter = map_xml_agvinfo.begin(); iter != map_xml_agvinfo.end(); iter++) {
			if (iter->second.vhid == req.detail.vhid) {
				// 先清空 
				std::vector<proto_agvattribute> tmp_attr;
				iter->second.attrs.swap(tmp_attr);
				// 再从req拷贝所有agvattribute至内存中
				std::copy(req.detail.attrs.begin(), req.detail.attrs.end(), std::back_inserter(iter->second.attrs));
				// ID唯一，找到更新后不用再找了 
				break;
			}
		}
		// 写入xml文件 
		nsp::toolkit::singleton<tcp_manager>::instance()->save_agvinfo_xml();
		res.load_menthod = req.load_menthod;
		result = psend(&res);
	}
	catch (...) {
		return -1;
	}

	if (result == 0) {
		// 更新成功，通知其他客户端 
		nsp::toolkit::singleton<tcp_manager>::instance()->set_notify_agvinfo_update();
	}
	return 0;
}
void tcp_session::on_recvdata(const std::string &pkt)
{
	nsp::proto::proto_head head;
	int cb = pkt.size();
	if (!head.build((const unsigned char *)pkt.data(), cb)) {
		this->close();
		return;
	}

	// 记录收到报文时间 
	heart_beat_tickt = nsp::os::gettick();

	int type = head.type_;
	int result = -1;
	switch (type) {
	case kAgvInfoProto_heart_beat:
		result = on_heart_beat(pkt);
		break;
	case kAgvInfoProto_get_agvinfo:
		result = on_get_agvinfo(pkt);
		break;
	case kAgvInfoProto_set_agvinfo:
		result = on_set_agvinfo(pkt);
		break;
	case kAgvInfoProto_get_agvdetail:
		result = on_get_agvdetail(pkt);
		break;
	case kAgvInfoProto_set_agvdetail:
		result = on_set_agvdetail(pkt);
		break;
	case kAgvInfoProto_heart_beat_ack:
		result = 0;
		break;
	default:
		loerror(MODULE_NAME) << "tcp server get an invalid protocol head specify.";
		break;
	}

	if (result < 0)	{
		lowarn(MODULE_NAME) << "failed to handler receive package,then close this link.";
		this->close();
	}
}

int tcp_session::post_notify_agvinfo_update()
{
	loinfo(MODULE_NAME) << "post_notify_agvinfo_update. " << remote().ipv4();
	nsp::proto::proto_head pkt;
	pkt.type_ = kAgvInfoProto_update_notify;
	return psend(&pkt);
}

int tcp_session::check_heart_beat()
{
	uint64_t now_tick = nsp::os::gettick() - heart_beat_tickt;
	static uint32_t count = 0;

	if (now_tick >= TIME_SEND_HEART_BEAT && now_tick < TIME_OUT_HEART_BEAT) {
		// 距离上次收到报文，大于30秒且小于90秒
		if (count % 30) {
			// 每30秒发送一个心跳检测报文
			nsp::proto::proto_head pkt;
			pkt.type_ = kAgvInfoProto_heart_beat;
			pkt.id_ = ++count;
			loinfo(MODULE_NAME) << remote().ipv4() << " check_heart_beat.";
			return psend(&pkt);
		}
		count++;
	}

	if (now_tick > TIME_OUT_HEART_BEAT) {
		// 距离上次收到报文，大于90秒的话，认为连接有问题，断开连接 
		lowarn(MODULE_NAME) << remote().ipv4() << " will be disconnected.";
		return -1;
	}

	return 0;
}

void tcp_session::on_disconnected(const HTCPLINK previous)
{
	loinfo(MODULE_NAME) << remote().ipv4() << " disconnected.";
}

int tcp_session::on_established()
{
	loinfo(MODULE_NAME) << remote().ipv4() << " established.";
	return 0;
}
