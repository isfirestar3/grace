#include "version.h"

#include "var.h"
#include "mnt.h"
#include "vehicle.h"
#include "navigation.h"
#include "nav.h"
#include "protect.h"

#include "drive_unit.h"

#include "logger.h"
#include "nsp.h"

#include "canio.h"
#include "args.h"

#if !_WIN32
#include "gzdrive.h"
#include "regist_event.h"
#include <signal.h>
#endif

#include "error.h"

#include "posix_wait.h"
#include "posix_time.h"
#include "posix_thread.h"
#include "clist.h"

#include <time.h>

#include "memdump.h"

int run__interval_control(posix__waitable_handle_t *waiter, uint64_t begin_tick, uint32_t maximum_delay) {
    uint32_t tsc;
    int interval = (int) (posix__gettick() - begin_tick);
    if (interval < 0) {
        return interval;
    }

    if (interval >= (int) maximum_delay) {
        return interval;
    }

    tsc = (uint32_t) (maximum_delay - interval);
    if (waiter) {
        posix__waitfor_waitable_handle(waiter, tsc);
    }

    return interval;
}

#pragma pack(push, 1)

typedef struct {
    struct list_head link_;
    void( *func_)(void *functional_object);
} canio__event_callback_t;

#pragma pack(pop)

int run__algorithm_traj_control(void *memory_dump_object) {
    int retval = -1;

    posix__boolean_t sim = run__getarg_simflag();
    var__navigation_t *nav, *onav;
    var__vehicle_t *veh;
	uint64_t ts0,ts1,ts2,ts3,ts4,ts5,ts6,ts7,ts8;
	ts0 = ts1 = ts2 = ts3 = ts4 = ts5 = ts6 = ts7 = ts8 = 0;
	ts0 = posix__clock_gettime();

#if !_WIN32
	var__vehicle_t *oveh;
	/* VCU 环境下的导航前仿真流程*/
	if (sim) {
		oveh = var__get_vehicle();
		/* 命令速度作为反馈速度 */
		memcpy(&oveh->i.actual_velocity_, &oveh->i.command_velocity_, sizeof(velocity_t));
		/* 底盘时间戳使用系统时间戳 */
		oveh->i.time_stamp_ = posix__clock_gettime();
		oveh->i.time_stamp_ /= 10000;
		var__release_object_reference(oveh);
	}
#endif

	ts1 = posix__clock_gettime();
    veh = var__create_vehicle_dup();
	ts2 = posix__clock_gettime();
    nav = var__create_navigation_dup(0);
	ts3 = posix__clock_gettime();

#if 0
    /* 每一个导航周期， 均对重要内存数据进行详细记录, 这里至少记录了底盘和导航的数据 */
    if (memory_dump_object) {
        add_memory_record(memory_dump_object, &veh->manual_velocity_, sizeof (var__vehicle_t) - offsetof(var__vehicle_t, manual_velocity_));
        add_memory_record(memory_dump_object, &nav->track_status_, sizeof (var__navigation_t) - offsetof(var__navigation_t, track_status_));
        move_memory_record_tonext(memory_dump_object);
    }
#endif
	
    retval = nav__traj_control(nav, veh, var__get_driveunit(NULL), sim);
	ts4 = posix__clock_gettime();
	
	/* 导航后的仿真流程, 因为需要将数据提交到导航的非提交区域， 
		所以必须引用导航的原始对象
		递推 odo meter 仿真反馈给导航定位
		底盘时间戳仿真反馈给导航定位
		定位置信度满 */
#if _WIN32
	if (sim && retval >= 0) {
		onav = var__get_navigation();
		onav->pos_.x_ = veh->i.odo_meter_.x_;
        onav->pos_.y_ = veh->i.odo_meter_.y_;
        onav->pos_.angle_ = veh->i.odo_meter_.angle_;
        onav->pos_time_stamp_ = veh->i.time_stamp_;
        onav->pos_confidence_ = 1;
		var__release_object_reference(onav);
	}
#endif
	
    var__commit_vehicle_dup(veh);
	ts5 = posix__clock_gettime();
    var__commit_navigation_dup(nav);
	ts6 = posix__clock_gettime();

    var__release_vehicle_dup(veh);
	ts7 = posix__clock_gettime();
    var__release_navigation_dup(nav);
	ts8 = posix__clock_gettime();
	
#if !_WIN32
	if (sim && retval >= 0) {
		static double delta_t = 0.02;
		oveh = var__get_vehicle();
		oveh->i.odo_meter_.theta_ += oveh->i.actual_velocity_.w_ * delta_t;
		oveh->i.odo_meter_.x_ += oveh->i.actual_velocity_.x_ * cos(oveh->i.odo_meter_.theta_) * delta_t;
		oveh->i.odo_meter_.y_ += oveh->i.actual_velocity_.x_ * sin(oveh->i.odo_meter_.theta_) * delta_t;
		var__release_object_reference(oveh);
		
		onav = var__get_navigation();
		onav->pos_.x_ = veh->i.odo_meter_.x_;
        onav->pos_.y_ = veh->i.odo_meter_.y_;
        onav->pos_.angle_ = veh->i.odo_meter_.angle_;
        onav->pos_time_stamp_ = veh->i.time_stamp_;
        onav->pos_confidence_ = 1;
		var__release_object_reference(onav);
	}
#endif

	if ((ts8 - ts0) > 500000) {
		log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem, "algorithm_traj_control timeout. %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
					ts0, ts1, ts2, ts3, ts4, ts5, ts6, ts7, ts8);
	}
		
    return retval;
}

