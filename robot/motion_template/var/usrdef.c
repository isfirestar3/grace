#include "usrdef.h"
#include "var.h"

static
objhld_t __local = -1;

int var__load_usrbuf() {
    var__functional_object_t *object;

    if (var__allocate_functional_object(sizeof ( var__usrdef_buffer_t), kVarType_UserDefined, &object) < 0) {
        return -1;
    }
    object->object_id_ = kVarFixedObject_UserDefinition;

    __local = object->handle_;
    var__insert_object(object);
    return 0;
}

var__usrdef_buffer_t *var__get_usrbuf() {
    var__functional_object_t *obj;
    var__usrdef_buffer_t *usrbuf;

    if (__local > 0) {
        obj = objrefr(__local);
        if (obj) {
            usrbuf = var__object_body_ptr(var__usrdef_buffer_t, obj);
            var__acquire_lock(obj);
            return usrbuf;
        }
    }

    return NULL;
}

var__usrdef_buffer_t *var__create_usrbuf_dup() {
    var__functional_object_t *obj, *dobj;
    var__usrdef_buffer_t *dusrbuf;

    obj = objrefr(__local);
    if (!obj) {
        return NULL;
    }

    dusrbuf = NULL;
    dobj = (var__functional_object_t *) malloc(obj->length_);
    if (dobj) {
        var__acquire_lock(obj);
        memcpy(dobj, obj, obj->length_);

        // 记录复制来源
        dobj->dup_ = obj->handle_;
        var__release_lock(obj);

        dusrbuf = var__object_body_ptr(var__usrdef_buffer_t, dobj);

        // 初始化赋值的对象锁
        posix__pthread_mutex_init(&dobj->locker_);

        // 原始对象的链表不需要拷贝
        INIT_LIST_HEAD(&dobj->pagefault_);
        INIT_LIST_HEAD(&dobj->global_);
    }

    objdefr(__local);
    return dusrbuf;
}

int var__commit_usrbuf_dup(const var__usrdef_buffer_t *usrbuf) {
    var__functional_object_t *oobj, *dobj;
    char *cpydst;
    const char *cpysrc;

    if (!usrbuf) return -1;

    dobj = var__object_head_ptr(usrbuf);
    if (!dobj) return -1;

    oobj = objrefr(dobj->dup_);
    if (!oobj) return -1;

    var__acquire_lock(oobj);

    cpydst = oobj->body_;
    cpysrc = (const char *) usrbuf;
    memcpy(cpydst, cpysrc, sizeof ( var__usrdef_buffer_t));

    var__release_lock(oobj);

    objdefr(dobj->dup_);
    return 0;
}

void var__release_usrbuf_dup(var__usrdef_buffer_t *usrbuf) {
    var__functional_object_t *dobj;

    if (!usrbuf) return;

    dobj = var__object_head_ptr(usrbuf);
    if (dobj) {
        posix__pthread_mutex_release(&dobj->locker_);
        free(dobj);
    }
}