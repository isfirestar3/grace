#ifndef __DRIVER_MOONS_H__
#define __DRIVER_MOONS_H__
#include "driver_base.h"

class driver_moons :public driver_base
{
public:
	driver_moons();
	~driver_moons();

	
	int read_and_write(var__functional_object_t* var);

private:
	
	int read(var__functional_object_t* var);
	int write(var__functional_object_t* var);

private:
	int set_enable(bool enable);
	int set_velocity(int v);
	int get_veloctiy(int& v);
	int get_current(int& cur);
	bool get_stop_state(int di, bool &stop_state);
	int set_position(int pos);
	int get_position(int& pos);
	int pos_is_reached(bool& b_reached);
	int get_di(int& di);
	int clear_fault();
private:
	//MDNode m_node;
	int node_id;
private:
	bool m_emergency_stop;
	int enabled_ = 0;
	int postion_cnt_ = 0;
	int64_t last_postion_ = 0;

	int position_err_cnt_ = 0;
	int retry_cnt_ = 0;
};

#endif

