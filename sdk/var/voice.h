#if !defined VAR_MUSIC_CAN_H
#define VAR_MUSIC_CAN_H

#include "vartypes.h"

#pragma pack(push, 1)

typedef struct {
    /*+000*/ var__can_device_t candev_head_;
    /*+036*/ int send_id_;
    /*+044*/ int recv_id_;

    /* 可写(数据下行,可作为绑定目标) */
    /*+052*/ posix__boolean_t enable_;
    /*+056*/ uint8_t music_id_; //音乐id
    /*+057*/ uint8_t music_volume_; //音量
    /*+058*/ uint8_t byte0_;
    /*+059*/ uint8_t byte1_;
    /*+060*/ uint8_t byte2_;
    /*+061*/ uint8_t byte3_;
    /*+062*/ uint8_t byte4_;
    /*+063*/ uint8_t byte5_;
    /*+064*/ uint8_t byte6_;
    /*+065*/ uint8_t byte7_;

    // 提交(数据上行， 可作为绑定源)

    union {
#if __cplusplus
        struct {
#else
        struct _i_voice_device_internal_t {
#endif 
            /*+066*/ int node_state_;
            /*+070*/ int error_code_;
            /*+074*/ int64_t actual_music_id_;
            /*+082*/ int64_t actual_music_volume_;
            /*+090*/ uint64_t timestamp_;
            /*+098*/ posix__boolean_t enabled_;
        } i;

        char u_;
    };
} var__voice_device_t; /*+102*/

#pragma pack(pop)

#if !defined __cplusplus

#include "object.h"

extern
int var__load_voice_device();

/* 根据设备句柄或者设备ID获取 music_device 设备所指向的 music_device 对象原始数据区指针，必须手动  var__release_object_reference 释放
 */
extern
var__voice_device_t *var__get_voice_device(objhld_t hld);
extern
var__voice_device_t *var__get_voice_device_byid(int id);

/* 创建一份指定句柄或者ID的 music_device 设备对象的拷贝, 使用完成后必须手动 var__release_music_device_dup 释放
 */
extern
var__voice_device_t *var__create_voice_device_dup(objhld_t hld);
extern
var__voice_device_t *var__create_voice_device_dup_byid(int id);

/* 将籍由 var__create_music_device_dup 或  var__create_music_device_dup_byid 获得的 music_device 对象拷贝提交到原始数据区
 */
extern
int var__commit_voice_device_dup(const var__voice_device_t *music_device);

/* 释放 var__create_music_device_dup 或 var__create_music_device_dup_byid 所得的 music_device 对象拷贝
 */
extern
void var__release_voice_device_dup(var__voice_device_t *music_device);

#endif

#endif