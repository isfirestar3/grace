#include "asio_manage.h"
#include <Windows.h>
#include "log.h"

//////////////////////////////////////////asio 异步管理////////////////////////////////////////////////

asio_manage::asio_manage() :asio_timeout_waiter_(0){

}

asio_manage::~asio_manage(){
	if (timeout_thread_){
		if (timeout_thread_->joinable()){
			asio_timeout_waiter_.sig();
			timeout_thread_->join();
		}
		delete timeout_thread_;
		timeout_thread_ = nullptr;
	}

	asio_timeout_waiter_.reset();
}

//asio 插入
int asio_manage::push(const uint32_t pkt_id, const std::shared_ptr<motion::asio_block>& asio){
	std::lock_guard<decltype(map_lock_)> locker(map_lock_);
	if (asio_map_.find(pkt_id) != asio_map_.end()){
#ifdef _DEBUG
		nspwarn << "can not push new asio_block into map,there is already exist pkt_id.";
#endif
		return -1;
	}
	asio_map_.insert(std::make_pair(pkt_id, asio));
	//启动线程
	if (!timeout_thread_){
		timeout_thread_ = new std::thread(std::bind(&asio_manage::timeout_check, this));
	}
	return 0;
}

//asio执行（查找+删除）
int asio_manage::exec_ex(const uint32_t pkt_id, const void*data){
	std::shared_ptr<motion::asio_block> asio;
	{
		std::lock_guard<decltype(map_lock_)> locker(map_lock_);
		auto iter = asio_map_.find(pkt_id);
		if (iter == asio_map_.end()){
#ifdef _DEBUG
			nsperror << "can not operate exec function,can not find id:" << pkt_id << " package.";
#endif
			return -1;
		}

		asio = iter->second;
		asio_map_.erase(iter);
	}
	if (asio->callback_function_){
		asio->callback_function_(data);
		return 0;
	}
#ifdef _DEBUG
	nsperror << "can not callback fuction,there isn't callback_function_ in asio_block.";
#endif
	return -1;
}

int asio_manage::exec_data_forward(const uint32_t cmd, const void*data){
	uint32_t ack_id = (cmd & 0xff) << 24;
	return exec_ex(ack_id, data);
}

int asio_manage::exec(const uint32_t pkt_id, const void*data){

	uint32_t ack_id = (pkt_id & 0x00ffffff);
	return exec_ex(ack_id, data);
}

int asio_manage::exec(const uint32_t pkt_id, const void* stream, const int error, const int cb){
	std::shared_ptr<motion::asio_block> asio;
	{
		std::lock_guard<decltype(map_lock_)> locker(map_lock_);
		auto iter = asio_map_.find(pkt_id);
		if (iter == asio_map_.end()){
#ifdef _DEBUG
			nsperror << "can not operate exec function,can not find id:" << pkt_id << " package.";
#endif
			return -1;
		}

		asio = iter->second;
		asio_map_.erase(iter);
	}
	if (asio->object_callback_function_){
		asio->ack_ = (unsigned char *)stream;
		asio->cb_ = cb;
		asio->error_ = error;
		asio->object_callback_function_(asio);
		return 0;
	}
#ifdef _DEBUG
	nsperror << "can not callback fuction,there isn't object_callback_function_ in asio_block.";
#endif
	return -1;
}

int asio_manage::exec_message(const uint8_t cmd, const void*data)
{
	std::shared_ptr<motion::asio_block> asio;
	{
		std::lock_guard<decltype(map_message_lock_)> locker(map_message_lock_);
		auto iter = assio_message_map_.find(cmd);
		if (iter == assio_message_map_.end()){
#ifdef _DEBUG
			nsperror << "can not operate exec function,can not find message id:" << cmd << " package.";
#endif
			return -1;
		}

		asio = iter->second;
		assio_message_map_.erase(iter);
	}
	if (asio->callback_function_){
		asio->callback_function_(data);
		return 0;
	}
#ifdef _DEBUG
	nsperror << "can not callback fuction,there isn't callback_function_ in asio_block.";
#endif
	return -1;
}

