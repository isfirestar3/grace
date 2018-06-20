#include "agv_shell_alive.h"
#include <stdlib.h>
#include <algorithm>
#include "agv_shell_session.h"
#include "log.h"

agv_shell_alive::agv_shell_alive()
{
	init();
}

agv_shell_alive::~agv_shell_alive()
{
	uinit();
}

void agv_shell_alive::add_session(const std::shared_ptr<nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>>& ptr)
{
	std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
	vct_session_.push_back(ptr);
}

void agv_shell_alive::remove_session(const std::shared_ptr<nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>>& ptr)
{
	std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
	auto iter = std::find_if(vct_session_.begin(), vct_session_.end(), [&](const std::shared_ptr<nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>>& ptr_iter)->bool{
		return ptr == ptr_iter ? true : false;
	});
	if (iter == vct_session_.end())
	{
		return;
	}
	if (*iter)
	{
		std::shared_ptr<agv_shell_session> client_session = std::static_pointer_cast<agv_shell_session>(*iter);
		loinfo("agv_shell_interface") << "remove alive session object:" << client_session->get_target_endpoint().to_string() << " ,the link is:" << client_session->get_link();
	}
	vct_session_.erase(iter);
}

void agv_shell_alive::init()
{
	if (timeout_th_)
	{
		return;
	}
	try
	{
		timeout_th_ = new std::thread(std::bind(&agv_shell_alive::run_check_th, this));
	}
	catch (...)
	{
		loerror("agv_shell_interface") << "can not create check alive thread.";
	}
	return;
}

void agv_shell_alive::uinit()
{
	is_exits_ = true;
	if (timeout_th_)
	{
		if (timeout_th_->joinable())
		{
			timeout_th_->join();
		}
		delete timeout_th_;
		timeout_th_ = nullptr;
	}
	th_wait_.reset();
}

void agv_shell_alive::run_check_th()
{
	static int TIMEOUT_INTERVAL = 2000;
	static int MAX_COUNT = 3;
	while (th_wait_.wait(TIMEOUT_INTERVAL))
	{
		if (is_exits_)break;

		{
			std::lock_guard<decltype(session_mutex_)> lock(session_mutex_);
			auto iter = vct_session_.begin();
			while (iter != vct_session_.end())
			{
				std::shared_ptr<agv_shell_session> client_session = std::static_pointer_cast<agv_shell_session>(*iter);
				if (client_session->get_alive_count() > MAX_COUNT)
				{
					client_session->clear_process_list();
					loerror("agv_shell_interface") << "get alive package timeout,then disconnect from target endpoint:" << client_session->get_target_endpoint().to_string() << " ,the link is:" << client_session->get_link();
					client_session->close();
					//É¾³ý¸Ãsession¶ÔÏó
					iter = vct_session_.erase(iter);
				}
				else
				{
					client_session->add_alive_count();
					iter++;
				}
			}
		}
	}
}