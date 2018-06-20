#ifndef __DIRVER_BASE_H__
#define __DIRVER_BASE_H__

#include "var.h"


class driver_base
{
public:
	driver_base();
	virtual ~driver_base();

	
	virtual int read_and_write(var__functional_object_t* var) = 0;

public:
	int set_driver_callback(void(*driver_call_back)(void *functional_object));
protected:
	void callback_data(void* v);
	uint64_t get_timestamp();
	
private:
	void(*driver_call_back_)(void *functional_object);
};

#endif


