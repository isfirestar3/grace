
#include "canbus_base.h"

canbus_base::canbus_base(int can_id, int can_type, int baud)
: interface_is_opened_(true), canopen_is_opened_(true)
, can_id_(can_id), can_type_(can_type), baud_(baud)
{

}


canbus_base::~canbus_base()
{
}

int canbus_base::get_can_id()
{
	return can_id_;
}

int canbus_base::get_can_type()
{
	return can_type_;
}

int canbus_base::get_can_baud()
{
	return baud_;
}


