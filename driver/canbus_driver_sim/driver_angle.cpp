#include "driver_angle.h"
#include "angle_encoder.h"
#include "var.h"
#include <time.h>
#include "log.h"

driver_angle::driver_angle()
{
}


driver_angle::~driver_angle()
{
}

int driver_angle::read(var__functional_object_t* var)
{
    var__angle_encoder_t* angle = var__object_body_ptr(var__angle_encoder_t, var);
    angle->i.time_stamp_ = driver_base::get_timestamp() - angle->candev_head_.latency_;
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
    
	/*int16 v = 0;
	//id 0x00????????????????????? by zhengjiajia
	const Error* err = dev_401.Ain16Read(0x00, v);
	if (err) {
		angle = 0;
		return -1;
	}
	else {
		angle = v;
	}
	loinfo << "??¦Ë????:" << angle;*/
	return 0;
}