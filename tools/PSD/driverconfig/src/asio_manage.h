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

	//asio执行（查找+删除）
	int exec(const uint32_t pkt_id, const void*data);

	int exec_data_forward(const uint32_t cmd, const void*data);

	int exec(const uint32_t pkt_id, const void* stream, const int error, const int cb);

	//asio发送数据
	int write(const uint32_t pkt_id, const std::shared_ptr<motion::asio_block>&asio, const std::function<int()>&MyWrite);

	int write_data_forward(const uint32_t cmd, const std::shared_ptr<motion::asio_block>&asio, const std::function<int()>&MyWrite);

	int post_message(const uint8_t cmd, const std::shared_ptr<motion::asio_block>&asio, const std::function<int()>&MyWrite);

	int exec_message(const uint8_t cmd, const void*data);
private:
	int exec_ex(const uint32_t pkt_id, const void*data);
	//asio 插入
	int push(const uint32_t pkt_id, const std::shared_ptr<motion::asio_block>& asio);
	//asio移除
	int remove(const uint32_t pkt_id, std::shared_ptr<motion::asio_block>&asio);
public:
	void timeout_check();
	void timeout_notify(const std::shared_ptr<motion::asio_block>& asio_block);

private:
	std::recursive_mutex map_lock_;
	std::recursive_mutex map_message_lock_;
	std::map<uint32_t, std::shared_ptr<motion::asio_block>> asio_map_;
	std::map<uint8_t, std::shared_ptr<motion::asio_block>> assio_message_map_;
	std::thread *timeout_thread_ = nullptr;
	nsp::os::waitable_handle asio_timeout_waiter_;
};