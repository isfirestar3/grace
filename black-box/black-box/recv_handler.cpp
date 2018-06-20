#include "recv_handler.h"

#define NET_WINDOW_SIZE     1

recv_handler::recv_handler(const unsigned char *data, int cb, const std::function<void(const unsigned char *, int &)> &handler)
{
	if (data && cb > 0 && handler)
	{
		try
		{
			data_ = new unsigned char[cb];
			memcpy(data_, data, cb);
			cb_ = cb;
			task_handler_ = handler;
		}
		catch (...)
		{

		}
	}
}

recv_handler::~recv_handler()
{
	if (data_ && cb_>0)
	{
		delete[] data_;
		data_ = nullptr;
		cb_ = 0;
	}
}

void recv_handler::on_task()
{
	if (task_handler_)
	{
		task_handler_(data_, cb_);
	}
}

net_recv_manager::net_recv_manager()
{
	post_handler_.allocate(nsp::os::getnprocs());
}

net_recv_manager::~net_recv_manager()
{
	post_handler_.join();
}


void net_recv_manager::async_recv_handler(const std::string& pkt,const std::function<void(const unsigned char*, int&)>&hanlder)
{
	try
	{
		std::shared_ptr<recv_handler> task = std::make_shared<recv_handler>((unsigned char*)pkt.c_str(), pkt.size(), hanlder);
		post_handler_.post(task);
	}
	catch (...)
	{
	}
}
