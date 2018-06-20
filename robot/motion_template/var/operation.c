#include "operation.h"
#include "var.h"

static
objhld_t __local = -1;

int var__load_opt() {
    var__operation_t *opt;
    var__functional_object_t *object;

    if (var__allocate_functional_object(sizeof ( var__operation_t), kVarType_Operation, &object) < 0) {
        return -1;
    }
    object->object_id_ = kVarFixedObject_Operation;
    
    opt = var__object_body_ptr(var__operation_t, object);
    var__init_status_describe(&opt->status_);

    __local = object->handle_;
    var__insert_object(object);
    return 0;
}

var__operation_t *var__get_opt() {
    var__functional_object_t *obj;
    var__operation_t *opt;

    if (__local > 0) {
        obj = objrefr(__local);
        if (obj) {
            opt = var__object_body_ptr(var__operation_t, obj);
            var__acquire_lock(obj);
            return opt;
        }
    }

    return NULL;
}

var__operation_t *var__create_opt_dup() {
    var__functional_object_t *obj, *dobj;
    var__operation_t *dopt;

    obj = objrefr(__local);
    if (!obj) {
        return NULL;
    }

    dopt = NULL;
    dobj = (var__functional_object_t *) malloc(obj->length_);
    if (dobj) {
        var__acquire_lock(obj);
        memcpy(dobj, obj, obj->length_);

        // 记录复制来源
        dobj->dup_ = obj->handle_;
        var__release_lock(obj);

        dopt = var__object_body_ptr(var__operation_t, dobj);

        // 初始化赋值的对象锁
        posix__pthread_mutex_init(&dobj->locker_);

        // 原始对象的链表不需要拷贝
        INIT_LIST_HEAD(&dobj->pagefault_);
        INIT_LIST_HEAD(&dobj->global_);
    }

    objdefr(__local);
    return dopt;
}

int var__commit_opt_dup(const var__operation_t *opt) {
    var__functional_object_t *oobj, *dobj;
    var__operation_t *oopt;
    char *cpydst;
    const char *cpysrc;

    if (!opt) return -1;

    dobj = var__object_head_ptr(opt);
    if (!dobj) return -1;

    oobj = objrefr(dobj->dup_);
    if (!oobj) return -1;

    oopt = var__object_body_ptr(var__operation_t, oobj);
    var__acquire_lock(oobj);

    cpydst = oobj->body_;
    cpydst += offsetof(var__operation_t, u_);
    cpysrc = (const char *) opt;
    cpysrc += offsetof(var__operation_t, u_);
    memcpy(cpydst, cpysrc, sizeof ( struct _i_operation_internal_t));

    var__commit_status_describe_dup(&oopt->status_, &opt->status_);

    var__release_lock(oobj);

    objdefr(dobj->dup_);
    return 0;
}

void var__release_opt_dup(var__operation_t *opt) {
    var__functional_object_t *dobj;

    if (!opt) return;

    dobj = var__object_head_ptr(opt);
    if (dobj) {
        posix__pthread_mutex_release(&dobj->locker_);
        free(dobj);
    }
}