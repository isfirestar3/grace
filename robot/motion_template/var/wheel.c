#include "wheel.h"
#include "object.h"
#include "var.h"
#include "xml_loaded.h"

#include <time.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
struct list_head __local = {0};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static
int vari__swheel_loaded_handler(xmlNodePtr element) {

    var__functional_object_t *object;
    xmlNodePtr node;
    var__swheel_t *swheel;

    while (element) {
        if (0 != xmlStrcmp(BAD_CAST"wheel", element->name)) {
            element = element->next;
            continue;
        }

        if (var__allocate_functional_object(sizeof ( var__swheel_t), kVarType_SWheel, &object) < 0) {
            break;
        }
        swheel = var__object_body_ptr(var__swheel_t, object);

        node = element->xmlChildrenNode;
        while (node) {
            if (0 == xmlStrcmp(BAD_CAST"id", node->name)) {
                object->object_id_ = covert_first_text_value_l(node);
            } else if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
                covert_first_text_value_s(node, object->name_, cchof(object->name_));
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"min_angle")) {
                swheel->min_angle_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"max_angle")) {
                swheel->max_angle_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"zero_angle")) {
                swheel->zero_angle_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"zero_angle_enc")) {
                swheel->zero_angle_enc_ = covert_first_text_value_ull(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"max_w")) {
                swheel->max_w_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"enable")) {
                swheel->i.enable_ = covert_first_text_value_i(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"control_mode")) {
                swheel->control_mode_ = covert_first_text_value_i(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"scale_control")) {
                swheel->scale_control_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"scale_feedback")) {
                swheel->scale_feedback_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"control_cp")) {
                swheel->control_cp_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"control_ci")) {
                swheel->control_ci_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"control_cd")) {
                swheel->control_cd_ = covert_first_text_value_d(node);
            } else {
                ;
            }
            node = node->next;
        }

        swheel->time_stamp_ = time((time_t *) NULL);

        // 插入本地管理列表
        list_add_tail(&object->local_, &__local);

        // 插入全局的对象管理列表
        var__insert_object(object);

        element = element->next;
    }

    return 0;
}

int var__load_swheel() {

	INIT_LIST_HEAD(&__local);
    return xml__read_data("var", "swheel.xml", "wheels", vari__swheel_loaded_handler);
}

static
int vari__dwheel_loaded_handler(xmlNodePtr element) {
    var__functional_object_t *object;
    xmlNodePtr node;
    var__dwheel_t *dwheel;

    while (element) {
        if (0 != xmlStrcmp(BAD_CAST"wheel", element->name)) {
            element = element->next;
            continue;
        }

        if (var__allocate_functional_object(sizeof ( var__dwheel_t), kVarType_DWheel, &object) < 0) {
            break;
        }
        dwheel = var__object_body_ptr(var__dwheel_t, object);

        node = element->xmlChildrenNode;
        while (node) {
            if (0 == xmlStrcmp(BAD_CAST"id", node->name)) {
                object->object_id_ = covert_first_text_value_l(node);
            } else if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
                covert_first_text_value_s(node, object->name_, cchof(object->name_));
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"max_speed")) {
                dwheel->max_speed_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"max_acc")) {
                dwheel->max_acc_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"max_dec")) {
                dwheel->max_dec_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"enable")) {
                dwheel->i.enable_ = covert_first_text_value_i(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"control_mode")) {
                dwheel->control_mode_ = covert_first_text_value_i(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"scale_control")) {
                dwheel->scale_control_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"scale_feedback")) {
                dwheel->scale_feedback_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"roll_weight")) {
                dwheel->roll_weight_ = covert_first_text_value_d(node);
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"slide_weight")) {
                dwheel->slide_weight_ = covert_first_text_value_d(node);
            }

            node = node->next;
        }

        dwheel->time_stamp_ = time((time_t *) NULL);

        // 插入本地管理列表
        list_add_tail(&object->local_, &__local);

        // 插入全局的对象管理列表
        var__insert_object(object);
        element = element->next;
    }

    return 0;
}

