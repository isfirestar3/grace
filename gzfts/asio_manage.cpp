#include "asio_manage.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <limits.h>
#endif
#include "log.h"

asio_block::asio_block(const asio_block&lref){
	ack_ = lref.ack_;
	cb_ = lref.cb_;
	usr_ = lref.usr_;
	error_ = lref.error_;
	callback_function_ = lref.callback_function_;
	tick_ = nsp::os::gettick();
}

asio_block &asio_block::operator = (const asio_block&lref){
	if (this != &lref){
		ack_ = lref.ack_;
		cb_ = lref.cb_;
		usr_ = lref.usr_;
		error_ = lref.error_;
		callback_function_ = lref.callback_function_;
		tick_ = nsp::os::gettick();
	}
	return *this;
}

void asio_block::refresh(){
	tick_ = nsp::os::gettick();
}


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
int asio_manage::push(const int pkt_id, const std::shared_ptr<asio_block>& asio){
	std::lock_guard<decltype(map_lock_)> locker(map_lock_);
	if (asio_map_.find(pkt_id) != asio_map_.end()){
#ifdef _DEBUG
		lowarn("fts") << "can not push new asio_block into map,there is already exist pkt_id.";
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
int asio_manage::exec(const int pkt_id, const void*data){
	std::shared_ptr<asio_block> asio;
	{
		std::lock_guard<decltype(map_lock_)> locker(map_lock_);
		auto iter = asio_map_.find(pkt_id);
		if (iter == asio_map_.end()){
#ifdef _DEBUG
			loerror("fts") << "can not operate exec function,can not find id:" << pkt_id << " package.";
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
	loerror("fts") << "can not callback fuction,there isn't callback_function_ in asio_block.";
#endif
	return -1;
}

int asio_manage::exec(const int pkt_id, const void* stream, const int error, const int cb){
	std::shared_ptr<asio_block> asio;
	{
		std::lock_guard<decltype(map_lock_)> locker(map_lock_);
		auto iter = asio_map_.find(pkt_id);
		if (iter == asio_map_.end()){
#ifdef _DEBUG
			loerror("fts") << "can not operate exec function,can not find id:" << pkt_id << " package.";
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
	loerror("fts") << "can not callback fuction,there isn't object_callback_function_ in asio_block.";
#endif
	return -1;
}

//asio移除
int asio_manage::remove(const int pkt_id, std::shared_ptr<asio_block>&asio){
	std::lock_guard<decltype(map_lock_)> locker(map_lock_);
	auto iter = asio_map_.find(pkt_id);
	if (iter == asio_map_.end()){
#ifdef _DEBUG
		loerror("fts") << "can not remove in asio,can not find id:" << pkt_id << " package.";
#endif
		return -1;
	}
	asio = iter->second;
	asio_map_.erase(iter);
	return 0;
}

//asio发送数据
int asio_manage::write(const int pkt_id, const std::shared_ptr<asio_block>&asio, const std::function<int()>&MyWrite){
	if (!MyWrite){
#ifdef _DEBUG
		loerror("fts") << "lamba function is not exist.";
#endif
		return -1;
	}
	if (push(pkt_id, asio) < 0){
		return -1;
	}
	if (MyWrite() < 0){
		std::shared_ptr<asio_block> previews;
		remove(pkt_id, previews);
#ifdef _DEBUG
		loerror("fts") << "failed to send packeage,the pkt_id is " << pkt_id;
#endif
		return -1;
	}
	return 0;
}

void asio_manage::timeout_check(){
	static const uint32_t ASIO_CHECK_INTERVAL = 5000;
	static const uint32_t ASIO_TIMEOUT = 5000;
	uint32_t tick;
	while (asio_timeout_waiter_.wait(ASIO_CHECK_INTERVAL) > 0){
		tick = nsp::os::gettick();
		{
			std::lock_guard<decltype(map_lock_)> locker(map_lock_);
			auto iter = asio_map_.begin();
			while (iter != asio_map_.end()){
				if (tick != iter->second->tick_){
					if (tick > iter->second->tick_){
						if ((tick - iter->second->tick_) > ASIO_TIMEOUT){
#ifdef _DEBUG
							lowarn("fts") << "asio manage get a timeout,the pkt_id is " << iter->first;
#endif
							//回调上层
							iter->second->error_ = nsp::proto::errorno_t::kRequestTimeout;
							timeout_notify(iter->second);
							iter = asio_map_.erase(iter);
							continue;
						}
					}
#ifdef _WIN32
					else if ((tick + (MAXDWORD - iter->second->tick_)) > ASIO_TIMEOUT){
#else
                    else if ((tick + (UINT_MAX - iter->second->tick_)) > ASIO_TIMEOUT){
#endif
#ifdef _DEBUG
						lowarn("fts") << "asio manage get a overflow timeout,the pkt_id is " << iter->first;
#endif
						//回调上层
						iter->second->error_ = nsp::proto::errorno_t::kRequestTimeout;
						timeout_notify(iter->second);
						iter = asio_map_.erase(iter);
						continue;
					}
				}
				else
				{
					iter->second->refresh();
				}
				++iter;
			}
		}
	}

	// asio 诊断线程退出， 通常是链接已经断开， 任何残存的 asio 都不再有意义
	std::lock_guard<decltype(map_lock_)> guard(map_lock_);
	asio_map_.clear();
}

void asio_manage::timeout_notify(const std::shared_ptr<asio_block>& asio_block){
	asio_data data;
	data.set_err(nsp::proto::errorno_t::kRequestTimeout);
	if (asio_block->callback_function_){
		asio_block->callback_function_((const char*)&data);
	}
	if (asio_block->object_callback_function_){
		asio_block->object_callback_function_(asio_block);
	}
}
