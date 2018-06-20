#pragma once

#include <stdint.h>

#ifdef _USRDLL
#define METH dllexport
#else
#define METH dllimport
#endif

#ifndef EXP
#if _WIN32
#define EXP(type)   _declspec(METH) type 
#else
#define EXP(type)   type
#endif
#endif
#pragma pack(push, 1)

typedef struct{
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
}config_parameter_t;

typedef struct
{
	uint8_t command_code_;
	uint8_t empty1_[3];
	uint16_t length_;
	char  module_type_[32];
	uint32_t check_code_;
	uint8_t data_;
}control_message_t;

typedef struct {
	double FocalLength_1;
	double FocalLength_2;
	double PrincipalPoint_1;
	double PrincipalPoint_2;
	double RadialDistortion_1;
	double RadialDistortion_2;
	double RadialDistortion_3;
	double TangentialDistortion_1;
	double TangentialDistortion_2;
}parameter_data_t;

typedef struct{
	uint8_t comm_code_;
	uint8_t empty_[3];
	uint32_t length_;
	char model_[32];
	uint8_t	parameter_data_[72];
}parameter_post_t;

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

typedef struct packet_config_parameter{
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
}packet_config_parameter;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif
	//初始化网络包
	EXP(int) init_net_session(const char *module_address, int(*camera_rcv_callback_t)(void *image, const unsigned char *pic));

	//发送配置信息
	EXP(int) post_config_info(packet_config_parameter config_info);

	//发送控制报
	EXP(int) post_control_info(uint8_t control_info);

	//发送内参数据
	EXP(int) post_parameter_info(char* parameter_path, parameter_data_t parameter_data);

	EXP(void) query_firmware_info(char *name, int *namelen, uint32_t *checksum);

	EXP(uint8_t) query_status();
	//设置发送图片信息回调
	//EXP(void) register_image_callback(int(*notify_image_callback)(const unsigned char* bytes, int id, int len, uint32_t width, uint32_t high, uint64_t time_stamp));
#ifdef __cplusplus
}
#endif
