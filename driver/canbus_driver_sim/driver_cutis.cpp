
#include "var.h"
#include "log.h"
#include "driver_cutis.h"
#include "../nsp/os_util.hpp"
#include "../var/curtis.h"

driver_cutis::driver_cutis()
{
}


driver_cutis::~driver_cutis()
{
}


int driver_cutis::read(var__functional_object_t* var)
{
	var__curtis_t *curtis = var__object_body_ptr(var__curtis_t, var);

	curtis->i.actual_angle_ = 100;
	curtis->i.actual_velocity_ = last_velocity_;
	curtis->i.control_feedback_ = 1;
	curtis->i.enabled_ = 1;
	curtis->i.error_code_ = 0;
	curtis->i.node_state_ = 0;
	curtis->i.timestamp_ = driver_base::get_timestamp();
	curtis->i.voltage_ = 10000;
	return 0;
}


int driver_cutis::write(var__functional_object_t* var)
{
	var__curtis_t *curtis = var__object_body_ptr(var__curtis_t, var);
    last_velocity_ = curtis->command_velocity_;
	return 0;
}

int driver_cutis::read_and_write(var__functional_object_t* var)
{
	read(var);
	write(var);
	return 0;
}