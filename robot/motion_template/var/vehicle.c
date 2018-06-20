#include "vehicle.h"
#include "object.h"
#include "var.h"
#include "xml_loaded.h"
#include "args.h"
#include "logger.h"

#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static
objhld_t __local = -1;

var__vehicle_t *var__get_vehicle() {
    var__functional_object_t *obj;
    var__vehicle_t *veh;

    if (__local > 0) {
        obj = objrefr(__local);
        if (obj) {
            veh = var__object_body_ptr(var__vehicle_t, obj);
            var__acquire_lock(obj);
            return veh;
        }
    }

    return NULL;
}

static
int vari__vehicle_loaded_handler(xmlNodePtr element) {
    var__vehicle_t *vehicle;
    var__functional_object_t *object;

    if (var__allocate_functional_object(sizeof ( var__vehicle_t), kVarType_Vehicle, &object) >= 0) {
        object->object_id_ = kVarFixedObject_Vehide;

        vehicle = var__object_body_ptr(var__vehicle_t, object);

        while (element) {
            if (0 == xmlStrcmp(BAD_CAST"name", element->name)) {
                covert_first_text_value_s(element, object->name_, cchof(object->name_));
            } else if (0 == xmlStrcmp(BAD_CAST"vehicle_id", element->name)) {
                vehicle->vehicle_id_ = covert_first_text_value_i(element);
            } else if (0 == xmlStrcmp(BAD_CAST"vehicle_name", element->name)) {
                covert_first_text_value_s(element, vehicle->vehicle_name_, cchof(vehicle->vehicle_name_));
            } else if (0 == xmlStrcmp(BAD_CAST"vehicle_type", element->name)) {
                vehicle->vehicle_type_ = covert_first_text_value_i(element);
            } else if (0 == xmlStrcmp(BAD_CAST"chassis_type", element->name)) {
                vehicle->chassis_type_ = (char) covert_first_text_value_l(element);
            } else if (0 == xmlStrcmp(BAD_CAST"max_speed", element->name)) {
                vehicle->max_speed_ = covert_first_text_value_d(element);
            } else if (0 == xmlStrcmp(BAD_CAST"creep_speed", element->name)) {
                vehicle->creep_speed_ = covert_first_text_value_d(element);
            } else if (0 == xmlStrcmp(BAD_CAST"max_acc", element->name)) {
                vehicle->max_acc_ = covert_first_text_value_d(element);
            } else if (0 == xmlStrcmp(BAD_CAST"max_dec", element->name)) {
                vehicle->max_dec_ = covert_first_text_value_d(element);
            } else if (0 == xmlStrcmp(BAD_CAST"max_w", element->name)) {
                vehicle->max_w_ = covert_first_text_value_d(element);
            } else if (0 == xmlStrcmp(BAD_CAST"creep_w", element->name)) {
                vehicle->creep_w_ = covert_first_text_value_d(element);
            } else if (0 == xmlStrcmp(BAD_CAST"max_acc_w", element->name)) {
                vehicle->max_acc_w_ = covert_first_text_value_d(element);
            } else if (0 == xmlStrcmp(BAD_CAST"max_dec_w", element->name)) {
                vehicle->max_dec_w_ = covert_first_text_value_d(element);
            } else if (0 == xmlStrcmp(BAD_CAST"steer_angle_error_tolerance", element->name)) {
                vehicle->steer_angle_error_tolerance_ = covert_first_text_value_d(element);
            } else {
                ;
            }
            element = element->next;
        }

        /* 如果启动参数中指定了模拟运行， 则应该查看启动参数中是否有指定底盘的起始坐标 */
        if (run__getarg_simflag()) {
            run__getarg_simodo(&vehicle->i.odo_meter_.x_, &vehicle->i.odo_meter_.y_, &vehicle->i.odo_meter_.angle_);
        }

        // 底盘使能缺省为起用
        vehicle->enable_ = 1;

        // 插入本地管理列表
        __local = object->handle_;

        // 插入全局的对象管理列表
        var__insert_object(object);
        return 0;
    }

    return -1;
}

int var__load_vehicle() {
	return xml__read_data("var", "vehicle.xml", "vehicle", &vari__vehicle_loaded_handler);
}

// 复制一份 vehicle 对象

var__vehicle_t *var__create_vehicle_dup() {
    var__functional_object_t *oobj, *dobj;
    var__vehicle_t *dveh;

    oobj = objrefr(__local);
    if (!oobj) {
        return NULL;
    }

    dveh = NULL;
    dobj = (var__functional_object_t *) malloc(oobj->length_);
    if (dobj) {
        dveh = var__object_body_ptr(var__vehicle_t, dobj);

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
    }
    objdefr(__local);
    return dveh;
}

int var__commit_vehicle_dup(const var__vehicle_t *veh) {
    var__functional_object_t *oobj, *dobj;

    if (!veh) {
        return -1;
    }

    dobj = var__object_head_ptr(veh);
    if (!dobj) {
        return -1;
    }

    oobj = objrefr(dobj->dup_);
    if (!oobj) {
        return -1;
    }

    var__acquire_lock(oobj);
#if _WIN32
    char *cpydst = oobj->body_;
    cpydst += offsetof(var__vehicle_t, u_);
    const char *cpysrc = (const char *) veh;
    cpysrc += offsetof(var__vehicle_t, u_);
    memcpy(cpydst, cpysrc, sizeof ( struct _i_vehicle_internal_t));
#else
    /*
        In the arm runtime environment,
        many fields of the value will be reported from VCU, without actual calculation through the navigation cycle. 
        If submitted according to the 'copy objects' of Win32, data may be rolled up.
    */
    var__vehicle_t *oveh = (var__vehicle_t *)oobj->body_;
    oveh->i.command_velocity_ = veh->i.command_velocity_;
    oveh->i.ref_velocity_ = veh->i.ref_velocity_;
    oveh->i.actual_command_velocity_ = veh->i.actual_command_velocity_;

    oveh->i.is_moving_ = veh->i.is_moving_;

    oveh->i.normal_stopped_ = veh->i.normal_stopped_;
    oveh->i.emergency_stopped_ = veh->i.emergency_stopped_;
    oveh->i.slow_done_ = veh->i.slow_done_;

    oveh->i.total_odo_meter_ = veh->i.total_odo_meter_;
#endif
    var__release_lock(oobj);

    objdefr(dobj->dup_);
    return 0;
}

void var__release_vehicle_dup(var__vehicle_t *dup) {
    var__functional_object_t *dobj;

    if (!dup) return;

    dobj = var__object_head_ptr(dup);
    if (dobj) {
        posix__pthread_mutex_release(&dobj->locker_);
        free(dobj);
    }
}