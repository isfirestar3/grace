#ifndef __DRIVER_DIO_H__
#define __DRIVER_DIO_H__
#include "driver_base.h"

class driver_dio :public driver_base
{
public:
	driver_dio();
	~driver_dio();

	//int add_node(CML::CanOpen *canOpen, const var__functional_object_t *d);
	int read_and_write(var__functional_object_t* var);

private:
	
	int read(var__functional_object_t* var);
	int write(var__functional_object_t* var);
private:
	//MDNode m_node;
	int node_id;

};
#endif
