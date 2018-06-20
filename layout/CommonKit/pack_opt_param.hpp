#pragma once
#include "BasicDefine.h"

class pack_opt_param
{
public:
	pack_opt_param()
	{
		memset(opt_param_, 0, 20 * sizeof(uint64_t));
	}
	~pack_opt_param()
	{

	}
public:
	uint64_t& operator [](int index)
	{
		return opt_param_[index];
	}
	int post()
	{

	}

private:
	uint64_t opt_param_[20];
};