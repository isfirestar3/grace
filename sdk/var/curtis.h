#if !defined VAR_CURITS_H
#define VAR_CURITS_H

#include "vartypes.h"

#pragma pack(push, 1)

typedef struct {
    /*+000*/ var__can_device_t candev_head_;

    /* 全局只读(CURITS 配置属性) */
    /*+036*/ int internel_type_; // 柯蒂斯内部类型， 此类型框架不关心， 但是需要从配置文件获取
    /*+040*/ int send_id_;
    /*+044*/ int recv_id_;
    /*+048*/ enum var__byte_orders_t byte_order_;

    /* 可写(数据下行,可作为绑定目标) */
    /*+052*/ posix__boolean_t enable_;
    /*+049*/ uint8_t control1_;
    /*+050*/ uint8_t control2_;
    /*+051*/ int64_t command_velocity_; // 目标速度
    /*+059*/ int64_t command_angle_; // 目标角度
    /*+067*/ int64_t command_lift_speed1_; // 目标叉速度
    /*+075*/ int64_t command_lift_speed2_; // 目标叉速度
    /*+083*/ int64_t command_lift_speed3_; // 目标叉速度
    /*+091*/ int64_t command_lift_speed4_; // 目标叉速度

    // 提交(数据上行， 可作为绑定源)
    union {
#if __cplusplus
        struct {
#else
        struct _i_curtis_internal_t {
#endif 
            /*+099*/ int node_state_;
            /*+103*/ int error_code_;
            /*+107*/ int64_t actual_velocity_;
            /*+115*/ int64_t actual_angle_;
            /*+123*/ uint64_t timestamp_;
            /*+131*/ posix__boolean_t enabled_;
            /*+135*/ int voltage_;
            /*+139*/ uint8_t control_feedback_;
        } i;

        char u_;
    };
} var__curtis_t; /*+088*/

#pragma pack(pop)

#if !defined __cplusplus

#include "object.h"

extern
int var__load_curtis();

/* 根据设备句柄或者设备ID获取 curtis 设备所指向的 curtis 对象原始数据区指针，必须手动  var__release_object_reference 释放
 */
extern
var__curtis_t *var__get_curtis(objhld_t hld);
extern
var__curtis_t *var__get_curtis_byid(int id);

/* 创建一份指定句柄或者ID的 curtis 设备对象的拷贝, 使用完成后必须手动 var__release_curtis_dup 释放
 */
extern
var__curtis_t *var__create_curtis_dup(objhld_t hld);
extern
var__curtis_t *var__create_curtis_dup_byid(int id);

/* 将籍由 var__create_curtis_dup 或  var__create_curtis_dup_byid 获得的 curtis 对象拷贝提交到原始数据区
 */
extern
int var__commit_curtis_dup(const var__curtis_t *curits);

/* 释放 var__create_curtis_dup 或 var__create_curtis_dup_byid 所得的 curtis 对象拷贝
 */
extern
void var__release_curtis_dup(var__curtis_t *curits);

#endif

#endif