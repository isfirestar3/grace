#include "safety.h"
#include "var.h"
#include "vartypes.h"

static
objhld_t __local = -1;

int var__load_safety(){
    var__functional_object_t *object;

    if (var__allocate_functional_object(sizeof ( var__safety_t), kVarType_SafetyProtec, &object) < 0) {
        return -1;
    }
    object->object_id_ = kVarFixedObject_SaftyProtec;

    /* 记录本地对象 */
     __local = object->handle_;

    // 插入全局的对象管理列表
    var__insert_object(object);
    
    return 0;
}

var__safety_t *var__get_safety(){
     var__functional_object_t *obj;
    var__safety_t *safety;

    if (__local > 0) {
        obj = objrefr(__local);
        if (obj) {
            if (obj->type_ != kVarType_SafetyProtec) {
                objdefr(__local);
                return NULL;
            }
            safety = var__object_body_ptr(var__safety_t, obj);
            var__acquire_lock(obj);
            return safety;
        }
    }

    return NULL;
}