void *run__safety_proc(void *argv) {
    posix__waitable_handle_t waiter;
    uint64_t begin_tick;
    static const uint32_t CUSTOM_INTERVAL = 50;

    if (posix__init_synchronous_waitable_handle(&waiter) < 0) {
        return NULL;
    }

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "safety loop success startup.");

    while (1) {
        begin_tick = posix__gettick();

        if (safety__proc() < 0) {
            log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem | kLogTarget_Stdout, "safety loop terminated cause by executive error.");
            break;
        }

        run__interval_control(&waiter, begin_tick, CUSTOM_INTERVAL);
    }

    log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem | kLogTarget_Stdout, "safety loop exit.");
    posix__uninit_waitable_handle(&waiter);
    return NULL;
}

void *run__guard_proc(void *argv) {
    posix__waitable_handle_t waiter;
    uint64_t begin_tick;
    static const uint32_t GUARD_INTERVAL = 50;
    var__error_handler_t *err;
    int fixed_object_count;
    int *fixed_object_ids = NULL;
    int i;
    var__vehicle_t *veh;
    int fault, previous_fault;

    previous_fault = 0;

    if (posix__init_synchronous_waitable_handle(&waiter) < 0) {
        return NULL;
    }

    /* 预设在整个进程生命周期， 不会发生对象增减 */
    fixed_object_count = var__query_global_count();
    if (fixed_object_count <= 0) {
        return NULL;
    }
    fixed_object_ids = (int *) malloc(fixed_object_count * sizeof (int));
    if (!fixed_object_ids) {
        return NULL;
    }
    var__query_global_object_ids(fixed_object_count, fixed_object_ids);

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "guard loop success startup.");

    while (1) {
        begin_tick = posix__gettick();

        err = var__get_error_handler();
        if (!err) {
            break;
        }

        /* Traversal all objects and lookup errors */
        fault = 0;
        for (i = 0; i < fixed_object_count; i++) {
            if ( err->error_[fixed_object_ids[i]].status < 0 ) {
                if (0 != err->error_[fixed_object_ids[i]].framwork){
                    log__save("motion_template", kLogLevel_Warning, kLogTarget_Stdout | kLogTarget_Filesystem, 
                        "error object id=%u, framwork error=%d", fixed_object_ids[i], err->error_[fixed_object_ids[i]].framwork);
                    fault |= VEH_SOFTWARE_FAULT;
                }
                if (0 != err->error_[fixed_object_ids[i]].software) {
                    log__save("motion_template", kLogLevel_Warning, kLogTarget_Stdout | kLogTarget_Filesystem, 
                        "error object id=%u, software error=%d", fixed_object_ids[i], err->error_[fixed_object_ids[i]].software);
                    fault |= VEH_SOFTWARE_FAULT;
                }
                if (0 != err->error_[fixed_object_ids[i]].hardware) {
                    log__save("motion_template", kLogLevel_Warning, kLogTarget_Stdout | kLogTarget_Filesystem, 
                        "error object id=%u, hardware error=0x%08X", fixed_object_ids[i], err->error_[fixed_object_ids[i]].hardware);
                    fault |= VEH_HARDWARE_FAULT;
                }
                break;
            }
        }
        var__release_object_reference(err);

        /* any software/hardware error detected, then enable vehicle fault stop*/
        if (fault) {
            veh = var__get_vehicle();
            if (veh) {
                previous_fault = veh->fault_stop_;
                veh->fault_stop_ |= fault;
                var__release_object_reference(veh);
            }

            if (VEH_HEALTHY == previous_fault) {
                log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem | kLogTarget_Stdout, "guard thread, fault stop=0x%08X", fault);
            }
        }

        run__interval_control(&waiter, begin_tick, GUARD_INTERVAL);
    }

    log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem | kLogTarget_Stdout, "guard loop exit.");
    posix__uninit_waitable_handle(&waiter);
    free(fixed_object_ids);

    return NULL;
}

