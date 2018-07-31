#pragma once

#include "asio.hpp"

#define FIRMWARE_MAIN_OPERATE_CODE_MODULE_CMD				0x02
#define FIRMWARE_SUB_OPERATE_CODE_RESET_VCU					0x07
#define FIRMWARE_MAIN_OPERATE_CODE_GET_INFO					0x03
#define FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE				0x04
#define FIRMWARE_SUB_OPERATE_CODE_GET_VCU_TYPE				0x01

#define FIRMWARE_COMPLETE_GET_VCU_VERSION					0x50

#define FIRMWARE_SUB_OPERATE_CODE_GET_VCU_VERSION			0x80
#define FIRMWARE_SUB_OPERATE_CODE_GET_BOOT_VERSION			0x81
#define FIRMWARE_SUB_OPERATE_CODE_GET_MAIN_VERSION			0x82
#define FIRMWARE_SUB_OPERATE_CODE_GET_USER_VERSION			0x83
#define FIRMWARE_SUB_OPERATE_CODE_GET_CAMERA_VERSION		0x81
#define FIRMWARE_SUB_OPERATE_CODE_GET_CPU_VERSION			0x02  //add

#define FIRMWARE_SUB_OPERATE_CODE_READ_BOOT_FILE			0x01
#define FIRMWARE_SUB_OPERATE_CODE_READ_MAIN_FILE			0x02
#define FIRMWARE_SUB_OPERATE_CODE_READ_USER_FILE			0x03
#define FIRMWARE_SUB_OPERATE_CODE_READ_CAMERA_FILE			0x01
#define FIRMWARE_SUB_OPERATE_CODE_READ_NORAML_FILE			0x00 //读取刚刚下发bin文件操作码

#define FIRMWARE_UNCONTROL_GET_BOOT_VERSION					0x41
#define FIRMWARE_UNCONTROL_GET_MAIN_VERSION					0x42
#define FIRMWARE_UNCONTROL_GET_USER_VERSION					0x43
#define FIRMWARE_UNCONTROL_GET_CAMERA_VERSION				0x41

#define FIRMWARE_RESET_OPERATE_CODE_CALLBACK				0x90						


#define FIRMWARE_RW_FILE_BLOCK_LENGTH						8

enum FIRMWARE_SOFTWARE_TYPE
{
	BOOT_SOFTWARE = 0,
	MAIN_SOFTWARE,
	USER_SOFTWARE,
	CAMERA_SOFTWARE,
	COMPARE_SOFTWARE//此项表示读取刚刚下发的文件
};

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

enum TABLEVIEW_BTN
{
	Btn_ReGet = 0x01,			//重新获取
	Btn_Retry,						//重试
	Btn_Degrade						//降级
};

struct recv_vcu_data : motion::asio_data{
	std::string remote_ip_;
	unsigned int data_length_;
	std::string data_context_;
};

struct recv_bin_data : motion::asio_data{
	unsigned int offset_;
	unsigned int data_length_;
	std::string data_context_;
};