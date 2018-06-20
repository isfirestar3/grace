#pragma once

#include "singleton.hpp"
#include "gzfts_api.h"

class fts_receiver {
	fts_receiver();
	~fts_receiver();
	friend class nsp::toolkit::singleton<fts_receiver>;

public:

private:
	void initlization();
	void uint();


};