#include "optpar.h"
#include "var.h"
#include "object.h"

static
objhld_t __local = -1;

int var__load_optpar() {

    var__functional_object_t *object;
    var__operation_parameter_t *target;

    if (var__allocate_functional_object(sizeof ( var__operation_parameter_t), kVarType_OperationTarget, &object) < 0) {
        return -1;
    }
    object->object_id_ = kVarFixedObject_OptPar;
    
    target = var__object_body_ptr(var__operation_parameter_t, object);

    __local = object->handle_;

    memset(target, 0, sizeof ( var__operation_parameter_t));
    return 0;
}

var__operation_parameter_t *var__get_optpar() {
    var__functional_object_t *obj;
    var__operation_parameter_t *target;

    if (__local > 0) {
        obj = objrefr(__local);
        if (obj) {
            if (obj->type_ != kVarType_OperationTarget) {
                objdefr(__local);
                return NULL;
            }
            target = var__object_body_ptr(var__operation_parameter_t, obj);
            var__acquire_lock(obj);
            return target;
        }
    }

    return NULL;
}