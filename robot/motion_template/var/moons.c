#include "moons.h"
#include "object.h"
#include "var.h"
#include "xml_loaded.h"
#include "bus.h"
#include "mnt.h"
#include "canio.h"
#include "logger.h"
#include "entry.h"

#include "posix_wait.h"
#include "posix_time.h"

#include "version.h"

static
struct list_head __local;

#if _WIN32

static
void *moos_create_dup(objhld_t hld) {
    var__moos_t *moos;
    moos = var__create_moos_dup(hld);
    return var__object_head_ptr(moos);
}

static
void moos_release_dup(void *object) {

    var__moos_t *moos;
    moos = var__object_body_ptr(var__moos_t, object);

    var__commit_moos_dup(moos);
    var__release_moos_dup(moos);
}

static
void *vari__moos_self_rw(void *parameter) {

    var__moos_t *moos, *dup;
    canio__busarg_t canbusarg;
    objhld_t hld;
    uint64_t begin_tick;
    var__functional_object_t *obj;
    posix__waitable_handle_t waiter;

    moos = (var__moos_t *) parameter;
    if (var__get_canio_arg(moos->candev_head_.canbus_, &canbusarg) < 0) {
        return NULL;
    }
    hld = var__object_head_ptr(moos)->handle_;

    if (posix__init_synchronous_waitable_handle(&waiter) < 0) {
        log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                "moos waitable handle init failed.");
        return NULL;
    }

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
            "moos self IO thread for object %d success startup.", var__object_head_ptr(moos)->object_id_);

    do {
        begin_tick = posix__gettick();

        dup = var__create_moos_dup(hld);
        if (!dup) {
            break;
        }
        obj = var__object_head_ptr(dup);
        if (canio__rw(&canbusarg, (void **) &obj, 1) < 0) {
            log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "moos drive rw proc failed. device thread will be terminated.");
            break;
        }
        var__commit_moos_dup(dup);
        var__release_moos_dup(dup);

        run__interval_control(&waiter, begin_tick, DRIVE_IO_INTERVAL);
    } while (1);

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem,
            "moos self IO thread for object %d terminated.", var__object_head_ptr(moos)->object_id_);
    posix__uninit_waitable_handle(&waiter);
    return NULL;
}

static
int vari__init_moos_object(var__functional_object_t *object) {
    var__moos_t *moos;
    canio__busarg_t argv;

    moos = var__object_body_ptr(var__moos_t, object);

    // 从相关的 bus 对象获取总线参数
    if (var__get_canio_arg(moos->candev_head_.canbus_, &argv) < 0) {
        return -1;
    }
    return canio__create_node(&argv, object, 1);
}

#endif

static
int vari__moos_loaded_handler(xmlNodePtr element) {
    var__moos_t *moos;
    var__functional_object_t *object;
    xmlNodePtr node;

    while (element) {
        if (0 == xmlStrcmp(BAD_CAST"device", element->name)) {
            if (var__allocate_functional_object(sizeof ( var__moos_t), kVarType_Moons, &object) < 0) {
                break;
            }
            moos = var__object_body_ptr(var__moos_t, object);
            var__init_status_describe(&moos->status_);

            node = element->xmlChildrenNode;
            while (node) {
                if (0 == xmlStrcmp(BAD_CAST"id", node->name)) {
                    object->object_id_ = covert_first_text_value_l(node);
                } else if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
                    covert_first_text_value_s(node, object->name_, cchof(object->name_));
                } else if (0 == xmlStrcmp(BAD_CAST"profile_speed", node->name)) {
                    moos->profile_speed_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"profile_acc", node->name)) {
                    moos->profile_acc_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"profile_dec", node->name)) {
                    moos->profile_dec_ = covert_first_text_value_i(node);
                } else {
                    xml__check_candevice_head(node, &moos->candev_head_);
                }

                node = node->next;
            }

            moos->enable_ = 1;

#if !_WIN32
            // 插入本地管理列表
            list_add_tail(&object->local_, &__local);

            // 插入全局的对象管理列表
            var__insert_object(object);
