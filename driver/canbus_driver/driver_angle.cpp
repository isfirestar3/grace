
#include "driver_angle.h"
#include "angle_encoder.h"
#include "var.h"

#include "log.h"

driver_angle::driver_angle() :driver_base(kVarType_AngleEncoder)
{
}


driver_angle::~driver_angle()
{
}

int driver_angle::add_node(CML::CanOpen *canOpen, const var__functional_object_t *d)
{
	var__angle_encoder_t* angle = var__object_body_ptr(var__angle_encoder_t, d);

	node_id = angle->candev_head_.cannode_;
	const Error *err;
	err = m_node.Init(*canOpen, node_id);

	if (err) 
	{
		nsperror << "Init angle Node Fail! node =" << node_id << " Error: " << err->toString();
		return err->GetID();
	}
    m_node.sdo.SetTimeout(20);

	nspinfo << "init angle node " << node_id << " success!";
	return 0;
}

int driver_angle::read(var__functional_object_t* var)
{
	var__angle_encoder_t* angle = var__object_body_ptr(var__angle_encoder_t, var);
	angle->i.time_stamp_ = driver_base::get_timestamp() - angle->candev_head_.latency_;
	int angle_get;
	angle->i.error_code_ = get_angle(angle_get);
	if (0 != angle->i.error_code_)
	{
		return -1;
	}
	angle->i.actual_angle_ = angle_get;
	return 0;
}


int driver_angle::read_and_write(var__functional_object_t* var)
{
	read(var);
	//write(var);
	return 0;
}


int driver_angle::get_angle(int &angle)
{
	uint32 v = 0;
	const Error* err = m_node.sdo.Upld32(0x6004, 0, v);
	if (err) {
		angle = 0;
		nsperror << "get_angle node =" << node_id << " failed:" << err->toString();
		return -1;
	}
	else {
		angle = v;
	}

	if (cnt++ > 100)
	{
		nspinfo << node_id << " angle node =" << node_id << " encoder:" << angle;
		cnt = 0;
	}


	return 0;
}