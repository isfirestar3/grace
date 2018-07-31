#include "session_task_manager.h"

base_task_t::base_task_t(const unsigned char *data, int cb, const std::function<void(const unsigned char *, int)> &handler)
{
	task_type_ = kTaskType_Recv;
	if (data && cb > 0 && handler) {
		try {
			data_ = new unsigned char[cb];
			memcpy(data_,data, cb);
			cb_ = cb;
			recv_handler = handler;
		}catch(...) {
			;
		}
	}
}

base_task_t::~base_task_t()
{
	if (data_ && cb_ > 0 ) {
		delete []data_;
		data_ = nullptr;
		cb_ = 0;
	}	
}

base_task_t::base_task_t(const base_task_t &lref)
{
	task_type_ = lref.task_type_;
	if (lref.data_ && lref.cb_ > 0) {
		try {
			cb_ = lref.cb_;
			data_ = new unsigned char[cb_];
			memcpy(data_,lref.data_,cb_);
		} catch(...) {
			;
		}
	}
	recv_handler = lref.recv_handler;
}

base_task_t::base_task_t(base_task_t &&rref)
{
	task_type_ = rref.task_type_;
	data_ = rref.data_;
	cb_ = rref.cb_;
	rref.data_ = nullptr;
	rref.cb_ = 0;
	recv_handler = rref.recv_handler;
}

void base_task_t::on_task()
{
#ifndef _WIN32
	if (__builtin_expect(!!(kTaskType_Recv == task_type_), 1)) {
		if (__builtin_expect(!!(recv_handler), 1)) {
#else
	if (kTaskType_Recv == task_type_) {
		if (recv_handler) {
#endif
			recv_handler(data_, cb_);
		}
	}
}

/////////////////////////////////////////////////////////////// session_task_manager ///////////////////////////////////////////////////////////////
session_task_manager::session_task_manager() {
	task_pool_.allocate(nsp::os::getnprocs());
}

session_task_manager::~session_task_manager() {
	task_pool_.join();
}

void session_task_manager::schedule_async_receive(const std::string &pkt, const std::function<void(const unsigned char *,int)> &handler) {
	try {
		std::shared_ptr<base_task_t> sptr = std::make_shared<base_task_t>((const unsigned char *)pkt.data(), pkt.size(),handler);
		task_pool_.post(sptr);
	}catch(...){
		;
	}
}

session_keepalive_manager::session_keepalive_manager() {
	task_pool_.allocate(1);
}

session_keepalive_manager::~session_keepalive_manager() {
	task_pool_.join();
}

void session_keepalive_manager::schedule_async_receive(const std::string &pkt, const std::function<void(const unsigned char *, int)> &handler) {
	try {
		std::shared_ptr<base_task_t> sptr = std::make_shared<base_task_t>((const unsigned char *)pkt.data(), pkt.size(), handler);
		task_pool_.post(sptr);
	}
	catch (...){
		;
	}
}
