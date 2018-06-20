
#include "canbus_base.h"

canbus_base::canbus_base(int can_id, int can_type, int baud)
: interface_is_opened_(false), canopen_is_opened_(false)
, can_id_(can_id), can_type_(can_type), baud_(baud)
{

}


canbus_base::~canbus_base()
{
}

int canbus_base::get_canopen(CML::CanOpen * &canOpen)
{
	canOpen = &canopen_;
	return canopen_is_opened_ ? 0 : -1;
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

int canbus_base::get_caninterface(CML::CanInterface* &can_interface)
{
	can_interface = can_interface_;
	return interface_is_opened_ ? 0 : -1;
}

int canbus_base::open_canopen()
{
	if (canopen_is_opened_)
	{
		return 0;
	}

	if (false == interface_is_opened_)
	{
		return -1;
	}
	const CML::Error *err = canopen_.Open(*can_interface_);
	if (err)
	{
		return err->GetID();
	}

	canopen_is_opened_ = true;

	return 0;

}

