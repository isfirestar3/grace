#pragma once

#include "tcp_session.h"
#include "singleton.hpp"
#include "common_data.h"
#include "../common/agvinfo_proto.hpp"
#include "os_util.hpp"

#include <memory>
#include <thread>

// map中的key为MacAddress 
typedef std::multimap<std::string, proto_agvinfo> map_agvinfo;
#define INVALID_VHID (~0)
#define DEFAULT_MTPORT (4409)
#define DEFAULT_SHPORT (4410)
#define DEFAULT_FTSPORT (4411)

class tcp_manager{
	tcp_manager();
	~tcp_manager();
	friend class nsp::toolkit::singleton<tcp_manager>;

public:
	int tcp_listen(const std::string& ipv4, const uint16_t port);
	// 从xml文件中读取agvinfo到vec_xml_agvinfo
	int load_agvinfo_xml();
	// 把map_xml_agvinfo写入到xml文件 
	int save_agvinfo_xml();
	// 获得数据map_xml_agvinfo
	map_agvinfo& get_xml_agvinfo();
	// 获得数据map_xml_agvinfo需要的锁 
	std::recursive_mutex & get_mutex_agvinfo();

	// 心跳检测
	void check_heart_beat();
	// 设置通知客户端agvinfo发生变化标志(两秒左右后通知到客户端) 
	void set_notify_agvinfo_update();
	// 检测是否需要通知客户端agvinfo发生变化了 
	int check_notify_agvinfo_update();
	// 通知客户端agvinfo发生变化了 
	void post_notify_agvinfo_update();
	// 接收到udp消息，需要插入一台车信息 
	void insert_macinfo(const robot_mac_info_t& mac_info);
	// 接收到udp消息，需要更新一台车信息 
	void update_macinfo(const robot_mac_info_t& mac_info);
	// 接收到udp消息，需要删除一台车信息 
	void delete_macinfo(const robot_mac_info_t& mac_info);


private:
	// 获取xml文件的crc32校验码 
	uint32_t get_xml_crc32();
	// 定时监测xml文件改变线程
	void check_thread();
	// 检查xml文件改
	void check_xml_update();
	// 初始化操作: 初始化crc32校验码，初次读取xml文件，启动监测线程
	void initlizatin();
	// 反初始化操作：等待监测线程退出 
	void uinit();

private:
	std::shared_ptr<nsp::tcpip::tcp_application_service<tcp_session>> server_ptr_;

	nsp::os::waitable_handle wait_check_;
	std::thread* check_th_ = nullptr;
	std::atomic<bool> exit_th_{ false };

	// 需要通知客户端数据更新标志 0:不需要通知 1:需要通知
	int need_notify;
	// xml文件的CRC32校验码 
	uint32_t crc32_xml_old;
	uint32_t crc32_xml_new;
	// 访问map_xml_agvinfo时使用(同时只能一个客户端读写操作)
	std::recursive_mutex r_mutex_agvinfo;
	// agv全量信息(Key为MacAddress) 
	map_agvinfo map_xml_agvinfo;
};
