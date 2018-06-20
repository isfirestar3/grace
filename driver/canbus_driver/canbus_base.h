#ifndef __CANBUS_BASE_H__
#define __CANBUS_BASE_H__
#include "CML.h"
CML_NAMESPACE_USE();

class canbus_base
{
public:
	canbus_base(int can_id, int can_type, int baud);
	virtual ~canbus_base();

	virtual int open_can_interface() = 0;
	virtual int open_canopen();
	virtual int get_canopen(CML::CanOpen * &canOpen);
	virtual int get_caninterface(CML::CanInterface* &can_interface);
public:
	int get_can_id();
	int get_can_type();
	int get_can_baud();
protected:
	bool interface_is_opened_;
	bool canopen_is_opened_;
	CML::CanOpen canopen_;
	CML::CanInterface * can_interface_;
private:
	int can_id_;
	int can_type_;
	int baud_;
};

#endif

