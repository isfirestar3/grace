#include "dio.h"
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
#include "posix_string.h"

#include "version.h"

static
struct list_head __local;

#if !ARM_LINUX

static
void *dio_create_dup(objhld_t hld) {
    var__dio_t *dio;
    dio = var__create_dio_dup(hld);
    return var__object_head_ptr(dio);
}

static
void dio_release_dup(void *object) {

    var__dio_t *dio;
    dio = var__object_body_ptr(var__dio_t, object);

    var__commit_dio_dup(dio);
    var__release_dio_dup(dio);
}

static
void *vari__dio_self_rw(void *parameter) {

    var__dio_t *dio, *dup;
    canio__busarg_t canbusarg;
    objhld_t hld;
    uint64_t begin_tick;
    var__functional_object_t *obj;
    posix__waitable_handle_t waiter;

    dio = (var__dio_t *) parameter;
    if (var__get_canio_arg(dio->candev_head_.canbus_, &canbusarg) < 0) {
        return NULL;
    }
    hld = var__object_head_ptr(dio)->handle_;

    if (posix__init_synchronous_waitable_handle(&waiter) < 0) {
        log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                "DIO self waitable handle init failed.");
        return NULL;
    }

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
            "DIO self IO thread for object %d success startup.", var__object_head_ptr(dio)->object_id_);

    do {
        begin_tick = posix__gettick();

        dup = var__create_dio_dup(hld);
        if (!dup) {
            break;
        }
        obj = var__object_head_ptr(dup);
        if (canio__rw(&canbusarg, (void **) &obj, 1) < 0) {
            log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "DIO drive rw proc failed. device thread will be terminated.");
            break;
        }
        var__commit_dio_dup(dup);
        var__release_dio_dup(dup);

        run__interval_control(&waiter, begin_tick, DRIVE_IO_INTERVAL);
    } while (1);

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem,
            "DIO self IO thread for object %d terminated.", var__object_head_ptr(dio)->object_id_);

    posix__uninit_waitable_handle(&waiter);

    return NULL;
}

static
int vari__init_dio_object(var__functional_object_t *object) {
    var__dio_t *dio;
    canio__busarg_t argv;

    dio = var__object_body_ptr(var__dio_t, object);

    if (var__get_canio_arg(dio->candev_head_.canbus_, &argv) < 0) {
        return -1;
    }
    return canio__create_node(&argv, object, 1);
}

#endif // _WIN32

static
int vari__ai_loaded_blocks(xmlNodePtr element, var__dio_t *device) {
    xmlNodePtr node;
    var__dio_canbus_usrdef_block_t *block;
    char target[16];
    int i;

    if (!device || !element) {
        return -1;
    }

    i = 0;

    while (element && (i < MAXIMUM_DIO_BLOCK_COUNT)) {
        block = &device->i.ai_[i];

        if (0 == xmlStrcmp(BAD_CAST"block", element->name)) {
            node = element->xmlChildrenNode;
            while (node) {
                if (0 == xmlStrcmp(BAD_CAST"start_address", node->name)) {
                    covert_first_text_value_s(node, target, cchof(target));
                    block->start_address_ = strtoul(target, NULL, 16);
                } else if (0 == xmlStrcmp(BAD_CAST"effective", node->name)) {
                    block->effective_count_of_index_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"internel_type", node->name)) {
                    block->internel_type_ = covert_first_text_value_i(node);
                }
                node = node->next;
            }
        }
        ++i;
        element = element->next;
    }
    return 0;
}

static
int vari__ao_loaded_blocks(xmlNodePtr element, var__dio_t *device) {
    xmlNodePtr node;
    var__dio_canbus_usrdef_block_t *block2, *block;
    char target[16];
    int i;

    if (!device || !element) {
        return -1;
    }

    i = 0;

    while (element && (i < MAXIMUM_DIO_BLOCK_COUNT)) {
        block = &device->ao_[i];
        block2 = &device->i.ao2_[i];

        if (0 == xmlStrcmp(BAD_CAST"block", element->name)) {
            node = element->xmlChildrenNode;
            while (node) {
                if (0 == xmlStrcmp(BAD_CAST"start_address", node->name)) {
                    covert_first_text_value_s(node, target, cchof(target));
                    block2->start_address_ = block->start_address_ = strtoul(target, NULL, 16);
                } else if (0 == xmlStrcmp(BAD_CAST"effective", node->name)) {
                    block2->effective_count_of_index_ = block->effective_count_of_index_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"internel_type", node->name)) {
                    block2->internel_type_ = block->internel_type_ = covert_first_text_value_i(node);
                }
                node = node->next;
            }
        }
        ++i;
        element = element->next;
    }
    return 0;
}

