#pragma once

#include <functional>

class dhcp_notify{
public:
	dhcp_notify(const std::function<void()>& func) : network_callback_(func){}
	~dhcp_notify(){}

public:
	void on_task() {
		if (network_callback_)
		{
			network_callback_();
		}
	}

private:
	std::function<void()> network_callback_;
};