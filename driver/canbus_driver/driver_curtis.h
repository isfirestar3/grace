#ifndef __DRIVER_curtis_H__
#define __DRIVER_curtis_H__

#include "driver_base.h"
#include "curtis.h"
#include <mutex>
#include  <iostream>  

class driver_curtis :public driver_base, Receiver
{
public:
	driver_curtis();
	~driver_curtis();

	int add_node(CML::CanOpen *canOpen, const var__functional_object_t *d);
	int read_and_write(var__functional_object_t* var);

private:
	int read(var__functional_object_t* var);
	int write(var__functional_object_t* var);

	int NewFrame(CanFrame &recv_frame);

	void analysis_1(var__curtis_t* curtis);
	void package_1(var__curtis_t* curtis, CanFrame &send_data);

    void analysis_2(var__curtis_t* curtis);
    void package_2(var__curtis_t* curtis, CanFrame &send_data);
private:


	int enabled_ = 0;
	
	CML::CanOpen *_canOpen;

	//CanFrame tempFrame;
	//buf_recv_canFrame tempFrame;
	int32 temp_recv_id;
	uint8_t temp_recv_data[8];
	
	std::mutex __mtx_curtis;
};

#endif
