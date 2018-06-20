#include "elmo.h"
#include "object.h"
#include "var.h"
#include "xml_loaded.h"
#include "bus.h"
#include "mnt.h"
#include "canio.h"
#include "logger.h"
#include "entry.h"
#include "error.h"

#include "posix_wait.h"
#include "posix_time.h"

#include "version.h"

static
struct list_head __local;

#if _WIN32

static
void *elmo_create_dup(objhld_t hld) {
    var__elmo_t *elmo;
    elmo = var__create_elmo_dup(hld);
    return var__object_head_ptr(elmo);
}

static
void elmo_release_dup(void *object) {

    var__elmo_t *elmo;
    elmo = var__object_body_ptr(var__elmo_t, object);
    var__commit_elmo_dup(elmo);
    var__release_elmo_dup(elmo);
}

static
void *vari__elmo_self_rw(void *parameter) {

    var__elmo_t *elmo, *dup;
    canio__busarg_t canbusarg;
    objhld_t hld;
    uint64_t begin_tick;
    var__functional_object_t *obj;
    posix__waitable_handle_t waiter;

    elmo = (var__elmo_t *) parameter;
    if (var__get_canio_arg(elmo->candev_head_.canbus_, &canbusarg) < 0) {
        return NULL;
    }
    hld = var__object_head_ptr(elmo)->handle_;

    if (posix__init_synchronous_waitable_handle(&waiter) < 0) {
        log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                "elmo self waitable handle init failed..");
        return NULL;
    }

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
            "elmo self IO thread for object %d success startup.", var__object_head_ptr(elmo)->object_id_);

    do {
        begin_tick = posix__gettick();

        dup = var__create_elmo_dup(hld);
        if (!dup) {
            break;
        }
        obj = var__object_head_ptr(dup);
        if (canio__rw(&canbusarg, (void **) &obj, 1) < 0) {
            log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "elmo drive rw proc failed. device thread will be terminated.");
            break;
        }
        var__commit_elmo_dup(dup);
        var__release_elmo_dup(dup);

        run__interval_control(&waiter, begin_tick, DRIVE_IO_INTERVAL);
    } while (1);

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem,
            "elmo self IO thread for object %d terminated.", var__object_head_ptr(elmo)->object_id_);
    posix__uninit_waitable_handle(&waiter);
    return NULL;
}

static
int vari__init_elmo_object(var__functional_object_t *object) {
    var__elmo_t *elmo;
    canio__busarg_t argv;

    elmo = var__object_body_ptr(var__elmo_t, object);

    // 从相关的 bus 对象获取总线参数
    if (var__get_canio_arg(elmo->candev_head_.canbus_, &argv) < 0) {
        return -1;
    }

    /* 创建节点失败 */
    if (canio__create_node(&argv, object, 1) < 0) {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem, "canbus node for elmo create failed,type=%d port=%d", argv.type_, argv.port_);
        var__mark_framwork_error(object->object_id_, var__make_error_code(kVarType_Elmo, kFramworkFatal_CanbusNodeNonResponse));
        return -1;
    }

    return 0;
}

#endif // _WIN32

static
int vari__elmo_loaded_handler(xmlNodePtr element) {
    var__elmo_t *elmo;
    var__functional_object_t *object;
    xmlNodePtr node;

    while (element) {
        if (0 == xmlStrcmp(BAD_CAST"device", element->name)) {
            if (var__allocate_functional_object(sizeof ( var__elmo_t), kVarType_Elmo, &object) < 0) {
                break;
            }
            elmo = var__object_body_ptr(var__elmo_t, object);
            var__init_status_describe(&elmo->status_);

            node = element->xmlChildrenNode;
            while (node) {
                if (0 == xmlStrcmp(BAD_CAST"id", node->name)) {
                    object->object_id_ = covert_first_text_value_l(node);
                } else if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
                    covert_first_text_value_s(node, object->name_, cchof(object->name_));
                } else if (0 == xmlStrcmp(BAD_CAST"profile_speed", node->name)) {
                    elmo->profile_speed_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"profile_acc", node->name)) {
                    elmo->profile_acc_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"profile_dec", node->name)) {
                    elmo->profile_dec_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"mode", node->name)) {
                    elmo->control_mode_ = covert_first_text_value_i(node);
                } else {
                    xml__check_candevice_head(node, &elmo->candev_head_);
                }

                node = node->next;
            }

            elmo->enable_ = 1;

            if (kDriverControlMode_Unknown == elmo->control_mode_) {
                elmo->control_mode_ = kDriverControlMode_SpeedMode;
            }

