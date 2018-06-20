#if !defined VAR_ERRORS_H
#define VAR_ERRORS_H
#endif

#include "vartypes.h"
#include "var.h"
#include "errtypes.h"

/* 
 *  重要:
 * 1. 错误码次高位为1，表示为致命错误， 调用框架例程 var__mark_framwork_error/var__mark_hardware_error 会自动置 condition_code_ 为 -1
 * 2. 错误码最高位为1， 表示为错误， 0表示为非错误
*/

#define VAR_ERROR_MASK          (0x80000000)
#define VAR_FATAL_ERROR_MASK    (0xC0000000)

#pragma pack(push, 1)

typedef struct {
    int status;
    int framwork;   /* MT 框架层面的错误 */
    int software;   /* 软件层面错误 */
    int hardware;   /* 硬件层面的错误 */
}var__error_item_t;

typedef struct {
    /* 框架错误的配置信息 */
    int navigation_timeout_as_fatal_;           /* 导航循环超时认定为错误， <=0 则忽略此项 */
    posix__boolean_t navigation_error_as_fatal_;
    int busrw_timeout_as_fatal_;
    int driverrw_timeout_as_fatal_;
    int average_request_completion_delay_as_fatal_;
    int average_request_checking_window_if_enabled_;
    posix__boolean_t tcp_master_object_destoryed_as_fatal_;
    posix__boolean_t process_scheduler_interrupted_as_fatal_;
    posix__boolean_t valloc_fault_as_fatal_;
    
    /* CAN总线驱动相关配置信息 */
    int driver_err_frame_window_;                  //can驱动错误检测窗口大小
    int driver_err_cons_lost_cnt_;                 //can驱动连续丢包阈值
    double driver_err_thres_framelost_;            //can驱动窗口内丢包率阈值
    double driver_err_speed_follow_error_;          /* 速度跟踪错误 */
    double driver_err_speed_min_acc_;               /* 最小加速度 */
    
    /* 导航错误 */
    double localization_timeout_;                   /* 定位超时， 单位 s */
    double odo_timeout_;                            /* 里程超时， 单位 s */
    double average_localization_delay_threshold_;   /* 定位平均超时， 单位 s */
    double average_odo_delay_threshold_;            /* 里程平均超时， 单位 s */
    
    var__error_item_t error_[MAXIMUM_OBJECT_INDEX];
}var__error_handler_t;

#pragma pack(pop)

extern
int var__load_error_handler_object();

/* 获取错误处理器的指针， 必须手动  var__release_object_reference 释放
 */
extern
var__error_handler_t *var__get_error_handler();

extern
void var__mark_framwork_error(int fixed_object_id, int error_code);
extern
void var__mark_software_error(int fixed_object_id, int error_code);
extern
void var__mark_hardware_error(int fixed_object_id, int error_code);
extern
int var__clear_fault();