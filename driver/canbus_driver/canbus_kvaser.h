#ifndef __CANBUS_KVASER_H__
#define __CANBUS_KVASER_H__

#include "canbus_base.h"
#include "can_kvaser.h"

class canbus_kvaser:public canbus_base
{
public:
	canbus_kvaser(int can_id, int can_type, int baud);
	~canbus_kvaser();

	int open_can_interface();
private:
	KvaserCAN* kvaser_can_;
	char can_name[20];
};
#endif