#if !_WIN32
            // 插入本地管理列表
            list_add_tail(&object->local_, &__local);

            // 插入全局的对象管理列表
            var__insert_object(object);
#else
            if (vari__init_elmo_object(object) >= 0) {

                if (elmo->candev_head_.merge_) {
                    var__attach_busrw(elmo->candev_head_.canbus_, &elmo_create_dup, &elmo_release_dup, object->handle_);
                } else {
                    elmo->candev_head_.self_rw_ = (void *) (posix__pthread_t *) malloc(sizeof ( posix__pthread_t));
                    posix__pthread_create((posix__pthread_t *) & elmo->candev_head_.self_rw_, &vari__elmo_self_rw, elmo);
                }

                // 插入本地管理列表
                list_add_tail(&object->local_, &__local);

                // 插入全局的对象管理列表
                var__insert_object(object);
            } else {
                log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                        "failed to init elmo device node for object:%d", object->object_id_);
                var__delete_functional_object(object);
            }
#endif
        }

        element = element->next;
    }

    return 0;
}

// 从 ./etc/dev/elmo.xml 读取所有的 ELMO 配置对象，并加入到对象管理区

int var__load_elmo() {

    INIT_LIST_HEAD(&__local);

    if (xml__read_data("dev", "elmo.xml", "elmo", vari__elmo_loaded_handler)) {
        return -1;
    }

    return 0;
}

var__elmo_t *var__get_elmo(objhld_t hld) {
    var__functional_object_t *obj;
    var__elmo_t *elmo;

    if (hld > 0) {
        obj = objrefr(hld);
        if (obj) {
            if (obj->type_ != kVarType_Elmo) {
                objdefr(hld);
                return NULL;
            }
            elmo = var__object_body_ptr(var__elmo_t, obj);
            var__acquire_lock(obj);
            return elmo;
        }
    }

    return NULL;
}

var__elmo_t *var__get_elmo_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld < 0) {
        return NULL;
    }
    return var__get_elmo(hld);
}

var__elmo_t *var__create_elmo_dup(objhld_t hld) {
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
    return var__object_body_ptr(var__elmo_t, dobj);
}

var__elmo_t *var__create_elmo_dup_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld >= 0) {
        return var__create_elmo_dup(hld);
    }
    return NULL;
}

void var__release_elmo_dup(var__elmo_t *elmo) {
    var__functional_object_t *dobj;

    dobj = var__object_head_ptr(elmo);
    if (dobj) {
        posix__pthread_mutex_release(&dobj->locker_);
        free(dobj);
    }
}

int var__commit_elmo_dup(const var__elmo_t *elmo) {
    var__functional_object_t *oobj, *dobj;
    var__elmo_t *oelmo;
    char *cpydst;
    const char *cpysrc;

    if (!elmo) return -1;

    dobj = var__object_head_ptr(elmo);
    if (!dobj) return -1;

    oobj = objrefr(dobj->dup_);
    if (!oobj) return -1;

    oelmo = var__object_body_ptr(var__elmo_t, oobj);
    var__acquire_lock(oobj);

    cpydst = oobj->body_;
    cpydst += offsetof(var__elmo_t, u_);
    cpysrc = (const char *) elmo;
    cpysrc += offsetof(var__elmo_t, u_);
    memcpy(cpydst, cpysrc, sizeof ( struct _i_elmo_internal_t));

    var__commit_status_describe_dup(&oelmo->status_, &elmo->status_);

    var__release_lock(oobj);

    objdefr(dobj->dup_);
    return 0;
}
