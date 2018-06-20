#include "curtis.h"
#include "object.h"
#include "var.h"
#include "xml_loaded.h"
#include "bus.h"
#include "mnt.h"
#include "canio.h"
#include "logger.h"
#include "entry.h"
#include "error.h"

#include "posix_time.h"

#include "version.h"

static
struct list_head __local;

#if _WIN32

static
void *curtis_create_dup(objhld_t hld) {
    var__curtis_t *curtis;
    curtis = var__create_curtis_dup(hld);
    return var__object_head_ptr(curtis);
}

static
void curtis_release_dup(void *object) {

    var__curtis_t *curtis;
    curtis = var__object_body_ptr(var__curtis_t, object);
    var__commit_curtis_dup(curtis);
    var__release_curtis_dup(curtis);
}

static
void *vari__curtis_self_rw(void *parameter) {

    var__curtis_t *curtis, *dup;
    canio__busarg_t canbusarg;
    objhld_t hld;
    uint64_t begin_tick;
    var__functional_object_t *obj;
    posix__waitable_handle_t waiter;

    curtis = (var__curtis_t *) parameter;
    if (var__get_canio_arg(curtis->candev_head_.canbus_, &canbusarg) < 0) {
        return NULL;
    }
    hld = var__object_head_ptr(curtis)->handle_;
    
    if (posix__init_synchronous_waitable_handle(&waiter) < 0 ){
        log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
            "curtis self waitable handle init failed..");
        return NULL;
    }

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
            "curtis self IO thread for object %d success startup.", var__object_head_ptr(curtis)->object_id_);

    do {
        begin_tick = posix__gettick();

        dup = var__create_curtis_dup(hld);
        if (!dup) {
            break;
        }
        obj = var__object_head_ptr(dup);
        if (canio__rw(&canbusarg, (void **) &obj, 1) < 0) {
            log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "curtis drive rw proc failed. device thread will be terminated.");
            break;
        }
        var__commit_curtis_dup(dup);
        var__release_curtis_dup(dup);

        run__interval_control(&waiter, begin_tick, DRIVE_IO_INTERVAL);
    } while (1);

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem,
            "curtis self IO thread for object %d terminated.", var__object_head_ptr(curtis)->object_id_);
    posix__uninit_waitable_handle(&waiter);
    return NULL;
}

static
int vari__init_curtis_object(var__functional_object_t *object) {
    var__curtis_t *curtis;
    canio__busarg_t argv;

    curtis = var__object_body_ptr(var__curtis_t, object);

    // 从相关的 bus 对象获取总线参数
    if (var__get_canio_arg(curtis->candev_head_.canbus_, &argv) < 0) {
        return -1;
    }
    
    /* 创建节点失败 */
    if (canio__create_node(&argv, object, 1) < 0){
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem, 
            "canbus node for curtis create failed,type=%d port=%d", argv.type_, argv.port_);
        var__mark_framwork_error(object->object_id_, var__make_error_code(kVarType_Curtis, kFramworkFatal_WorkerThreadNonResponse));
        return -1;
    }
    
    return 0;
}

#endif // !ARM_LINUX

