#pragma once

#include <memory>
#include <functional>
#include <mutex>
#include <map>
#include <thread>

#include "os_util.hpp"
#include "asio.hpp"

class asio_manage{

public:
	asio_manage();
	~asio_manage();

	//asio 插入
	int push(const int pkt_id, const std::shared_ptr<motion::asio_block>& asio);
	//asio执行（查找+删除）
	int exec(const int pkt_id, const void*data);
	int exec(const int pkt_id, const void* stream, const int error, const int cb);
	//asio移除
	int remove(const int pkt_id, std::shared_ptr<motion::asio_block>&asio);
	//asio发送数据
	int write(const int pkt_id, const std::shared_ptr<motion::asio_block>&asio, const std::function<int()>&MyWrite);

public:
	void timeout_check();
	void timeout_notify(const std::shared_ptr<motion::asio_block>& asio_block);

private:
	std::recursive_mutex map_lock_;
	std::map<int, std::shared_ptr<motion::asio_block>> asio_map_;
	std::thread *timeout_thread_ = nullptr;
	nsp::os::waitable_handle asio_timeout_waiter_;
};