//asio发送数据
int asio_manage::write(const uint32_t pkt_id, const std::shared_ptr<motion::asio_block>&asio, const std::function<int()>&MyWrite){
	if (!MyWrite){
#ifdef _DEBUG
		nsperror << "function is not exist.";
#endif
		return -1;
	}
	uint32_t ack_id = pkt_id & 0x00ffffff;
	if (push(ack_id, asio) < 0){
		return -1;
	}
	if (MyWrite() < 0){
		std::shared_ptr<motion::asio_block> previews;
		remove(ack_id, previews);
#ifdef _DEBUG
		nsperror << "failed to send packeage,the pkt_id is " << pkt_id;
#endif
		return -1;
	}
	return 0;
}

int asio_manage::write_data_forward(const uint32_t cmd, const std::shared_ptr<motion::asio_block>&asio, const std::function<int()>&MyWrite){
	if (!MyWrite){
#ifdef _DEBUG
		nsperror << "function is not exist.";
#endif
		return -1;
	}
	uint32_t ack_id = (cmd & 0xff) << 24;

	if (push(ack_id, asio) < 0){
		return -1;
	}
	if (MyWrite() < 0){
		std::shared_ptr<motion::asio_block> previews;
		remove(ack_id, previews);
#ifdef _DEBUG
		nsperror << "failed to send packeage,the ack_id is " << cmd;
#endif
		return -1;
	}
	return 0;
}

int asio_manage::post_message(const uint8_t cmd, const std::shared_ptr<motion::asio_block>&asio, const std::function<int()>&MyWrite)
{
	if (!MyWrite){
#ifdef _DEBUG
		nsperror << "function is not exist.";
#endif
		return -1;
	}

	std::lock_guard<decltype(map_message_lock_)> locker(map_message_lock_);
	if (assio_message_map_.find(cmd) == assio_message_map_.end()){
		assio_message_map_.insert(std::make_pair(cmd, asio));
	}

	if ( MyWrite() < 0){
#ifdef _DEBUG
		nsperror << "failed to send packeage,the cmd is " << cmd;
#endif
		return -1;
	}

	return 0;
}

//asio移除
int asio_manage::remove(const uint32_t ack_id, std::shared_ptr<motion::asio_block>&asio){
	std::lock_guard<decltype(map_lock_)> locker(map_lock_);
	auto iter = asio_map_.find(ack_id);
	if (iter == asio_map_.end()){
#ifdef _DEBUG
		nsperror << "can not remove in asio,can not find id:" << ack_id << " package.";
#endif
		return -1;
	}
	asio = iter->second;
	asio_map_.erase(iter);
	return 0;
}

void asio_manage::timeout_check(){
	static const uint32_t ASIO_CHECK_INTERVAL = 1000;
	static const uint32_t ASIO_TIMEOUT = 2000;
	uint32_t tick;
	while (asio_timeout_waiter_.wait(ASIO_CHECK_INTERVAL) > 0){
		tick = ::GetTickCount();
		{
			std::lock_guard<decltype(map_lock_)> locker(map_lock_);

			auto iter = asio_map_.begin();
			while (iter != asio_map_.end()){
				if (tick != iter->second->tick_){
					if (tick > iter->second->tick_){
						if ((tick - iter->second->tick_) > ASIO_TIMEOUT){
#ifdef _DEBUG
							nspwarn << "asio manage get a timeout,the pkt_id is "<<iter->first;
#endif
							//回调上层
							iter->second->error_ = nsp::proto::errorno_t::kRequestTimeout;
							timeout_notify(iter->second);
							iter = asio_map_.erase(iter);
							continue;
						}
					}
					else if ((tick + (MAXDWORD - iter->second->tick_)) > ASIO_TIMEOUT){
#ifdef _DEBUG
						nspwarn << "asio manage get a overflow timeout,the pkt_id is " << iter->first;
#endif
						//回调上层
						iter->second->error_ = nsp::proto::errorno_t::kRequestTimeout;
						timeout_notify(iter->second);
						iter = asio_map_.erase(iter);
						continue;
					}
				}
				else{
					iter->second->refresh();
				}

				++iter;
			}
		}
	}

	// asio 诊断线程退出， 通常是链接已经断开， 任何残存的 asio 都不再有意义
	std::lock_guard<decltype(map_lock_)> guard(map_lock_);
	//asio_map_.clear();
}

void asio_manage::timeout_notify(const std::shared_ptr<motion::asio_block>& asio_block){
	motion::asio_data data;
	data.set_err(nsp::proto::errorno_t::kRequestTimeout);
	if (asio_block->callback_function_){
		asio_block->callback_function_((const char*)&data);
	}
	if (asio_block->object_callback_function_){
		asio_block->object_callback_function_(asio_block);
	}
}
