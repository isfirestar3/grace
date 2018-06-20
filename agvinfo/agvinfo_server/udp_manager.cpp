#include "udp_manager.h"
#include "tcp_manager.h"
#include "argvs.h"

udp_manager::udp_manager()
{
	initlizatin();
}

udp_manager::~udp_manager()
{
	uinit();
}

int udp_manager::udp_listen(const std::string& ipv4, const uint16_t port)
{
	try
	{
		share_session_ = std::make_shared<udp_session>();
	}
	catch (...)
	{
		return -1;
	}
	if (share_session_)
	{
		nsp::tcpip::endpoint ep(ipv4.c_str(), port);
		if (share_session_->create(ep) < 0)
		{
			loerror(MODULE_NAME) << "failed to bind udp:" << ipv4 << ":" << port << " listen.";
			return -1;
		}
		loinfo(MODULE_NAME) << "success to bind udp listen:" << port;
		return 0;
	}
	return -1;
}

void udp_manager::initlizatin()
{
	try
	{
		check_th_ = new std::thread(std::bind(&udp_manager::check_timeout, this));
	}
	catch (...)
	{
		loerror(MODULE_NAME) << "failed to start check timeout thread.";
	}
}

void udp_manager::uinit()
{
	exit_th_ = true;
	wait_check_.sig();
	if (check_th_)
	{
		if (check_th_->joinable())
		{
			check_th_->join();
		}
		delete check_th_;
		check_th_ = nullptr;
	}
	wait_check_.reset();
}

void udp_manager::check_timeout()
{
	static const uint32_t CHECK_INTERVAL = 2000;
	static const uint32_t CHECK_TIMEOUT_COUNT = 3;
	while (wait_check_.wait(CHECK_INTERVAL))
	{
		if (exit_th_)break;

		{
			std::lock_guard<decltype(r_mutex_)> lock(r_mutex_);
			auto iter_begin = map_mac_info_.begin();
			while (iter_begin != map_mac_info_.end())
			{
				if (iter_begin->second.pkt_timeout_count < CHECK_TIMEOUT_COUNT)
				{
					iter_begin->second.pkt_timeout_count++;
					iter_begin++;
				}
				else
				{
					//通知上层取数据
					nsp::toolkit::singleton<tcp_manager>::instance()->delete_macinfo(iter_begin->second);
					//此时超时，做删除该数据记录操作
					iter_begin = map_mac_info_.erase(iter_begin);
				}
			}
		}
	}
}

void udp_manager::save_mac_info(const robot_mac_info& m_info)
{
	std::lock_guard<decltype(r_mutex_)> lock(r_mutex_);
	if (map_mac_info_.count(m_info.mac_addr) == 0)
	{
		//未找到,直接插入该条数据
		map_mac_info_.insert(std::make_pair(m_info.mac_addr, m_info));
		//通知上层
		nsp::toolkit::singleton<tcp_manager>::instance()->insert_macinfo(m_info);
		return;
	}
	auto range = map_mac_info_.equal_range(m_info.mac_addr);
	bool is_find = false;
	for (auto iter = range.first; iter != range.second; iter++)
	{
		if (iter->second.ipv4 == m_info.ipv4 && 
			iter->second.fts_port == m_info.fts_port && 
			iter->second.shell_port == m_info.shell_port)
		{
			//找到该条记录，更新计数信息
			iter->second.pkt_timeout_count = m_info.pkt_timeout_count;
			nsp::toolkit::singleton<tcp_manager>::instance()->update_macinfo(m_info);
			is_find = true;
			break;
		}
	}
	
	if (!is_find)
	{
		//找到此MAC地址下没有该端口的记录，则添加一条
		map_mac_info_.insert(std::make_pair(m_info.mac_addr, m_info));
		//通知上层
		nsp::toolkit::singleton<tcp_manager>::instance()->insert_macinfo(m_info);
	}
	return;
}

void udp_manager::get_mac_all(std::vector<robot_mac_info>& vct_info)
{
	std::lock_guard<decltype(r_mutex_)> lock(r_mutex_);
	for (const auto iter : map_mac_info_)
	{
		robot_mac_info r_info;
		r_info.fts_port = iter.second.fts_port;
		r_info.ipv4 = iter.second.ipv4;
		r_info.mac_addr = iter.second.mac_addr;
		r_info.shell_port = iter.second.shell_port;
		vct_info.push_back(r_info);
	}
}