#else

            if (vari__init_moos_object(object) >= 0) {

                if (moos->candev_head_.merge_) {
                    var__attach_busrw(moos->candev_head_.canbus_, &moos_create_dup, &moos_release_dup, object->handle_);
                } else {
                    moos->candev_head_.self_rw_ = (void *) malloc(sizeof ( posix__pthread_t));
                    posix__pthread_create((posix__pthread_t *) & moos->candev_head_.self_rw_, &vari__moos_self_rw, moos);
                }

                // 插入本地管理列表
                list_add_tail(&object->local_, &__local);

                // 插入全局的对象管理列表
                var__insert_object(object);
            } else {
                log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                        "failed to init moos device node for object:%d", object->object_id_);
                var__delete_functional_object(object);
            }
#endif
        }
        element = element->next;
    }

    return 0;
}

// 从 ./etc/dev/moos.xml 读取所有的 moos 配置对象，并加入到对象管理区

int var__load_moos() {

    INIT_LIST_HEAD(&__local);

    if (xml__read_data("dev", "moos.xml", "moos", vari__moos_loaded_handler)) {
        return -1;
    }

    return 0;
}

var__moos_t *var__get_moos(objhld_t hld) {
    var__functional_object_t *obj;
    var__moos_t *moos;

    if (hld > 0) {
        obj = objrefr(hld);
        if (obj) {
            if (obj->type_ != kVarType_Moons) {
                objdefr(hld);
                return NULL;
            }
            moos = var__object_body_ptr(var__moos_t, obj);
            var__acquire_lock(obj);
            return moos;
        }
    }

    return NULL;
}

var__moos_t *var__get_moos_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld < 0) {
        return NULL;
    }
    return var__get_moos(hld);
}

var__moos_t *var__create_moos_dup(objhld_t hld) {
    var__functional_object_t *oobj, *dobj;

    if (hld < 0) return NULL;

    oobj = objrefr(hld);
    if (!oobj) return NULL;

    dobj = malloc(oobj->length_);
    if (!dobj) {
        objdefr(hld);
        return NULL;
    }

    var__acquire_lock(oobj);
    memcpy(dobj, oobj, oobj->length_);

    // 记录复制来源
    dobj->dup_ = oobj->handle_;

    var__release_lock(oobj);

    // 初始化赋值的对象锁
    posix__pthread_mutex_init(&dobj->locker_);

    // 原始对象的链表不需要拷贝
    INIT_LIST_HEAD(&dobj->pagefault_);
    INIT_LIST_HEAD(&dobj->global_);

    objdefr(hld);
    return var__object_body_ptr(var__moos_t, dobj);
}

var__moos_t *var__create_moos_dup_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld >= 0) {
        return var__create_moos_dup(hld);
    }
    return NULL;
}

void var__release_moos_dup(var__moos_t *moos) {
    var__functional_object_t *dobj;

    dobj = var__object_head_ptr(moos);
    if (dobj) {
        posix__pthread_mutex_release(&dobj->locker_);
        free(dobj);
    }
}

int var__commit_moos_dup(const var__moos_t *moos) {
    var__functional_object_t *oobj, *dobj;
    var__moos_t *ostepmotor;
    char *cpydst;
    const char *cpysrc;

    if (!moos) return -1;

    dobj = var__object_head_ptr(moos);
    if (!dobj) return -1;

    oobj = objrefr(dobj->dup_);
    if (!oobj) return -1;

    ostepmotor = var__object_body_ptr(var__moos_t, oobj);
    var__acquire_lock(oobj);

    cpydst = oobj->body_;
    cpydst += offsetof(var__moos_t, u_);
    cpysrc = (const char *) moos;
    cpysrc += offsetof(var__moos_t, u_);
    memcpy(cpydst, cpysrc, sizeof ( struct _i_moos_internal_t));

    var__commit_status_describe_dup(&ostepmotor->status_, &moos->status_);

    var__release_lock(oobj);

    objdefr(dobj->dup_);
    return 0;
}
