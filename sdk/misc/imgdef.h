/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   imgdef.h
 * Author: anderson
 *
 * Created on July 27, 2017, 3:47 PM
 */

#if !defined IMGDEF_H
#define IMGDEF_H

#include <stdint.h>

typedef int (*camera_rcv_callback_t)(void *image, const unsigned char *pic);

#pragma pack(push, 1)

struct gzimage_t {
    uint16_t image_id; // 硬件定义的图片ID
    uint16_t image_current_bytes; // 已经拷贝的总长度(字节), 
    uint16_t image_total_bytes; // 总长度(字节)
    uint16_t image_width;   // 原始图宽
    uint16_t image_high;    // 原始图高
    uint64_t image_timestamp;   // 相对时间戳

    uint64_t image_top_recv_ts; // 头帧接收时间
    uint64_t image_buttom_recv_ts; // 尾帧接收时间
    uint64_t image_pkt_adjust_ts; // 首帧包中的调整时间
    uint64_t imgae_exec_ts; // 执行时间
};

#define CMD_CFGPAR_ORDER        (0x20)

#define OUTPUT_TYPE_JPEG        (1)
#define OUTPUT_TYPE_BMP         (2)


#define CMD_CFGPAR_ACK      (0xA0)
struct packet_config_parameter_ack {
    uint8_t command;
    uint8_t error;
    uint8_t reserved[2];
    uint16_t length;
    char module[32];
    uint32_t checksum;
};

#define CMD_CTRL_FIRMWARE       (0x30)
struct packet_control_firmware {
    uint8_t command;
    uint8_t reserved[3];
    uint16_t length;
    char module[32];
    uint32_t checksum;
    uint8_t data;
};

#define FIRMWARE_CTRLCODE_STOP_CAP          (0)
#define FIRMWARE_CTRLCODE_START_CAP         (1)
#define FIRMWARE_CTRLCODE_INTERLACING       (2)  // 切换到隔行隔列模式, 1位为0表示全像素模式

#define CMD_FIRMWARE_STATUS_REPORT     (0xB0)


struct packet_firmware_status_report {
    uint8_t command;
    uint8_t reserved[3];
    uint16_t length;
    char module[32];
    uint32_t checksum;
    uint8_t data;
};

#define FIRMWARE_REPORT_STATUS_RUNNING      (1)
#define FIRMWARE_REPORT_STATUS_INTERLACING  (2)
#define FIRMWARE_REPORT_STATUS_WARNING      (0x20)
#define FIRMWARE_REPORT_STATUS_FATAL        (0x40)
#define CMD_CAMREA_PARAMETER				(0x32)   //内参数据
#define CMD_PARAMETER_ACK					(0xB2)   //内参数据回包
struct packet_head {
    uint16_t image_id; // 图片序号
    uint16_t frame_id; // 图帧序号
    uint16_t frame_size; // 图帧长度
};

struct packet_zero_frame {
    struct packet_head head;
    uint8_t type; // 输出图片格式	1 - JPEG格式，2 - 原图灰度格式
    uint8_t tstype; // 时间戳形式	0 - 相对时间戳，1 - 绝对时间戳
    uint16_t timestamp; // 时间戳	从开始拍照，到开始传输此数据帧的时间差，单位us
    uint8_t reserved[8]; // 保留字段
    uint16_t width;
    uint16_t high;
    uint32_t imgsize; // 图片总字节数
    uint8_t reserved2[64];
    uint8_t data[0];
};

struct packet_frame {
    struct packet_head head;
    uint8_t data[0];
};

struct packet_frame_ack {
    uint16_t image_id;
    uint32_t fatal_frame_count;         // 0 - 所有帧接收正常 非0 - 中间有数据帧丢失，值为丢失的帧数量
    uint16_t re_translate_order[0];     // 重传帧数组, 长度 = fatal_frame_count * 2
};

struct config_parameter_t{
	uint8_t  command_code_;
	uint8_t  empty1_[3];
	uint16_t length_;
	char	 module_type_[32];
	uint32_t check_code_;
	uint8_t  image_type_;
	uint8_t  image_quality_;
	uint16_t time_interval_;
	uint32_t time_exposure_;
	uint8_t  light_type_;
	uint8_t  gain_factor_;
	uint8_t  sampling_;
	uint8_t  timestamp_;
};

struct control_message_t
{
	uint8_t command_code_;
	uint8_t empty1_[3];
	uint16_t length_;
	char  module_type_[32];
	uint32_t check_code_;
	uint8_t data_;
};

struct parameter_data_t{
	double FocalLength_1;
	double FocalLength_2;
	double PrincipalPoint_1;
	double PrincipalPoint_2;
	double RadialDistortion_1;
	double RadialDistortion_2;
	double RadialDistortion_3;
	double TangentialDistortion_1;
	double TangentialDistortion_2;
};

struct parameter_post_t{
	uint8_t comm_code_;
	uint8_t empty_[3];
	uint32_t length_;
	char model_[32];
	uint8_t	parameter_data_[72];
};


struct packet_config_parameter{
	uint8_t command;       //命令码	0x20
	uint8_t reserved[3];
	uint16_t length;           // 长度	组态校验码之后的数据长度
	char module[32];      // 模块型号	ASCII码，“IPC36M”
	uint32_t checksum;      // 配置参数校验码  配置参数校验码计算方法：将此后12字节的配置参数，按uint32_t进行累加求和，若累加和为0则取反。计算结果即为配置参数校验码。
	uint8_t output_type;    // 输出图片格式	1 - JPEG格式，2 - 原图灰度格式
	uint8_t image_quality; // 图片质量	1-100，JPEG压缩率，在JPEG格式下有效
	uint16_t cap_interval; // 拍摄间隔时间 非0 - 连续拍摄的时间间隔，单位ms
	//全像素拍摄下小于30ms按30ms处理
	//隔行隔列拍摄下小于20ms按20ms处理
	//0 - 触发型拍摄
	uint32_t time_exposure; // 曝光时间	0 - 自动曝光；非0 - 曝光时间，单位us
	uint8_t light;              // 补光方式	0 - 不补光，1 - 同步闪光，2 - 常亮
	uint8_t gain_factor;  // 增益倍数 
	// 0 - 自动增益 非0 - 手动增益，其中16~64 时除以16后为增益倍数，1~15按16处理，大于64按64处理    
	uint8_t sampling;      // 降采样
	uint8_t time_recode;  // 时间戳类型 0 - 使用相对时间戳 1 - 启动1588协议，使用绝对时间戳
};

#pragma pack(pop)

#endif /* IMGDEF_H */

