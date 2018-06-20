
#include "canbus_kvaser.h"

#include "canio.h"
#include <stdio.h>
#include <memory>

canbus_kvaser::canbus_kvaser(int can_id, int can_type, int baud)
:canbus_base(can_id,can_type,baud)
{

}


canbus_kvaser::~canbus_kvaser()
{
}

int canbus_kvaser::open_can_interface()
{
	return 0;
}

int canbus_kvaser::open_canopen()
{
	return 0;
	if (canbus_base::get_can_type() == kCanDrvType_Kvaser)
	{
		if (canbus_base::canopen_is_opened_)
		{
			return 0;
		}

		canbus_base::canopen_is_opened_ = true;
	}
	else
	{
		return -2;
	}

	return 0;
}
