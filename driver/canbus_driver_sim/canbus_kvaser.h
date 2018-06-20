#ifndef __CANBUS_KVASER_H__
#define __CANBUS_KVASER_H__

#include "canbus_base.h"

class canbus_kvaser:public canbus_base
{
public:
	canbus_kvaser(int can_id, int can_type, int baud);
	~canbus_kvaser();

	int open_can_interface();
	int open_canopen();
private:
	//KvaserCAN* kvaser_can_;
};
#endif