int var__load_dwheel() {
        
	INIT_LIST_HEAD(&__local);

    return xml__read_data("var", "dwheel.xml", "wheels", vari__dwheel_loaded_handler);
}

static
int vari__sddext_loaded_handler(xmlNodePtr element) {

    var__functional_object_t *object;
    xmlNodePtr node;
    var__sdd_extra_t *sddwheel;

    while (element) {
        if (0 != xmlStrcmp(BAD_CAST"wheel", element->name)) {
            element = element->next;
            continue;
        }

        if (var__allocate_functional_object(sizeof ( var__sdd_extra_t), kVarType_SDDExtra, &object) < 0) {
            break;
        }
        sddwheel = var__object_body_ptr(var__sdd_extra_t, object);

        node = element->xmlChildrenNode;
        while (node) {
            if (0 == xmlStrcmp(BAD_CAST"id", node->name)) {
                object->object_id_ = covert_first_text_value_l(node);
            } else if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
                covert_first_text_value_s(node, object->name_, cchof(object->name_));
            } else if (0 == xmlStrcmp(node->name, BAD_CAST"gauge")) {
                sddwheel->gauge_ = covert_first_text_value_d(node);
            } else {
                ;
            }

            node = node->next;
        }

        // 插入本地管理列表
        list_add_tail(&object->local_, &__local);

        // 插入全局的对象管理列表
        var__insert_object(object);
        element = element->next;
    }

    return 0;
}

int var__load_sddext() {

	INIT_LIST_HEAD( &__local );

    return xml__read_data("var", "sddex.xml", "wheels", vari__sddext_loaded_handler);
}

void *var__get_wheel(objhld_t hld) {
    var__functional_object_t *obj;
	
    obj = objrefr(hld);
    if (obj) {
        if (obj->type_ != kVarType_DWheel && obj->type_ != kVarType_SWheel && obj->type_ != kVarType_SDDExtra) {
            objdefr(hld);
            return NULL;
        }
        var__acquire_lock(obj);
        return (void *) obj;
    }

    return NULL;
}

void *var__get_wheel_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld >= 0) {
        return var__get_wheel(hld);
    }

    return NULL;
}

void *var__create_wheel_dup(objhld_t hld) {
    var__functional_object_t *dobj, *obj;

    obj = objrefr(hld);
    if (!obj) {
        return NULL;
    }

    dobj = NULL;
    if (obj->type_ == kVarType_SWheel || obj->type_ == kVarType_DWheel || obj->type_ == kVarType_SDDExtra) {
        dobj = (var__functional_object_t *) malloc(obj->length_);
        if (dobj) {
            var__acquire_lock(obj);
            memcpy(dobj, obj, obj->length_);
            dobj->dup_ = obj->handle_;
            var__release_lock(obj);

            // 初始化赋值的对象锁
            posix__pthread_mutex_init(&dobj->locker_);

            // 原始对象的链表不需要拷贝
            INIT_LIST_HEAD(&dobj->pagefault_);
            INIT_LIST_HEAD(&dobj->global_);
        }
    }

    objdefr(hld);
    return dobj;
}

void *var__create_wheel_dup_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld >= 0) {
        return var__create_wheel_dup(hld);
    }
    return NULL;
}

void var__release_wheel_dup(void *dup) {
    var__functional_object_t *obj = (var__functional_object_t *) dup;
    if (obj) {
        posix__pthread_mutex_release(&obj->locker_);
        free(dup);
    }
}

int var__commit_wheel_dup(const void *dup) {
    var__functional_object_t *dobj = (var__functional_object_t *) dup;
    var__functional_object_t *oobj;
    objhld_t hld;

    if (!dup) return -1;

    hld = dobj->dup_;
    oobj = objrefr(hld);
    if (!oobj) {
        return -1;
    }

    var__acquire_lock(oobj);
    memcpy(oobj->body_, dobj->body_, oobj->body_length_);
    var__release_lock(oobj);

    objdefr(hld);
    return 0;
}