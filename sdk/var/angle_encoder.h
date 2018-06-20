#if !defined VAR_ANGLE_ENCODE_H
#define VAR_ANGLE_ENCODE_H

/* 角度编码器 驱动相关类
 */

#include "vartypes.h"

enum angle_encoder_type_t {
    kAngleEncoderType_ABS = 0, // 绝对角度
    kAngleEncoderType_REL, // 相对角度
};

#pragma pack(push, 1)

typedef struct {
    // 全局只读
    /*+000*/ var__can_device_t candev_head_;

    /*角度编码器配置信息*/
    /*+036*/ enum angle_encoder_type_t encoder_type_;

    // 提交(数据上行， 可作为绑定源)
    union {
#if __cplusplus
        struct {
#else
        struct _i_angle_encoder_internal_t {
#endif 
            /*+040*/ int state_; // CAN OPEN STATES
            /*+044*/ int64_t actual_angle_; //反馈角度(Scaled)
            /*+052*/ int error_code_;
            /*+056*/ uint64_t time_stamp_;
        } i;

        char u_;
    };

} var__angle_encoder_t; /*+064*/

#pragma pack(pop)

#if !defined __cplusplus

#include "object.h"

extern
int var__load_angle_encoder();

/* 根据设备句柄或者设备ID获取 ae 设备所指向的 ae 对象原始数据区指针，必须手动  var__release_object_reference 释放
 */
extern
var__angle_encoder_t *var__get_ae(objhld_t hld);
extern
var__angle_encoder_t *var__get_ae_byid(int id);

/* 创建一份指定句柄或者ID的 ae 设备对象的拷贝, 使用完成后必须手动 var__release_ae_dup 释放
 */
extern
var__angle_encoder_t *var__create_ae_dup(objhld_t hld);
extern
var__angle_encoder_t *var__create_ae_dup_byid(int id);

/* 将籍由 var__create_ae_dup 或  var__create_ae_dup_byid 获得的 ae 对象拷贝提交到原始数据区
 */
extern
int var__commit_ae_dup(const var__angle_encoder_t *ae);

/* 释放 var__create_ae_dup 或 var__create_ae_dup_byid 所得的 ae 对象拷贝
 */
extern
void var__release_ae_dup(var__angle_encoder_t *ae);

#endif

#endif