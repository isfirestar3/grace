#if !defined DEF_OPERATION_H
#define DEF_OPERATION_H

#include "vartypes.h"

enum operation_code_mean_t {
    kOperationCode_QueryParameter = 10,
};

#pragma pack(push,1)

typedef struct _var__operation {
    // 状态迁移
    /*+000*/ var__status_describe_t status_;
    
    uint64_t user_task_id_;
    uint64_t ato_task_id_;

    // 可写(数据下行,可作为绑定目标)
    /*+012*/ int code_; //操作类型（代码, 其中 10 为 LayoutDesigner 要求车载程序获取定制任务）

    /*+016*/ uint64_t param0_; //操作参数
    /*+024*/ uint64_t param1_;
    /*+032*/ uint64_t param2_;
    /*+040*/ uint64_t param3_;
    /*+048*/ uint64_t param4_;

    /*+056*/ uint64_t param5_;
    /*+064*/ uint64_t param6_;
    /*+072*/ uint64_t param7_;
    /*+080*/ uint64_t param8_;
    /*+088*/ uint64_t param9_;

    // 提交(数据上行， 可作为绑定源)
    union {
#if __cplusplus
        struct {
#else
        struct _i_operation_internal_t {
#endif
            /*+096*/ uint64_t param10_;
            /*+104*/ uint64_t param11_;
            /*+112*/ uint64_t param12_;
            /*+120*/ uint64_t param13_;
            /*+128*/ uint64_t param14_;

            /*+136*/ uint64_t param15_;
            /*+144*/ uint64_t param16_;
            /*+152*/ uint64_t param17_;
            /*+160*/ uint64_t param18_;
            /*+168*/ uint64_t param19_;

            uint64_t current_task_id_; // current running task id
        } i;

        char u_;
    };
} var__operation_t;

#pragma pack(pop)

#if !defined __cplusplus

#include "object.h"

extern
int var__load_opt();

/* 根据设备句柄或者设备ID获取 opt 设备所指向的 opt 对象原始数据区指针，必须手动  var__release_object_reference 释放
 */
extern
var__operation_t *var__get_opt();

/* 创建一份指定句柄或者ID的 opt 设备对象的拷贝, 使用完成后必须手动 var__release_opt_dup 释放
 */
extern
var__operation_t *var__create_opt_dup();

/* 将籍由 var__create_opt_dup 或  var__create_opt_dup_byid 获得的 opt 对象拷贝提交到原始数据区
 */
extern
int var__commit_opt_dup(const var__operation_t *opt);

/* 释放 var__create_opt_dup 或 var__create_opt_dup_byid 所得的 opt 对象拷贝
 */
extern
void var__release_opt_dup(var__operation_t *opt);

#endif

#endif