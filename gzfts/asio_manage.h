#pragma once

#include <memory>
#include <functional>
#include <mutex>
#include <map>
#include <thread>

#include "os_util.hpp"
#include "proto_definion.h"
#include "compiler.h"

class asio_data{
	nsp::proto::errorno_t err_ = nsp::proto::errorno_t::kSuccessful;
public:
	asio_data(){};
	~asio_data(){};
	asio_data& operator=(const asio_data& lref){
		if (this == &lref)return *this;
		set_err(lref.err_);
		return *this;
	}
	void set_err(nsp::proto::errorno_t err){ err_ = err; }
	int get_err()const { return err_; }
};

struct asio_block{

	asio_block() = delete;

	asio_block(const std::function<void(const void*)>&callback, const void*usr = nullptr, const int usr_len = 0){
		callback_function_ = callback;
		usr_ = usr;
		cb_ = usr_len;
		tick_ = nsp::os::gettick();
	}

	asio_block(const std::function<void(const std::shared_ptr<asio_block>&)>&callback, int hold_value){
		object_callback_function_ = callback;
		tick_ = nsp::os::gettick();
		UNREFERENCED_PARAMETER(hold_value);
	}

	~asio_block(){}

	asio_block(const asio_block&lref);
	asio_block &operator=(const asio_block&lref);
	void refresh();

	std::function<void(const void*)> callback_function_;
	std::function<void(const std::shared_ptr<asio_block>&)> object_callback_function_;

	unsigned char*ack_ = nullptr;
	int cb_ = 0;//用户上下文长度
	const void *usr_ = nullptr; // 用户上下文(如果需要存放请求ID,用于执行请求匹配，则在这里附加自己的结构化数据)
	int error_ = -1;
	uint64_t tick_;//计时器 
};

class asio_manage{

public:
	asio_manage();
	~asio_manage();

	//asio 插入
	int push(const int pkt_id, const std::shared_ptr<asio_block>& asio);
	//asio执行（查找+删除）
	int exec(const int pkt_id, const void*data);
	int exec(const int pkt_id, const void* stream, const int error, const int cb);
	//asio移除
	int remove(const int pkt_id, std::shared_ptr<asio_block>&asio);
	//asio发送数据
	int write(const int pkt_id, const std::shared_ptr<asio_block>&asio, const std::function<int()>&MyWrite);

public:
	void timeout_check();
	void timeout_notify(const std::shared_ptr<asio_block>& asio_block);

private:
	std::recursive_mutex map_lock_;
	std::map<int, std::shared_ptr<asio_block>> asio_map_;
	std::thread *timeout_thread_ = nullptr;
	nsp::os::waitable_handle asio_timeout_waiter_;
};