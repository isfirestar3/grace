#pragma once

#include "network_session.h"
#include "singleton.hpp"
#include "proto_udp_typedef.h"
#include "task_type_define.h"
#include <map>
#include <string>
#include <memory>

class network_client_manager{
	network_client_manager();
	~network_client_manager();
	friend class nsp::toolkit::singleton<network_client_manager>;

public:
	void read_register_data(task_type_t task_type, uint16_t sub_index, funcalltask_t callback = nullptr);

	void write_register_data(task_type_t task_type, uint16_t sub_index, const std::string &str_data, funcalltask_t callback = nullptr);

	void write_speed_pid(task_type_t task_type, uint16_t sub_index, double kp, funcalltask_t callback = nullptr);

	void write_vel_acc(task_type_t task_type, uint16_t sub_index, int32_t vel_acc_, funcalltask_t callback = nullptr);
	
	void write_vel_dec(task_type_t task_type, uint16_t sub_index, int32_t vel_dec_, funcalltask_t callback = nullptr);

	void write_halt_deceleration(task_type_t task_type, uint16_t sub_index, int32_t halt_deceleration_, funcalltask_t callback = nullptr);

	void write_motor_r(task_type_t task_type, uint16_t sub_index, float motor_r_, funcalltask_t callback = nullptr);

	void write_motor_l(task_type_t task_type, uint16_t sub_index, float motor_l_, funcalltask_t callback = nullptr);

	void write_polepairs(task_type_t task_type, uint16_t sub_index, uint8_t polepairs_, funcalltask_t callback = nullptr);
	
	void write_encoderlines(task_type_t task_type, uint16_t sub_index, uint16_t encoderlines_, funcalltask_t callback = nullptr);
	
	void write_vel_dir(task_type_t task_type, uint16_t sub_index, int16_t vel_dir_, funcalltask_t callback = nullptr);
	
	void write_break_t(task_type_t task_type, uint16_t sub_index, bool break_t_, funcalltask_t callback = nullptr);

	void write_max_current(task_type_t task_type, uint16_t sub_index, uint8_t max_current_, funcalltask_t callback = nullptr);

	void write_con_current(task_type_t task_type, uint16_t sub_index, uint8_t con_current_, funcalltask_t callback = nullptr);

	void write_di_config(task_type_t task_type, uint16_t sub_index, uint8_t di_config_, funcalltask_t callback = nullptr);

	void write_di_filter_time(task_type_t task_type, uint16_t sub_index, uint16_t di_filter_time_, funcalltask_t callback = nullptr);

	void write_di_state(task_type_t task_type, uint16_t sub_index, uint16_t di_state_, funcalltask_t callback = nullptr);
	
	void write_save(task_type_t task_type, uint16_t sub_index, uint32_t save_, funcalltask_t callback = nullptr);

	void write_switch(task_type_t task_type, uint16_t sub_index, uint8_t switch_, funcalltask_t callback = nullptr);

	void write_clr_fault(task_type_t task_type, uint16_t sub_index, int32_t clr_fault_, funcalltask_t callback = nullptr);

	void write_mode(task_type_t task_type, uint16_t sub_index, uint8_t mode_, funcalltask_t callback = nullptr);

	void write_diagnosis_current(task_type_t task_type, uint16_t sub_index, uint32_t diagnosis_current_, funcalltask_t callback = nullptr);

	void write_reset_caliburation(task_type_t task_type, uint16_t sub_index, uint8_t reset_caliburation_, funcalltask_t callback = nullptr);

	void write_homing(task_type_t task_type, uint16_t sub_index, uint8_t homing_, funcalltask_t callback = nullptr);
	
	void write_pos_ki(task_type_t task_type, uint16_t sub_index, int32_t pos_ki_, funcalltask_t callback = nullptr);
	
	void write_pos_kp(task_type_t task_type, uint16_t sub_index, int32_t pos_kp_, funcalltask_t callback = nullptr);
	
	void write_jerk(task_type_t task_type, uint16_t sub_index, float jerk_, funcalltask_t callback = nullptr);
	
	void write_acceleration(task_type_t task_type, uint16_t sub_index, float acceleration_, funcalltask_t callback = nullptr);
	
	void write_velocity(task_type_t task_type, uint16_t sub_index, float velocity_, funcalltask_t callback = nullptr);
	
	void write_kvff(task_type_t task_type, uint16_t sub_index, int32_t kvff_, funcalltask_t callback = nullptr);
	
	void write_kaff(task_type_t task_type, uint16_t sub_index, int32_t kaff_, funcalltask_t callback = nullptr);

	int init_network(const std::string& ip_local,const std::string& ip_por_str);

	int clean_session();

	int start_data_transmit_func(const std::string &ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial);

	int post_read_register_cmd(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, uint16_t register_index );

	int post_write_register_cmd(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, uint16_t register_index, const std::string &str_data);

	int close_data_transmit_func(const std::string &ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial);

	int initial_rom_update(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid);

	int post_read_version_cmd(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid);

	int post_abort_update_cmd(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid);

	int post_download_datapacket_cmd(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, std::string & datastr, uint16_t index);

private:
	nsp::tcpip::endpoint search_endpoint(const std::string ip);
private:
	std::recursive_mutex re_mutex_;

	std::map<std::string, nsp::tcpip::endpoint> map_endpoint_;//缓存所有的ip的endpoint对象

	std::shared_ptr<network_session> net_session_;
};