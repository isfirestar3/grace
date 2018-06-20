#include "angle_encoder.h"
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
struct list_head __local = {NULL};

#if _WIN32

static
void *ae_create_dup(objhld_t hld) {
    var__angle_encoder_t *ae;
    ae = var__create_ae_dup(hld);
    return var__object_head_ptr(ae);
}

static
void ae_release_dup(void *object) {

    var__angle_encoder_t *ae;
    ae = var__object_body_ptr(var__angle_encoder_t, object);

    var__commit_ae_dup(ae);
    var__release_ae_dup(ae);
}

static
void *vari__ae_self_rw(void *parameter) {

    var__angle_encoder_t *ae, *dup;
    canio__busarg_t canbusarg;
    objhld_t hld;
    uint64_t begin_tick;
    posix__waitable_handle_t waiter;

    ae = (var__angle_encoder_t *) parameter;
    if (var__get_canio_arg(ae->candev_head_.canbus_, &canbusarg) < 0) {
        return NULL;
    }
    hld = var__object_head_ptr(ae)->handle_;

    if (posix__init_synchronous_waitable_handle(&waiter) < 0) {
        log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                "angle encoder self waitable handle init failed.");
        return NULL;
    }

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
            "angle encoder self IO thread for object %d success startup.", var__object_head_ptr(ae)->object_id_);

    do {
        begin_tick = posix__gettick();

        dup = var__create_ae_dup(hld);
        if (canio__rw(&canbusarg, (void **) &dup, 1) < 0) {
            log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "angle encoder drive rw proc failed. device thread will be terminated.");
            break;
        }
        var__release_ae_dup(dup);

        run__interval_control(&waiter, begin_tick, DRIVE_IO_INTERVAL);
    } while (1);

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem,
            "angle encoder self IO thread for object %d terminated.", var__object_head_ptr(ae)->object_id_);

    posix__uninit_waitable_handle(&waiter);
    return NULL;
}

static
int vari__init_ae_object(var__functional_object_t *object) {
    var__angle_encoder_t *ae;
    canio__busarg_t argv;

    ae = var__object_body_ptr(var__angle_encoder_t, object);

    // 从相关的 bus 对象获取总线参数
    if (var__get_canio_arg(ae->candev_head_.canbus_, &argv) < 0) {
        return -1;
    }
    canio__create_node(&argv, object, 1);

    return 0;
}

#endif // _WIN32

static
int vari__ae_loaded_handler(xmlNodePtr element) {
    var__angle_encoder_t *ae;
    var__functional_object_t *object;
    xmlNodePtr node;

    while (element) {
        if (0 == xmlStrcmp(BAD_CAST"device", element->name)) {
            if (var__allocate_functional_object(sizeof ( var__angle_encoder_t), kVarType_AngleEncoder, &object) < 0) {
                break;
            }
            ae = var__object_body_ptr(var__angle_encoder_t, object);

            node = element->xmlChildrenNode;
            while (node) {
                if (0 == xmlStrcmp(BAD_CAST"id", node->name)) {
                    object->object_id_ = covert_first_text_value_l(node);
                } else if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
                    covert_first_text_value_s(node, object->name_, cchof(object->name_));
                } else if (0 == xmlStrcmp(BAD_CAST"encoder_type", node->name)) {
                    ae->encoder_type_ = (enum angle_encoder_type_t)covert_first_text_value_i(node);
                } else {
                    xml__check_candevice_head(node, &ae->candev_head_);
                }

                node = node->next;
            }

#if !_WIN32
			// 插入本地管理列表
			list_add_tail(&object->local_, &__local);

			// 插入全局的对象管理列表
			var__insert_object(object);
#else
            if (vari__init_ae_object(object) >= 0) {

                if (ae->candev_head_.merge_) {
                    var__attach_busrw(ae->candev_head_.canbus_, &ae_create_dup, &ae_release_dup, object->handle_);
                } else {
                    ae->candev_head_.self_rw_ = (posix__pthread_t *)malloc(sizeof ( posix__pthread_t));
                    posix__pthread_create((posix__pthread_t *) & ae->candev_head_.self_rw_, &vari__ae_self_rw, ae);
                }

                // 插入本地管理列表
                list_add_tail(&object->local_, &__local);

                // 插入全局的对象管理列表
                var__insert_object(object);
            } else {
                log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                        "failed to init angle encoder device node for object:%d", object->object_id_);
                var__delete_functional_object(object);
            }
#endif
        }
        element = element->next;
    }

    return 0;
}

// 从 ./etc/dev/angle_encoder.xml 读取所有的 ELMO 配置对象，并加入到对象管理区

int var__load_angle_encoder() {

    INIT_LIST_HEAD(&__local);

    if (xml__read_data("dev", "angle_encoder.xml", "ae", &vari__ae_loaded_handler)) {
        return -1;
    }

    return 0;
}

var__angle_encoder_t *var__get_ae(objhld_t hld) {
    var__functional_object_t *obj;
    var__angle_encoder_t *ae;

    if (hld > 0) {
        obj = objrefr(hld);
        if (obj) {
            if (obj->type_ != kVarType_AngleEncoder) {
                objdefr(hld);
                return NULL;
            }
            ae = var__object_body_ptr(var__angle_encoder_t, obj);
            var__acquire_lock(obj);
            return ae;
        }
    }

    return NULL;
}

var__angle_encoder_t *var__get_ae_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld < 0) {
        return NULL;
    }
    return var__get_ae(hld);
}

var__angle_encoder_t *var__create_ae_dup(objhld_t hld) {
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
    return var__object_body_ptr(var__angle_encoder_t, dobj);
}

var__angle_encoder_t *var__create_ae_dup_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld >= 0) {
        return var__create_ae_dup(hld);
    }
    return NULL;
}

int var__commit_ae_dup(const var__angle_encoder_t *ae) {

    var__functional_object_t *oobj, *dobj;
    char *cpydst;
    const char *cpysrc;

    if (!ae) return -1;

    dobj = var__object_head_ptr(ae);
    if (!dobj) return -1;

    oobj = objrefr(dobj->dup_);
    if (!oobj) return -1;

    var__acquire_lock(oobj);

    cpydst = oobj->body_;
    cpydst += offsetof(var__angle_encoder_t, u_);
    cpysrc = (const char *) ae;
    cpysrc += offsetof(var__angle_encoder_t, u_);
    memcpy(cpydst, cpysrc, sizeof ( struct _i_angle_encoder_internal_t));

    var__release_lock(oobj);

    objdefr(dobj->dup_);
    return 0;
}

void var__release_ae_dup(var__angle_encoder_t *ae) {
    var__functional_object_t *dobj;

    dobj = var__object_head_ptr(ae);
    if (dobj) {
        posix__pthread_mutex_release(&dobj->locker_);
        free(dobj);
    }
}
