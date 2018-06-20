#include "object.h"
#include "var.h"
#include "bus.h"
#include "xml_loaded.h"
#include "canio.h"
#include "entry.h"
#include "logger.h"
#include "error.h"

#include "posix_wait.h"
#include "posix_time.h"

#include "version.h"

typedef struct {
    void *dup_;
    canio__busarg_t argv_;
    drive_create_dup_t create_dup_proc_;
    drive_release_dup_t release_dup_proc_;
    objhld_t handle_;
    struct list_head entry_;
} vari__drive_io_t;

static
struct list_head __local = {0};

#if _WIN32

static
void *bus_rw(void *parameter) {
    uint64_t begin_tick;
    objhld_t hld = (objhld_t) parameter;
    var__functional_object_t **objs;
    canio__busarg_t argv;
    int i;
    posix__waitable_handle_t waiter;
    int busrw_timeout_as_fatal = 0, average_request_completion_delay_as_fatal = 0, average_request_checking_window_if_enabled = 0;
    var__error_handler_t *err;
    int interval, sum_interval_in_window = 0, average_checking_counter = 0;
    struct list_head *loop;
    vari__drive_io_t *rw;

    if (posix__init_synchronous_waitable_handle(&waiter) < 0) {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "canbus waitable handle init failed.");
        return NULL;
    }

    err = var__get_error_handler();
    if (!err) {
        posix__uninit_waitable_handle(&waiter);
        return NULL;
    }
    /* 从错误配置中独处总线线程超时错误认定值，<= 0 忽略此项 */
    busrw_timeout_as_fatal = err->busrw_timeout_as_fatal_;
    average_request_completion_delay_as_fatal = err->average_request_completion_delay_as_fatal_;
    average_request_checking_window_if_enabled = err->average_request_checking_window_if_enabled_;
    var__release_object_reference(err);

    /* 线程中引用对象， 用于保护生命周期 */
    var__functional_object_t *object = (var__functional_object_t *) objrefr(hld);
    var__canbus_t *canbus = var__object_body_ptr(var__canbus_t, object);
    if (!object) return NULL;

    argv.port_ = canbus->canbus_id_;
    argv.type_ = canbus->canbus_type_;

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, 
        "can bus IO thread for port %d success startup.", argv.port_);
    while (1) {
        objs = NULL;
        i = 0;

        begin_tick = posix__gettick();

        var__acquire_lock(object);

        if (!list_empty(&canbus->mnt_)) {
            objs = malloc(sizeof ( void *) * canbus->mnt_cnt_);

            list_for_each(loop, &canbus->mnt_) {
                rw = containing_record(loop, vari__drive_io_t, entry_);
                rw->dup_ = rw->create_dup_proc_(rw->handle_);
                if (rw->dup_) {
                    objs[i++] = rw->dup_;
                }
            }
        }

        var__release_lock(object);

        if (objs && i > 0) {
            canio__rw(&argv, (void **) objs, i);
            free(objs);
        }

        var__acquire_lock(object);

        list_for_each(loop, &canbus->mnt_) {
            rw = containing_record(loop, vari__drive_io_t, entry_);
            if (rw->dup_) {
                rw->release_dup_proc_(rw->dup_);
                rw->dup_ = NULL;
            }
        }
        var__release_lock(object);

        interval = run__interval_control(&waiter, begin_tick, DRIVE_IO_INTERVAL);

        /* 如果启用了总线线程的整体超时错误检查 */
        if (busrw_timeout_as_fatal > 0) {
            if (interval >= busrw_timeout_as_fatal) {
                log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem, 
                    "can bus IO thread executive timeout.interval=%d", interval);
                var__mark_framwork_error(object->object_id_, var__make_error_code(kVarType_CanBus, kFramworkFatal_WorkerThreadNonResponse));
            }
        }

        /* 如果启用了平均超时的错误检查 */
        if ((average_request_checking_window_if_enabled > 0) && (average_request_completion_delay_as_fatal > 0)) {
            ++average_checking_counter;
            sum_interval_in_window += interval;

            /* 已经到达检查点 */
            if (average_checking_counter >= average_request_checking_window_if_enabled) {
                if ((sum_interval_in_window / average_checking_counter) > average_request_completion_delay_as_fatal) {
                    log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem, 
                                "can bus IO thread average IO timeout.sum interval=%d", sum_interval_in_window);
                    var__mark_framwork_error(object->object_id_, var__make_error_code(kVarType_CanBus, kFramworkFatal_AverageIOTimeout));
                }
                average_checking_counter = 0;
                sum_interval_in_window = 0;
            }
        }
    }

    log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem, "can bus IO thread for port %d terminated.", argv.port_);
    objdefr(hld);

    posix__uninit_waitable_handle(&waiter);
    return NULL;
}

