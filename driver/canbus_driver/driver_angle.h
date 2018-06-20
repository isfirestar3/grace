#ifndef __DRIVER_ANGLE_H__
#define __DRIVER_ANGLE_H__
#include "driver_base.h"

class driver_angle :public driver_base
{
public:
	driver_angle();
	~driver_angle();

	int add_node(CML::CanOpen *canOpen, const var__functional_object_t *d);
	int read_and_write(var__functional_object_t* var);

private:
	int read(var__functional_object_t* var);
	//int write(var__functional_object_t* var);

private:
	int get_angle(int &angle);

private:
	Node m_node;
	int node_id;
	int cnt = 0;
};

#endif

