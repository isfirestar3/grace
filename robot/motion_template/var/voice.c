#include "voice.h"
#include "object.h"
#include "var.h"
#include "xml_loaded.h"
#include "bus.h"
#include "mnt.h"
#include "canio.h"
#include "logger.h"
#include "entry.h"

#include "posix_time.h"
#include "posix_wait.h"

#include "version.h"

static
struct list_head __local;

#if _WIN32

static
void *voice_device_create_dup(objhld_t hld) {
    var__voice_device_t *voice_device;
    voice_device = var__create_voice_device_dup(hld);
    return var__object_head_ptr(voice_device);
}

static
void voice_device_release_dup(void *object) {

    var__voice_device_t *voice_device;
    voice_device = var__object_body_ptr(var__voice_device_t, object);
    var__commit_voice_device_dup(voice_device);
    var__release_voice_device_dup(voice_device);
}

static
void *vari__voice_device_self_rw(void *parameter) {

    var__voice_device_t *voice_device, *dup;
    canio__busarg_t canbusarg;
    objhld_t hld;
    uint64_t begin_tick;
    var__functional_object_t *obj;
    posix__waitable_handle_t waiter;

    voice_device = (var__voice_device_t *) parameter;
    if (var__get_canio_arg(voice_device->candev_head_.canbus_, &canbusarg) < 0) {
        return NULL;
    }
    hld = var__object_head_ptr(voice_device)->handle_;

    if (posix__init_synchronous_waitable_handle(&waiter) < 0) {
        log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                "voice_device self waitable handle init failed..");
        return NULL;
    }

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
            "voice_device self IO thread for object %d success startup.", var__object_head_ptr(voice_device)->object_id_);

    do {
        begin_tick = posix__gettick();

        dup = var__create_voice_device_dup(hld);
        if (!dup) {
            break;
        }
        obj = var__object_head_ptr(dup);
        if (canio__rw(&canbusarg, (void **) &obj, 1) < 0) {
            log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "voice_device drive rw proc failed. device thread will be terminated.");
            break;
        }
        var__commit_voice_device_dup(dup);
        var__release_voice_device_dup(dup);

        run__interval_control(&waiter, begin_tick, DRIVE_IO_INTERVAL);
    } while (1);

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem,
            "voice_device self IO thread for object %d terminated.", var__object_head_ptr(voice_device)->object_id_);
    posix__uninit_waitable_handle(&waiter);
    return NULL;
}

static
int vari__init_voice_device_object(var__functional_object_t *object) {
    var__voice_device_t *voice_device;
    canio__busarg_t argv;

    voice_device = var__object_body_ptr(var__voice_device_t, object);

    // 从相关的 bus 对象获取总线参数
    if (var__get_canio_arg(voice_device->candev_head_.canbus_, &argv) < 0) {
        return -1;
    }
    return canio__create_node(&argv, object, 1);
}

#endif // _WIN32

static
int vari__voice_device_loaded_handler(xmlNodePtr element) {
    var__voice_device_t *voice_device;
    var__functional_object_t *object;
    xmlNodePtr node;

    while (element) {
        if (0 == xmlStrcmp(BAD_CAST"device", element->name)) {
            if (var__allocate_functional_object(sizeof ( var__voice_device_t), kVarType_VoiceDevice, &object) < 0) {
                break;
            }
            voice_device = var__object_body_ptr(var__voice_device_t, object);

            node = element->xmlChildrenNode;
            while (node) {
                if (0 == xmlStrcmp(BAD_CAST"id", node->name)) {
                    object->object_id_ = covert_first_text_value_l(node);
                } else if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
                    covert_first_text_value_s(node, object->name_, cchof(object->name_));
                } else if (0 == xmlStrcmp(BAD_CAST"send_id", node->name)) {
                    voice_device->send_id_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"recv_id", node->name)) {
                    voice_device->recv_id_ = covert_first_text_value_i(node);
                } else {
                    xml__check_candevice_head(node, &voice_device->candev_head_);
                }

                node = node->next;
            }

            voice_device->enable_ = 1;

#if !_WIN32
            // 插入本地管理列表
            list_add_tail(&object->local_, &__local);

            // 插入全局的对象管理列表
            var__insert_object(object);
#else
            if (vari__init_voice_device_object(object) >= 0) {
                if (voice_device->candev_head_.merge_) {
                    var__attach_busrw(voice_device->candev_head_.canbus_, &voice_device_create_dup, &voice_device_release_dup, object->handle_);
                } else {
                    voice_device->candev_head_.self_rw_ = (void *) (posix__pthread_t *) malloc(sizeof ( posix__pthread_t));
                    posix__pthread_create((posix__pthread_t *) & voice_device->candev_head_.self_rw_, &vari__voice_device_self_rw, voice_device);
                }

                // 插入本地管理列表
                list_add_tail(&object->local_, &__local);

                // 插入全局的对象管理列表
                var__insert_object(object);
            } else {
                log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                        "failed to init voice_device device node for object:%d", object->object_id_);
                var__delete_functional_object(object);
            }
#endif
        }
        element = element->next;
    }

    return 0;
}

// 从 ./etc/dev/voice_device.xml 读取所有的 ELMO 配置对象，并加入到对象管理区

int var__load_voice_device() {

    INIT_LIST_HEAD(&__local);

    if (xml__read_data("dev", "voice.xml", "voice", vari__voice_device_loaded_handler)) {
        return -1;
    }

    return 0;
}

var__voice_device_t *var__get_voice_device(objhld_t hld) {
    var__functional_object_t *obj;
    var__voice_device_t *voice_device;

    if (hld > 0) {
        obj = objrefr(hld);
        if (obj) {
            if (obj->type_ != kVarType_Elmo) {
                objdefr(hld);
                return NULL;
            }
            voice_device = var__object_body_ptr(var__voice_device_t, obj);
            var__acquire_lock(obj);
            return voice_device;
        }
    }

    return NULL;
}

var__voice_device_t *var__get_voice_device_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld < 0) {
        return NULL;
    }
    return var__get_voice_device(hld);
}

var__voice_device_t *var__create_voice_device_dup(objhld_t hld) {
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
    return var__object_body_ptr(var__voice_device_t, dobj);
}

var__voice_device_t *var__create_voice_device_dup_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld >= 0) {
        return var__create_voice_device_dup(hld);
    }
    return NULL;
}

void var__release_voice_device_dup(var__voice_device_t *voice_device) {
    var__functional_object_t *dobj;

    dobj = var__object_head_ptr(voice_device);
    if (dobj) {
        posix__pthread_mutex_release(&dobj->locker_);
        free(dobj);
    }
}

int var__commit_voice_device_dup(const var__voice_device_t *voice_device) {
    var__functional_object_t *oobj, *dobj;
    char *cpydst;
    const char *cpysrc;

    if (!voice_device) return -1;

    dobj = var__object_head_ptr(voice_device);
    if (!dobj) return -1;

    oobj = objrefr(dobj->dup_);
    if (!oobj) return -1;

    var__acquire_lock(oobj);

    cpydst = oobj->body_;
    cpydst += offsetof(var__voice_device_t, u_);
    cpysrc = (const char *) voice_device;
    cpysrc += offsetof(var__voice_device_t, u_);
    memcpy(cpydst, cpysrc, sizeof ( struct _i_voice_device_internal_t));

    var__release_lock(oobj);

    objdefr(dobj->dup_);
    return 0;
}