static
int vari__curtis_loaded_handler(xmlNodePtr element) {
    var__curtis_t *curtis;
    var__functional_object_t *object;
    xmlNodePtr node;

    while (element) {
        if (0 == xmlStrcmp(BAD_CAST"device", element->name)) {
            if (var__allocate_functional_object(sizeof ( var__curtis_t), kVarType_Curtis, &object) < 0) {
                break;
            }
            curtis = var__object_body_ptr(var__curtis_t, object);

            node = element->xmlChildrenNode;
            while (node) {
                if (0 == xmlStrcmp(BAD_CAST"id", node->name)) {
                    object->object_id_ = covert_first_text_value_l(node);
                } else if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
                    covert_first_text_value_s(node, object->name_, cchof(object->name_));
                } else if (0 == xmlStrcmp(BAD_CAST"internel_type", node->name)) {
                    curtis->internel_type_ = covert_first_text_value_i(node);
                }else if (0 == xmlStrcmp(BAD_CAST"send_id", node->name)) {
                    curtis->send_id_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"recv_id", node->name)) {
                    curtis->recv_id_ = covert_first_text_value_i(node);
                } else if (0 == xmlStrcmp(BAD_CAST"byte_order", node->name)) {
                    curtis->byte_order_ = (enum var__byte_orders_t)covert_first_text_value_i(node);
                } else {
                    xml__check_candevice_head(node, &curtis->candev_head_);
                }

                node = node->next;
            }

#if ARM_LINUX
			// 插入本地管理列表
			list_add_tail(&object->local_, &__local);

			// 插入全局的对象管理列表
			var__insert_object(object);
#else
            if (vari__init_curtis_object(object) >= 0) {

                if (curtis->candev_head_.merge_) {
                    var__attach_busrw(curtis->candev_head_.canbus_, &curtis_create_dup, &curtis_release_dup, object->handle_);
                } else {
                    curtis->candev_head_.self_rw_ = (void *) (posix__pthread_t *) malloc(sizeof ( posix__pthread_t));
                    posix__pthread_create((posix__pthread_t *) & curtis->candev_head_.self_rw_, &vari__curtis_self_rw, curtis);
                }

                // 插入本地管理列表
                list_add_tail(&object->local_, &__local);

                // 插入全局的对象管理列表
                var__insert_object(object);
            } else {
                log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                        "failed to init curtis device node for object:%d", object->object_id_);
                var__delete_functional_object(object);
            }
#endif
        }
        element = element->next;
    }

    return 0;
}

// 从 ./etc/dev/curtis.xml 读取所有的 curtis 配置对象，并加入到对象管理区
int var__load_curtis() {

	INIT_LIST_HEAD( &__local );

    if (xml__read_data("dev","curtis.xml", "curtis", vari__curtis_loaded_handler)) {
        return -1;
    }

    return 0;
}

var__curtis_t *var__get_curtis(objhld_t hld) {
    var__functional_object_t *obj;
    var__curtis_t *curtis;

    if (hld > 0) {
        obj = objrefr(hld);
        if (obj) {
            if (obj->type_ != kVarType_Curtis) {
                objdefr(hld);
                return NULL;
            }
            curtis = var__object_body_ptr(var__curtis_t, obj);
            var__acquire_lock(obj);
            return curtis;
        }
    }

    return NULL;
}

var__curtis_t *var__get_curtis_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld < 0) {
        return NULL;
    }
    return var__get_curtis(hld);
}

var__curtis_t *var__create_curtis_dup(objhld_t hld) {
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
    return var__object_body_ptr(var__curtis_t, dobj);
}

var__curtis_t *var__create_curtis_dup_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld >= 0) {
        return var__create_curtis_dup(hld);
    }
    return NULL;
}

void var__release_curtis_dup(var__curtis_t *curtis) {
    var__functional_object_t *dobj;

    dobj = var__object_head_ptr(curtis);
    if (dobj) {
        posix__pthread_mutex_release(&dobj->locker_);
        free(dobj);
    }
}

int var__commit_curtis_dup(const var__curtis_t *curtis) {
    var__functional_object_t *oobj, *dobj;
    char *cpydst;
    const char *cpysrc;

    if (!curtis) return -1;

    dobj = var__object_head_ptr(curtis);
    if (!dobj) return -1;

    oobj = objrefr(dobj->dup_);
    if (!oobj) return -1;
    
    var__acquire_lock(oobj);

    cpydst = oobj->body_;
    cpydst += offsetof(var__curtis_t, u_);
    cpysrc = (const char *) curtis;
    cpysrc += offsetof(var__curtis_t, u_);
    memcpy(cpydst, cpysrc, sizeof ( struct _i_curtis_internal_t));

    var__release_lock(oobj);

    objdefr(dobj->dup_);
    return 0;
}
