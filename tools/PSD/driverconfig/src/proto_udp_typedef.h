#pragma once

#include "asio.hpp"

//启动数据转发功能
#define START_DATA_TRANSMIT_FUNC 0x06
//关闭数据转发功能
#define CLOSE_DATA_TRANSMIT_FUNC 0X07

//主操作码
#define FIRMWARE_MAIN_OPERATE_DATA_FORWARD			0x08

//副操作码
#define FIRMWARE_SUB_OPERATE_						0X01

// can数据转发命令
#define FIRMWARE_CAN_CMD_GET_SOFTWARE_VERSION					0X01
#define FIRMWARE_CAN_CMD_GET_SOFTWARE_VERSION_ACK					0X11
#define FIRMWARE_CAN_CMD_INITIAL_ROM_UPDATE						0X03
#define FIRMWARE_CAN_CMD_INITIAL_ROM_UPDATE_ACK					0X13
#define FIRMWARE_CAN_CMD_DOWNLOAD_DATAPACKET					0X04
#define FIRMWARE_CAN_CMD_DOWNLOAD_DATAPACKET_ACK					0X14
#define FIRMWARE_CAN_CMD_ABORT_UPDATE							0X0F
#define FIRMWARE_CAN_CMD_ABORT_UPDATE_ACK						0X1F

#define FIRMWARE_CAN_CMD_READ_WAVE_DATA								0X02				//TEMP

#define FIRMWARE_CAN_CMD_READ_REGISTER					0X0C
#define FIRMWARE_CAN_CMD_READ_REGISTER_ACK				0X1C
#define FIRMWARE_CAN_CMD_WRITE_REGISTER					0X0D
#define FIRMWARE_CAN_CMD_WRITE_REGISTER_ACK				0X1D

//寄存器索引

#define REGISTER_INDEX_DI_CONFIG					0x0004
#define REGISTER_INDEX_DI_STATE						0x0014
#define REGISTER_INDEX_DI_FILTER_TIME				0x0024
#define REGISTER_INDEX_DIAGNOSIS_CURRENT			0x0005
#define REGISTER_INDEX_MODE							0X09
#define REGISTER_INDEX_VEL_ACC						0X07
#define REGISTER_INDEX_VEL_DEC						0x0006
#define REGISTER_INDEX_HALT_DECELERATION			0x0035
#define REGISTER_INDEX_VELDIR						0X08
#define REGISTER_INDEX_SPEED_KP						0X0A
#define REGISTER_INDEX_SPEED_KI						0X0B
#define REGISTER_INDEX_ACCELERATION					0x0031
#define REGISTER_INDEX_DECELERATION					0x0035
#define REGISTER_INDEX_MAX_CURRENT					0X0C
#define REGISTER_INDEX_CON_CURRENT					0X0D
#define REGISTER_INDEX_POS_KP						0X0E
#define REGISTER_INDEX_POS_KI						0X0F
#define REGISTER_INDEX_HOMING						0X10
#define REGISTER_INDEX_BREAK						0X11
#define REGISTER_INDEX_POLEPAIRS					0X20
#define REGISTER_INDEX_ENCODERLINES					0X21
#define REGISTER_INDEX_MOTOR_R						0X22
#define REGISTER_INDEX_MOTOR_L						0X23
#define REGISTER_INDEX_SWITCH						0XFF
#define REGISTER_INDEX_SAVE							0X47
#define REGISTER_INDEX_CLR_FAULT					0x00FA
#define REGISTER_INDEX_RESET_CALIBURATION			0x0003
#define REGISTER_INDEX_JERK			                0x0030
#define REGISTER_INDEX_VELOCITY						0x0032
#define REGISTER_INDEX_KVFF							0x0033
#define REGISTER_INDEX_KAFF							0x0034

static const int SEDN_INTERVAL = 50;

enum EXECUTE_ACK
{
	ACK_Normal = 0x00,				//执行正常
	ACK_Version_Failed = 0x01,		//通讯协议版不匹配
	ACK_Limits = 0x02,				//权限不符
	ACK_Command_No = 0x03,			//命令不存在
	ACK_Addr_Not_Write = 0x04,		//地址不可写
	ACK_OverFlow_Memory = 0x05,		//数据地址溢出
	ACK_Content_Invaild = 0x06,		//数据内容非法或校验不通过
	ACK_VCU_Busy = 0x07,			//VCU忙，无法执行
	ACK_Target_Port_Faild = 0x08,	//数据转发的通讯端口存在故障
	ACK_Target_Not_exist = 0x09,	//数据转发的目标设备不存在
	ACK_Target_Not_Reponse = 0x0A,	//数据转发的目标设备未应答
	ACK_Data_Lose = 0x0B,			//数据分帧之间有丢包
	ACK_Other_Problom =0xff			//其它错误
};

enum CAN_RET
{
	CAN_RET_FAILED,
	CAN_RET_SUCCESSED
};

struct recv_data : motion::asio_data{
	std::string remote_ip_;
	unsigned int data_length_;
	std::string data_context_;
};

struct speed_data
{
	union
	{
		uint32_t kp_;
		uint32_t ki_;
	};
};

struct vel_acc_t
{
	int32_t vel_acc_;
};

struct vel_dec_t
{
	int32_t vel_dec_;
};

struct halt_deceleration_t
{
	int32_t halt_deceleration_;
};

struct motor_r
{
	float motor_r_;
};

struct motor_l
{
	float motor_l_;
};

struct polepairs
{
	uint8_t polepairs_;
};

struct encoderlines
{
	uint16_t encoderlines_;
};

struct vel_dir
{
	int16_t vel_dir_;
};

struct break_t
{
	bool break_t_;
};

struct max_current_t
{
	uint8_t max_current;
};

struct con_current_t
{
	uint8_t con_current;
};

struct di_config_t
{
	uint8_t di_config_;
};

struct di_filter_time_t
{
	uint16_t di_filter_time_;
};

struct di_state_t
{
	uint16_t di_state_;
};

struct save_t
{
	uint32_t save_;
};

struct switch_t
{
	uint8_t switch_;
};

struct clr_fault_t
{
	int32_t clr_fault_;
};

struct mode_t
{
	uint8_t mode_;
};

struct diagnosis_current_t
{
	uint32_t diagnosis_current_;
};

struct reset_caliburation_t
{
	uint8_t reset_caliburation_;
};

struct homing_t
{
	uint8_t homing_;
};

struct pos_ki_t
{
	int32_t pos_ki_;
};

struct pos_kp_t
{
	int32_t pos_kp_;
};

struct jerk_t
{
	float jerk_;
};

struct velocity_t
{
	float velocity_;
};

struct kvff_t
{
	int32_t kvff_;
};

struct kaff_t
{
	int32_t kaff_;
};

struct recv_bin_data : motion::asio_data{
	unsigned int offset_;
	unsigned int data_length_;
	std::string data_context_;
};
