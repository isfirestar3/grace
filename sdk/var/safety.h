#ifndef __VAR_SAFETY_H__
#define __VAR_SAFETY_H__

#include "vartypes.h"

enum em_safety_protect_output_t {
    kSafetyProtectOutput_None = -1,
    kSafetyProtectOutput_Normal = 0,
    kSafetyProtectOutput_Slowdown,
    kSafetyProtectOutput_Stop,
};

enum em_safety_protect_index {
    kSafetyProtectIdex_MainIndex = 0,
    kSafetyProtectIdex_SubIndex,
    kSafetyProtectIdex_SsubIndex,

    kSafetyProtectIdex_MaxNum,
};

enum safety_dev_data_types {
    safety_data_type_digital = 0,
    safety_data_type_analog,
    safety_data_type_analogword,
};

#pragma pack(push, 1)

typedef struct {
    int __src_dev_id;
    enum var__types __src_dev_type;
    int __data_src_channel[kSafetyProtectIdex_MaxNum];
    enum safety_dev_data_types __dev_data_type;
    enum em_safety_protect_output_t __reslut;
	char sensor_name[20];
    union {
        int __di_mask;
        double __ai_thres;
    };

    union {
        int __di_cur;
        double __ai_cur;
    };
} st_safety_dev_bank_src_info;

typedef struct {
    //控制
    int enable_;

    //反馈
    int enabled_; 
    int cur_bank_id_;
    int cur_bank_level;
    enum em_safety_protect_output_t safety_reslut_;

    st_safety_dev_bank_src_info sensor_trrigered_;

    int manual_bank_id_;
    int manual_bank_level_;
} var__safety_t;

#pragma pack(pop)

extern
int var__load_safety();

/* 获取安全防护对象的指针， 必须手动  var__release_object_reference 释放
 */
extern
var__safety_t *var__get_safety();

#endif