static
int load_canbus(int port, int type, int baud) {
    canio__busarg_t argv;
    var__error_handler_t *err;
    var__functional_object_t *object;
    int retval = -1;

    err = var__get_error_handler();
    if (!err) {
        return -1;
    }

    argv.port_ = port;
    argv.type_ = type;
    if (canio__create_bus(&argv, baud) >= 0) {
        object = var__object_head_ptr(err);
        canio__set_error_ptr(object);
        retval = 0;
    }
    var__release_object_reference(err);

    return retval;
}

#endif

static
int vari__canbus_loaded_handler(xmlNodePtr element) {
    var__canbus_t *canbus;
    var__functional_object_t *object;
    xmlNodePtr node;

    while (element) {
        if (0 == xmlStrcmp(BAD_CAST"bus", element->name)) {
            if (var__allocate_functional_object(sizeof ( var__canbus_t), kVarType_CanBus, &object) < 0) {
                break;
            }
            canbus = var__object_body_ptr(var__canbus_t, object);
            node = element->xmlChildrenNode;
            while (node) {
                if (0 == xmlStrcmp(BAD_CAST"id", node->name)) {
                    object->object_id_ = covert_first_text_value_l(node);
                } else if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
                    covert_first_text_value_s(node, object->name_, cchof(object->name_));
                } else if (0 == xmlStrcmp(BAD_CAST"canid", node->name)) {
                    canbus->canbus_id_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"type", node->name)) {
                    canbus->canbus_type_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"baud", node->name)) {
                    canbus->baud_rate_ = covert_first_text_value_i(node);
                } else {
                    ;
                }
                node = node->next;
            }

#if !_WIN32
            list_add_tail(&object->local_, &__local);
            var__insert_object(object);
#else
            /* 直接打开CAN口, 并得到设备对象, 如果失败， 则这条总线设备无法正常运作 */
            if (load_canbus(canbus->canbus_id_, canbus->canbus_type_, canbus->baud_rate_) < 0) {
                log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                        "failed to open canbus,parameter port=%d type=%d baud=%d", canbus->canbus_id_, canbus->canbus_type_, canbus->baud_rate_);
                var__delete_functional_object(object);
            } else {
                /* 插入本地管理列表 */
                list_add_tail(&object->local_, &__local);

                /* 插入全局的对象管理列表 */
                var__insert_object(object);

                /* 启动总线 IO 线程 */
                INIT_LIST_HEAD(&canbus->mnt_);
                posix__pthread_create(&canbus->thrw_, &bus_rw, (void *) object->handle_);
            }
#endif
        }
        element = element->next;
    }

    return 0;
}

/* 从 ./etc/dev/canbus.xml 读取所有的 canbus 配置对象，并加入到对象管理区 */
int var__load_canbus() {
    INIT_LIST_HEAD(&__local);
    return xml__read_data("dev", "canbus.xml", "can", &vari__canbus_loaded_handler);
}

int var__get_canio_arg(int busid, canio__busarg_t *argv) {
    struct list_head *loop;
    var__functional_object_t *object;
    var__canbus_t *bus;

    if (!argv) {
        return -1;
    }

    list_for_each(loop, &__local) {
        object = containing_record(loop, var__functional_object_t, local_);
        if (object->object_id_ == busid) {
            bus = var__object_body_ptr(var__canbus_t, object);
            var__acquire_lock(object);
            argv->port_ = bus->canbus_id_;
            argv->type_ = bus->canbus_type_;
            var__release_lock(object);
            break;
        }
    }

    return 0;
}

void var__attach_busrw(int bus_object_id, drive_create_dup_t create_dup_proc, drive_release_dup_t release_dup_proc, objhld_t handle) {
    struct list_head *loop;
    var__functional_object_t *object;
    var__canbus_t *bus;
    vari__drive_io_t *rw;

    list_for_each(loop, &__local) {
        object = containing_record(loop, var__functional_object_t, local_);
        if (object->object_id_ == bus_object_id) {
            bus = var__object_body_ptr(var__canbus_t, object);

            var__acquire_lock(object);

            rw = malloc(sizeof ( vari__drive_io_t));
            rw->dup_ = NULL;
            var__get_canio_arg(bus_object_id, &rw->argv_);
            rw->create_dup_proc_ = create_dup_proc;
            rw->release_dup_proc_ = release_dup_proc;
            rw->handle_ = handle;
            list_add_tail(&rw->entry_, &bus->mnt_);
            bus->mnt_cnt_++;

            var__release_lock(object);

            break;
        }
    }
}