void *run__navigation_proc(void *argv) {
    posix__boolean_t navigation_error_as_fatal;
    posix__waitable_handle_t *monitor = (posix__waitable_handle_t *) argv;
    posix__waitable_handle_t waiter;
    uint64_t begin_tick;
    var__error_handler_t *err;
    static const uint32_t NAVIGATION_INTERVAL = 20;
    void *memory_dump_object;

    if (posix__init_synchronous_waitable_handle(&waiter) < 0) {
        return NULL;
    }

    /* 从错误配置中读取，是否因为导航计算失败而停车 */
    err = var__get_error_handler();
    if (!err) {
        posix__uninit_waitable_handle(&waiter);
        return NULL;
    }
    navigation_error_as_fatal = err->navigation_error_as_fatal_;
    var__release_object_reference(err);

    /*  创建内存记录对象, 不成功也不影响进程运行 */
    memory_dump_object = NULL;
    if (allocate_memory_dump(&memory_dump_object) < 0) {
        memory_dump_object = NULL;
    }

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"navigation loop success startup.");

    while (1) {
        begin_tick = posix__gettick();

        if (run__algorithm_traj_control(memory_dump_object) < 0) {
            log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem | kLogTarget_Stdout, "navigation loop executive fault.");
            if (navigation_error_as_fatal) {
                var__mark_framwork_error(kVarFixedObject_Navigation, var__make_error_code(kVarType_Navigation, kFramworkFatal_NavigationExecutiveFault));
            }
        }

#if _WIN32
        mnt__executive_page_fault();
#endif

        posix__sig_waitable_handle(monitor);

        run__interval_control(&waiter, begin_tick, NAVIGATION_INTERVAL);
    }

    log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem | kLogTarget_Stdout, "navigation loop exit.");

    return NULL;
}

static
void run__app_exit(void) {
    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "app atexit called.process is terminated.");
}

#if !_WIN32

static
void sighandler(int signum) {
    if (SIGPIPE == signum) {
        log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "SIGPIPE detected.with networking HUP?");
        return;
    }

    if (SIGINT == signum) {
        nsp__uninit_network();
		nspi__uninit_regist_cycle_event();
        exit(0);
    }
}
#endif

