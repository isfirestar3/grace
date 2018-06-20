#include "error.h"
#include "xml_loaded.h"
#include "vehicle.h"
#include "navigation.h"
#include "logger.h"

static
objhld_t __local = -1;

static
void vari__loaded_framwork_error_config(xmlNodePtr node, var__error_handler_t *err) {
    while (node) {
        if (0 == xmlStrcmp(BAD_CAST"navigation_timeout_as_fatal", node->name)) {
            err->navigation_timeout_as_fatal_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"navigation_error_as_fatal", node->name)) {
            err->navigation_error_as_fatal_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"busrw_timeout_as_fatal", node->name)) {
            err->busrw_timeout_as_fatal_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"driverrw_timeout_as_fatal", node->name)) {
            err->driverrw_timeout_as_fatal_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"average_request_completion_delay_as_fatal", node->name)) {
            err->average_request_completion_delay_as_fatal_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"average_request_checking_window_if_enabled", node->name)) {
            err->average_request_checking_window_if_enabled_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"tcp_master_object_destoryed_as_fatal", node->name)) {
            err->tcp_master_object_destoryed_as_fatal_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"process_scheduler_interrupted_as_fatal", node->name)) {
            err->process_scheduler_interrupted_as_fatal_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"valloc_fault_as_fatal", node->name)) {
            err->valloc_fault_as_fatal_ = covert_first_text_value_i(node);
        } else {
            ;
        }
        node = node->next;
    }
}

static
void vari__loaded_candrvier_error_config(xmlNodePtr node, var__error_handler_t *err) {
    while (node) {
        if (0 == xmlStrcmp(BAD_CAST"driver_err_frame_window", node->name)) {
            err->driver_err_frame_window_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"driver_err_cons_lost_cnt", node->name)) {
            err->driver_err_cons_lost_cnt_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"driver_err_thres_framelost", node->name)) {
            err->driver_err_thres_framelost_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"driver_err_speed_follow_error", node->name)) {
            err->driver_err_speed_follow_error_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"driver_err_speed_min_acc", node->name)) {
            err->driver_err_speed_min_acc_ = covert_first_text_value_d(node);
        } else {
            ;
        }
        node = node->next;
    }
}

static
void vari__loaded_navigation_error_config(xmlNodePtr node, var__error_handler_t *err) {
    while (node) {
        if (0 == xmlStrcmp(BAD_CAST"localization_timeout", node->name)) {
            err->localization_timeout_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"odo_timeout", node->name)) {
            err->odo_timeout_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"average_localization_delay_threshold", node->name)) {
            err->average_localization_delay_threshold_ = covert_first_text_value_i(node);
        } else if (0 == xmlStrcmp(BAD_CAST"average_odo_delay_threshold", node->name)) {
            err->average_odo_delay_threshold_ = covert_first_text_value_i(node);
        } else {
            ;
        }
        node = node->next;
    }
}

static
int vari__error_loaded_handler(xmlNodePtr element) {
    var__error_handler_t *err;
    var__functional_object_t *object;

    if (var__allocate_functional_object(sizeof ( var__error_handler_t), kVarType_ErrorHandler, &object) < 0) {
        return -1;
    }
    err = var__object_body_ptr(var__error_handler_t, object);
    object->object_id_ = kVarFixedObject_ErrorCollecter;

    while (element) {
        if (0 == xmlStrcmp(BAD_CAST"framwork", element->name)) {
            vari__loaded_framwork_error_config(element->children, err);
        } else if (0 == xmlStrcmp(BAD_CAST"candriver", element->name)) {
            vari__loaded_candrvier_error_config(element->children, err);
        } else if (0 == xmlStrcmp(BAD_CAST"navigation", element->name)) {
            vari__loaded_navigation_error_config(element->children, err);
        } else {
            ;
        }
        element = element->next;
    }

    __local = object->handle_;
    var__insert_object(object);
    return 0;
}

int var__load_error_handler_object() {

    return xml__read_data("var", "error.xml", "error", &vari__error_loaded_handler);
}

var__error_handler_t *var__get_error_handler() {
    var__functional_object_t *obj;
    var__error_handler_t *errobj;

    if (__local > 0) {
        obj = objrefr(__local);
        if (obj) {
            if (obj->type_ != kVarType_ErrorHandler) {
                objdefr(__local);
                return NULL;
            }
            errobj = var__object_body_ptr(var__error_handler_t, obj);
            var__acquire_lock(obj);
            return errobj;
        }
    }

    return NULL;
}

void var__mark_framwork_error(int fixed_object_id, int error_code) {
    var__error_handler_t *err;
    var__error_item_t *item;

    if (fixed_object_id <= 0 || fixed_object_id >= MAXIMUM_OBJECT_INDEX) {
        return;
    }

    err = var__get_error_handler();
    if (err) {
        item = &err->error_[fixed_object_id];
        if (0 == error_code) {
            item->framwork = 0;
            if ( ( 0 == item->hardware ) && (0 == item->software)) {
                item->status = 0;
            }
        } else {
            item->status = -1;
            item->framwork = error_code;
        }
        var__release_object_reference(err);
    }
}

void var__mark_software_error(int fixed_object_id, int error_code) {
    var__error_handler_t *err;
    var__error_item_t *item;

    if (fixed_object_id <= 0 || fixed_object_id >= MAXIMUM_OBJECT_INDEX) {
        return;
    }

    err = var__get_error_handler();
    if (err) {
        item = &err->error_[fixed_object_id];
        if (0 == error_code) {
            item->software = 0;
            if ( ( 0 == item->hardware ) && (0 == item->framwork)) {
                item->status = 0;
            }
        } else {
            item->status = -1;
            item->software = error_code;
        }
        var__release_object_reference(err);
    }
}

void var__mark_hardware_error(int fixed_object_id, int error_code) {
    var__error_handler_t *err;
    var__error_item_t *item;

    if (fixed_object_id <= 0 || fixed_object_id >= MAXIMUM_OBJECT_INDEX) {
        return;
    }

    err = var__get_error_handler();
    if (err) {
        item = &err->error_[fixed_object_id];
        if (0 == error_code) {
            item->hardware = 0;
            if ( ( 0 == item->software ) && (0 == item->framwork)) {
                item->status = 0;
            }
        } else {
            item->status = -1;
            item->hardware = error_code;
        }
        var__release_object_reference(err);
    }
}

int var__clear_fault() {
    var__error_handler_t *err;
    int i;
    var__vehicle_t *veh;
    var__navigation_t *nav;

    err = var__get_error_handler();
    if (err) {
        for (i = 0; i < MAXIMUM_OBJECT_INDEX; i++) {
            err->error_[i].status = 0;
        }
        var__release_object_reference(err);
    }

    nav = var__get_navigation();
    if (nav) {
        nav->i.tracking_error_ = 0;
        var__release_object_reference(nav);
    }

    /* all software errors canbe cleanup immediately, 
       but errors from device driver are not guaranteed tobe cleared. 
       mark clear bit to 1 if arch is arm-linux */
    veh = var__get_vehicle();
    if (veh) {
#if _WIN32
        veh->fault_stop_ = VEH_HEALTHY;
#else
        veh->fault_stop_ &= ~VEH_SOFTWARE_FAULT;
        veh->fault_stop_ |= VEH_MANUAL_CLEAR;
#endif
        var__release_object_reference(veh);
        log__save("motion_template", kLogLevel_Warning, kLogTarget_Stdout | kLogTarget_Filesystem, "clear fault command received");
    }

    return 0;
}