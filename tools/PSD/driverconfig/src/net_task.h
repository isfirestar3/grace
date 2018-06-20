#pragma once
#include <string>
#include "task_type_define.h"
#include "stdint.h"
#include <map>
#include <mutex>
#include "proto_udp_typedef.h"
#include "proto_udp_common.h"

static const std::map<int, uint8_t> register_index_map_ =
{
	{ (int)ktask_type_read_register_di_config, REGISTER_INDEX_DI_CONFIG },
	{ (int)ktask_type_read_register_di_state, REGISTER_INDEX_DI_STATE },
	{ (int)ktask_type_read_register_di_filter_time, REGISTER_INDEX_DI_FILTER_TIME },
	{ (int)ktask_type_read_register_diagnosis_current, REGISTER_INDEX_DIAGNOSIS_CURRENT },
	{ (int)ktask_type_read_register_mode, REGISTER_INDEX_MODE },
	{ (int)ktask_type_read_register_vel_acc, REGISTER_INDEX_VEL_ACC },
	{ (int)ktask_type_read_register_vel_dec, REGISTER_INDEX_VEL_DEC },
	{ (int)ktask_type_read_register_halt_deceleration, REGISTER_INDEX_HALT_DECELERATION },
	{ (int)ktask_type_read_register_veldir, REGISTER_INDEX_VELDIR },
	{ (int)ktask_type_read_register_speed_kp, REGISTER_INDEX_SPEED_KP },
	{ (int)ktask_type_read_register_speed_kl, REGISTER_INDEX_SPEED_KI },
	{ (int)ktask_type_read_register_acceleration, REGISTER_INDEX_ACCELERATION },
	{ (int)ktask_type_read_register_deceleration, REGISTER_INDEX_DECELERATION },
	{ (int)ktask_type_read_register_max_current, REGISTER_INDEX_MAX_CURRENT },
	{ (int)ktask_type_read_register_con_current, REGISTER_INDEX_CON_CURRENT },
	{ (int)ktask_type_read_register_pos_kp, REGISTER_INDEX_POS_KP },
	{ (int)ktask_type_read_register_pos_ki, REGISTER_INDEX_POS_KI },
	{ (int)ktask_type_read_register_homing, REGISTER_INDEX_HOMING },
	{ (int)ktask_type_read_register_break, REGISTER_INDEX_BREAK },
	{ (int)ktask_type_read_register_polepairs, REGISTER_INDEX_POLEPAIRS },
	{ (int)ktask_type_read_register_encoderlines, REGISTER_INDEX_ENCODERLINES },
	{ (int)ktask_type_read_register_motor_r, REGISTER_INDEX_MOTOR_R },
	{ (int)ktask_type_read_register_motor_l, REGISTER_INDEX_MOTOR_L },
	{ (int)ktask_type_read_register_switch, REGISTER_INDEX_SWITCH },
	{ (int)ktask_type_read_register_save, REGISTER_INDEX_SAVE },
	{ (int)ktask_type_read_register_jerk, REGISTER_INDEX_JERK },
	{ (int)ktask_type_read_register_velocity, REGISTER_INDEX_VELOCITY },
	{ (int)ktask_type_read_register_kvff, REGISTER_INDEX_KVFF },
	{ (int)ktask_type_read_register_kaff, REGISTER_INDEX_KAFF },
	{ (int)ktask_type_write_register_speed_kp, REGISTER_INDEX_SPEED_KP },
	{ (int)ktask_type_write_register_speed_kl, REGISTER_INDEX_SPEED_KI },
	{ (int)ktask_type_write_register_acceleration, REGISTER_INDEX_ACCELERATION },
	{ (int)ktask_type_write_register_deceleration, REGISTER_INDEX_DECELERATION },
	{ (int)ktask_type_write_register_motor_r, REGISTER_INDEX_MOTOR_R },
	{ (int)ktask_type_write_register_motor_l, REGISTER_INDEX_MOTOR_L },
	{ (int)ktask_type_write_register_polepairs, REGISTER_INDEX_POLEPAIRS },
	{ (int)ktask_type_write_register_encoderlines, REGISTER_INDEX_ENCODERLINES },
	{ (int)ktask_type_write_register_veldir, REGISTER_INDEX_VELDIR },
	{ (int)ktask_type_write_register_break, REGISTER_INDEX_BREAK },
	{ (int)ktask_type_write_register_max_current, REGISTER_INDEX_MAX_CURRENT },
	{ (int)ktask_type_write_register_con_current, REGISTER_INDEX_CON_CURRENT },
	{ (int)ktask_type_write_register_di_config, REGISTER_INDEX_DI_CONFIG },
	{ (int)ktask_type_write_register_di_state, REGISTER_INDEX_DI_STATE },
	{ (int)ktask_type_write_register_di_filter_time, REGISTER_INDEX_DI_FILTER_TIME },
	{ (int)ktask_type_write_register_save, REGISTER_INDEX_SAVE },
	{ (int)ktask_type_write_register_switch, REGISTER_INDEX_SWITCH },
	{ (int)ktask_type_write_register_clr_fault, REGISTER_INDEX_CLR_FAULT },
	{ (int)ktask_type_write_register_mode, REGISTER_INDEX_MODE },
	{ (int)ktask_type_write_register_diagnosis_current, REGISTER_INDEX_DIAGNOSIS_CURRENT },
	{ (int)ktask_type_write_register_reset_caliburation, REGISTER_INDEX_RESET_CALIBURATION },
	{ (int)ktask_type_write_register_pos_kp, REGISTER_INDEX_POS_KP },
	{ (int)ktask_type_write_register_pos_ki, REGISTER_INDEX_POS_KI },
	{ (int)ktask_type_write_register_homing, REGISTER_INDEX_HOMING },
	{ (int)ktask_type_write_register_jerk, REGISTER_INDEX_JERK },
	{ (int)ktask_type_write_register_velocity, REGISTER_INDEX_VELOCITY },
	{ (int)ktask_type_write_register_kvff, REGISTER_INDEX_KVFF },
	{ (int)ktask_type_write_register_kaff, REGISTER_INDEX_KAFF },
	{ (int)ktask_type_write_register_vel_acc, REGISTER_INDEX_VEL_ACC },
	{ (int)ktask_type_write_register_vel_dec, REGISTER_INDEX_VEL_DEC },
	{ (int)ktask_type_write_register_halt_deceleration, REGISTER_INDEX_HALT_DECELERATION },
};