void mtecr(const char *host_event, const char *reserved, int rescb) {
    if (host_event) {
        log__save("nshost", kLogLevel_Trace, kLogTarget_Filesystem, "%s", host_event);
    }
}

int main(int argc, char **argv) {
    var__error_handler_t *err;
    int navigation_timeout_as_fatal;
    posix__pthread_t navigation_tp, safty_tp, guard_tp;
    posix__waitable_handle_t monitor;
    int retval;
    static const char startup_message[] = POSIX__EOL
            "****************************************************************************"POSIX__EOL
            "*                            application startup                           *"POSIX__EOL
            "****************************************************************************";

#if !_WIN32
    /* Ctrl + C 控制进程完整安全的退出 
        signal(SIGINT, &sighandler);
     */
    signal(SIGPIPE, &sighandler);
    /* 为了不随终端关闭， 是否应该增加这个处理？ 能否真正替代 nohup(1)? */
    signal(SIGHUP, SIG_IGN);
#endif

    /* 检查参数并执行异化  */
    if (run__check_args(argc, argv) < 0) {
        return 1;
    }

    /* 输出启动信息 */
    log__save("motion_template", kLogLevel_Info, kLogTarget_Stdout | kLogTarget_Filesystem, startup_message);

    /* 设置退出例程 */
    atexit(&run__app_exit);

    if (posix__init_synchronous_waitable_handle(&monitor) < 0) {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "failed to create synchronous monitor event.");
        return 1;
    }

    srand((unsigned int) time(NULL));

#if _WIN32
    if (canio__register_event_callback(&nsp__canio_msg) < 0) {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "failed to regist callback address from drive shared object.");
        return 1;
    }
#endif

    objinit();

    /* 在加载其他任何对象之前， 加载错误处理对象, 且不允许失败 */
    if (var__load_error_handler_object() < 0) {
        return 1;
    }
    /* 获取错误配置中的相关参数 */
    err = var__get_error_handler();
    if (!err) {
        return 1;
    }
    navigation_timeout_as_fatal = err->navigation_timeout_as_fatal_;
    var__release_object_reference(err);

    var__load_dev_configure();
    var__load_var_configure();
    mnt__load_setting();

    /* 开始启动网络服务 */
#ifndef _WIN32
    nis_checr(&mtecr);
#endif

    tcp_init();
    udp_init();
    if (nsp__init_network() < 0) {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "failed to startup network service.");
        return 1;
    }

    /* 尝试连接事件服务器 
    char essnhost[posix__ipv4_length];
    run__getarg_essnhost(essnhost);
    gesn__config_server(essnhost, run__getarg_essnport());
    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "motion_template process startup.");*/

    /* arm 环境需要建立和M核的通道 */
#if !_WIN32
    nsp__init_gzd_object();
	nspi__init_regist_cycle_event();
#endif

    posix__pthread_create(&navigation_tp, &run__navigation_proc, &monitor);

    /* 启动安全防护过程 */
    if (safety__init() >= 0) {
        posix__pthread_create(&safty_tp, &run__safety_proc, NULL);
    } else {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "failed to init safety thread.");
    }

    /* 启动错误检测线程 */
    posix__pthread_create(&guard_tp, &run__guard_proc, NULL);

    while (1) {
        retval = posix__waitfor_waitable_handle(&monitor, (uint32_t) navigation_timeout_as_fatal);
        if ((ETIMEDOUT == retval) && (navigation_timeout_as_fatal > 0)) {
            log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem | kLogTarget_Stdout, "navigation loop keepalive test timeout.");
            var__mark_framwork_error(kVarFixedObject_Navigation, var__make_error_code(kVarType_Navigation, kFramworkFatal_WorkerThreadNonResponse));
        } else {
            if (retval < 0) {
                log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem | kLogTarget_Stdout, 
						"navigation loop keepalive test terminated.cause by waitable handle error.");
                break;
            }
        }
    }

    return 0;
}