static
int vari__dio_loaded_handler(xmlNodePtr element) {
    var__dio_t *dio;
    var__functional_object_t *object;
    xmlNodePtr node;

    while (element) {
        if (0 == xmlStrcmp(BAD_CAST"device", element->name)) {
            if (var__allocate_functional_object(sizeof ( var__dio_t), kVarType_DIO, &object) < 0) {
                break;
            }
            dio = var__object_body_ptr(var__dio_t, object);
            var__init_status_describe(&dio->status_);

            node = element->xmlChildrenNode;
            while (node) {
                if (0 == xmlStrcmp(BAD_CAST"id", node->name)) {
                    object->object_id_ = covert_first_text_value_l(node);
                } else if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
                    covert_first_text_value_s(node, object->name_, cchof(object->name_));
                } else if (0 == xmlStrcmp(BAD_CAST"di_channel_num", node->name)) {
                    dio->di_channel_num_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"do_channel_num", node->name)) {
                    dio->do_channel_num_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"ais", node->name)) {
                    vari__ai_loaded_blocks(node->xmlChildrenNode, dio);
                } else if (0 == xmlStrcmp(BAD_CAST"aos", node->name)) {
                    vari__ao_loaded_blocks(node->xmlChildrenNode, dio);
                } else {
                    xml__check_candevice_head(node, &dio->candev_head_);
                }
                node = node->next;
            }

#if !_WIN32
            list_add_tail(&object->local_, &__local);
            var__insert_object(object);
#else
            if (vari__init_dio_object(object) >= 0) {
                if (dio->candev_head_.merge_) {
                    var__attach_busrw(dio->candev_head_.canbus_, &dio_create_dup, &dio_release_dup, object->handle_);
                } else {
                    dio->candev_head_.self_rw_ = (void *) (posix__pthread_t *) malloc(sizeof ( posix__pthread_t));
                    posix__pthread_create((posix__pthread_t *) & dio->candev_head_.self_rw_, &vari__dio_self_rw, dio);
                }
                list_add_tail(&object->local_, &__local);
                var__insert_object(object);
            } else {
                log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                        "failed to init DIO device node for object:%d", object->object_id_);
                //var__delete_functional_object(object);
                list_add_tail(&object->local_, &__local);
                var__insert_object(object);
            }
#endif
        }
        element = element->next;
    }

    return 0;
}

int var__load_dio() {

    INIT_LIST_HEAD(&__local);

    // 无条件增加 InternalDIO 到对象列表
    var__functional_object_t *object;
    var__dio_t *dio;
    if (var__allocate_functional_object(sizeof ( var__dio_t), kVarType_DIO, &object) < 0) {
        return -1;
    }

    object->object_id_ = kVarFixedObject_InternalDIO;
    posix__strcpy(object->name_, sizeof (object->name_), "InternalDIO");

    dio = var__object_body_ptr(var__dio_t, object);
    var__init_status_describe(&dio->status_);

    list_add_tail(&object->local_, &__local);

    var__insert_object(object);

    // 从配置文件中读出其他的DIO对象信息
    return xml__read_data("dev", "dio.xml", "dio", &vari__dio_loaded_handler);
}

var__dio_t *var__get_dio(objhld_t hld) {
    var__functional_object_t *obj;
    var__dio_t *dio;

    if (hld > 0) {
        obj = objrefr(hld);
        if (obj) {
            if (obj->type_ != kVarType_DIO) {
                objdefr(hld);
                return NULL;
            }
            dio = var__object_body_ptr(var__dio_t, obj);
            var__acquire_lock(obj);
            return dio;
        }
    }

    return NULL;
}

var__dio_t *var__get_dio_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld < 0) {
        return NULL;
    }
    return var__get_dio(hld);
}

var__dio_t *var__create_dio_dup(objhld_t hld) {
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
    dobj->dup_ = oobj->handle_;

    var__release_lock(oobj);

    posix__pthread_mutex_init(&dobj->locker_);

    INIT_LIST_HEAD(&dobj->pagefault_);
    INIT_LIST_HEAD(&dobj->global_);

    objdefr(hld);
    return var__object_body_ptr(var__dio_t, dobj);
}

var__dio_t *var__create_dio_dup_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld >= 0) {
        return var__create_dio_dup(hld);
    }
    return NULL;
}

void var__release_dio_dup(var__dio_t *dio) {
    var__functional_object_t *dobj;

    dobj = var__object_head_ptr(dio);
    if (dobj) {
        posix__pthread_mutex_release(&dobj->locker_);
        free(dobj);
    }
}

int var__commit_dio_dup(const var__dio_t *dio) {
    var__functional_object_t *oobj, *dobj;
    var__dio_t *odio;
    char *cpydst;
    const char *cpysrc;

    if (!dio) return -1;

    dobj = var__object_head_ptr(dio);
    if (!dobj) return -1;

    oobj = objrefr(dobj->dup_);
    if (!oobj) return -1;

    odio = var__object_body_ptr(var__dio_t, oobj);
    var__acquire_lock(oobj);

    cpydst = oobj->body_;
    cpydst += offsetof(var__dio_t, u_);
    cpysrc = (const char *) dio;
    cpysrc += offsetof(var__dio_t, u_);
    memcpy(cpydst, cpysrc, sizeof ( struct _i_dio_internal_t));

    var__commit_status_describe_dup(&odio->status_, &dio->status_);

    var__release_lock(oobj);

    objdefr(dobj->dup_);
    return 0;
}