#define  MAX_RETURE_NUMBER		3

class net_task
{
public:
	net_task(const std::string &strlocal, const std::string &strhost, uint16_t port, uint8_t node_id, uint8_t sub_code, task_type_t task_type_operation, uint16_t register_index, funcalltask_t callback);

	net_task(const std::string &strlocal, const std::string &strhost, uint16_t port, uint8_t node_id, uint8_t sub_code, task_type_t task_type_operation, uint16_t register_index, const std::string &data, funcalltask_t callback);

	~net_task();

	void on_task();

	//void register_callback(funcalltask_t callback)
	//{
	//	function_callback_ = callback;
	//};

//public:
//	static std::map<int, uint8_t> register_index_map_;
//
//	static std::recursive_mutex register_map_lock_;
private:
	int start_data_transmit_func();

	int read_register();

	int write_register(const std::string &str_data);

	int close_data_transmit_func();

	int check_can_read_data_packet(const nsp::proto::udp::can_datapacket_t &can_data_packet, uint8_t node_id, uint16_t register_index);

	int check_can_write_data_packet(const nsp::proto::udp::can_datapacket_t &can_data_packet, uint8_t node_id, uint16_t register_index);

	std::string ip_local_;
	std::string ip_addr_;
	uint16_t port_;
	uint16_t register_index_;

	task_type_t  task_type_operation_ = ktask_type_default;

	funcalltask_t function_callback_;

	uint8_t	 node_id_;
	uint8_t  sub_code_;
	std::string data_;
};

