#ifndef __DRIVER_CUTIS_H__
#define __DRIVER_CUTIS_H__

#include "driver_base.h"

class driver_cutis :public driver_base
{
public:
	driver_cutis();
	~driver_cutis();
	int read_and_write(var__functional_object_t* var);

private:
	
	int read(var__functional_object_t* var);
	int write(var__functional_object_t* var);
private:
	int node_id;
    int64_t last_velocity_ = 0;
};
#endif
