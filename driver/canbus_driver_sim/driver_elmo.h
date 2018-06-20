#ifndef __DRIVER_ELMO_H__
#define __DRIVER_ELMO_H__
#include "driver_base.h"

enum CIA402_6060_CODE
{
	CIA402MODE_DISABLED         = 0,

	/// In this mode the CANopen network sends move commands to the amplifier,
	/// and the amplifier uses it's internal trajectory generator to perform
	/// the moves.
	/// This mode conforms to the CANopen device profile for motion control
	/// (DSP-402) profile position mode.
	CIA402MODE_CAN_PP      = 0x01,

	/// In this mode the CANopen network commands target velocity values to the
	/// amplifier.  The amplifier uses it's programmed acceleration and deceleration
	/// values to ramp the velocity up/down to the target.
	///
	/// Note that support for profile velocity mode was added in amplifier firmware
	/// version 3.06.  Earlier versions of firmware will report an error if this
	/// mode is selected.
	CIA402MODE_CAN_PV     = 0x03,

	/// In this mode the CANopen network commands torque values to the amplifier.
	///
	/// Note that support for profile torque mode was added in amplifier firmware
	/// version 3.34.  Earlier versions of firmware will report an error if this
	/// mode is selected.
	CIA402MODE_CAN_PT       = 0x04,

	/// This mode is used to home the motor (i.e. find the motor zero position)
	/// under the control of the CANopen network.
	/// This mode conforms to the CANopen device profile for motion control
	/// (DSP-402) homing mode.
	CIA402MODE_CAN_HOMING       = 0x06,

	/// In this mode the CANopen master calculates the motor trajectory and
	/// streams it over the CANopen network as a set of points that the
	/// amplifier interpolated between.
	/// This mode conforms to the CANopen device profile for motion control
	/// (DSP-402) interpolated position mode.
	CIA402MODE_CAN_PVT          = 0x07,

	CIA402MODE_CAN_CSP          = 0x08,
	CIA402MODE_CAN_CSV        	= 0x09,
	CIA402MODE_CAN_CST          = 0x0A,

	CIA402MODE_PROG_VEL         = 0x0B,
};

class driver_elmo:public driver_base
{
public:
	driver_elmo();
	~driver_elmo();

	int read_and_write(var__functional_object_t* var);

private:
	int read(var__functional_object_t* var);
	int write(var__functional_object_t* var);

private:
	int set_enable(bool enable);
	int set_velocity(int v);
	int get_veloctiy(int& v);
	int get_current(int& cur);
	bool get_stop_state(int di,bool &stop_state);
	int set_position(int pos);
	int get_position(int& pos);
	int pos_is_reached(bool& b_reached);
	int get_di(int& di);
	int get_error_code(int &error);
private:
	//MDNode m_node;
	//CiA402 m_amp;//Elmo??????
	int node_id;
private:
	bool m_emergency_stop;
	int enabled_ = 0;
	int postion_cnt_ = 0;
	int error_cnt_ = 0;
	int64_t last_postion_ = 0;
	int last_control_mode_ = 0;
	int retry_cnt_ = 0;

	int last_v_ = 0;

        CIA402_6060_CODE get_ctrl_mode( control_mode_t mode );
		uint64_t time_position_begin = 0;
		double deta_position = 0;
};

#endif

