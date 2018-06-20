#ifndef __DRIVER_ELMO_H__
#define __DRIVER_ELMO_H__

#include "driver_base.h"

class driver_elmo:public driver_base
{
public:
	driver_elmo();
	~driver_elmo();

	int add_node(CML::CanOpen *canOpen, const var__functional_object_t *d);
	int read_and_write(var__functional_object_t* var);

private:
	int8 get_ctrl_mode(control_mode_t mode);
	int read(var__functional_object_t* var);
	int write(var__functional_object_t* var);

private:
	int set_enable(bool enable);
	int set_velocity(int v);
	int get_veloctiy(int& v);
	int get_current(int& cur);
	bool get_stop_state( int di,bool &stop_state);
	int set_position(int pos);
	int get_position(int& pos);
	int pos_is_reached(bool& b_reached);
	int get_di(int& di);
	int get_error_code(int &error);
private:
	MDNode m_node;
	CiA402 m_amp;//ElmoÇý¶¯Æ÷
	int node_id;
private:
	bool m_emergency_stop;
	int enabled_ = 0;
	int postion_cnt_ = 0;
	int error_cnt_ = 0;
	int64_t last_postion_ = 0;
	int last_control_mode_ = 0;
	int retry_cnt_ = 0;
	int current_rate = 10000;

	uint64_t time_position_begin = 0;
	int64_t deta_position = 0;

	int last_err = 0;
};

#endif

