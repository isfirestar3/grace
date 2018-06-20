#if !defined ERR_TYPES_H
#define ERR_TYPES_H

#if !defined ERROR_MASK
#define ERROR_MASK          (0xC0000000)
#endif /* !ERROR_MASK */

#if !defined WARNING_MASK
#define WARNING_MASK        (0x80000000)
#endif /* WARNING_MASK */

#if !defined var__make_error_code
#define var__make_error_code(var_type, code)    (code) //( ((int)(var_type) << 16) | ERROR_MASK | (unsigned short)(0xFFFF & (code)) )
#endif

#if !defined var__make_warning_code
#define var__make_warning_code(var_type, code)    (code) //( ((int)(var_type) << 16) | WARNING_MASK |  (unsigned short)(0xFFFF & (code)) )
#endif

enum framwork_error_t {
    kFramworkFatal_Common = 1, /* 通用严重错误 */
    kFramworkFatal_WorkerThreadNonResponse = 2, /* 工作线程无响应 */
    kFramworkFatal_AverageIOTimeout = 4, /* 平均 IO 超时 */
    kFramworkFatal_NavigationExecutiveFault = 010, /* 导航执行错误 */
    kFramworkFatal_TcpMasterServicesHalted = 020, /* TCP 主干服务被终止 */
    kFramworkFatal_CanbusNodeNonResponse = 040, /* CAN 总线节点无响应/创建节点失败 */
};

enum canbus_driver_error_t {
    kCanbusDriver_ErrorHardware = 1, //硬件错误
    kCanbusDriver_ErrorFrameCountExtend, //通信连续丢包
    kCanbusDriver_ErrorFrameWindowExtend, //通信丢包率过高
    kCanbusDriver_ErrorFrameSpeedFollowErr,
    kCanbusDriver_ErrorHoming,
    kCanbusDriver_WarnEmergencyStop = 0x100, //急停
};

enum nav_error_t {
    kNav_ErrorLocConfidence = 1,
    kNav_ErrorLocTimeOut,
    kNav_ErrorLocAverageTimeOut,
    kNav_ErrorTraj,
    kNav_ErrorTracking,
    kNav_ErrorNoMap,
    kNav_ErrorOdoTimeOut,
    kNav_ErrorOdoAverageTimeOut,
    kNav_ErrorLocNotInit,
};

enum loc_errot_t
{
    kLoc_ErrorOdoException = 1,
    kLoc_ErrorOdoTimeOut,
    kLoc_ErrorImuException,
    kLoc_ErrorImuTimeOut,
    kLoc_ObserveSensorException,
    kLoc_ObserveSensorTimeOut,
    kLoc_ObserveException,
    kLoc_ObserveNoneInThres,
    kLoc_MissCode,
    kLoc_SensorDataExtend,
};
#endif /* !ERR_TYPES_H */