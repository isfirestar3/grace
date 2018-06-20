#pragma once
#include "string"
#include "serialize.hpp"
#include "functional"

class network_task
{
public:
	network_task(std::string data, std::function<int (nsp::proto::proto_interface *)> psend = nullptr);
	~network_task();
public:
	int on_task();
private:
	std::string packet_;
	std::function<int(nsp::proto::proto_interface *)> psend_ = nullptr;
};

