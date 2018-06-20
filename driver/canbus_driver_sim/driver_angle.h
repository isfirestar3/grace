#ifndef __DRIVER_ANGLE_H__
#define __DRIVER_ANGLE_H__
#include "driver_base.h"

class driver_angle :public driver_base
{
public:
	driver_angle();
	~driver_angle();
	int read_and_write(var__functional_object_t* var);

private:
	int read(var__functional_object_t* var);
	//int write(var__functional_object_t* var);

private:
	int get_angle(int &angle);

private:
	//MDNode m_node;
	int node_id;

};

#endif

