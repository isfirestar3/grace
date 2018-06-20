#include "drive_unit.h"
#include "object.h"
#include "var.h"
#include "xml_loaded.h"
#include "wheel.h"

static
struct list_head __local = {0};
static
int __object_count = 0;

static
void fill_position_properties(position_t *pos, xmlNode *node) {

    xmlAttr *attr;
    const char *sz;

    if (!pos || !node) return;

    attr = node->properties;
    if (!attr) return;

    while (attr) {
        if (0 == xmlStrcmp(attr->name, BAD_CAST"x")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                pos->x_ = strtod(sz, NULL);
            }
        } else if (0 == xmlStrcmp(attr->name, BAD_CAST"y")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                pos->y_ = strtod(sz, NULL);
            }
        } else if (0 == xmlStrcmp(attr->name, BAD_CAST"theta")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                pos->theta_ = strtod(sz, NULL);
            }
        }
        attr = attr->next;
    }
}

static
int vari__drive_unit_loaded_handler(xmlNodePtr element) {

    var__functional_object_t *object;
    var__drive_unit_t *unit;
    associated_object_t *associated_object;
    xmlNodePtr node;
    xmlNodePtr lower;

    while (element) {
        if (0 != xmlStrcmp(BAD_CAST"unit", element->name)) {
            element = element->next;
            continue;
        }

        if (var__allocate_functional_object(sizeof ( var__drive_unit_t), kVarType_DriveUnit, &object) < 0) {
            break;
        }
        unit = var__object_body_ptr(var__drive_unit_t, object);
        INIT_LIST_HEAD(&unit->associated_);

        node = element->xmlChildrenNode;
        while (node) {
            if (0 == xmlStrcmp(BAD_CAST"id", node->name)) {
                object->object_id_ = covert_first_text_value_i(node);
            } else if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
                covert_first_text_value_s(node, object->name_, cchof(object->name_));
            } else if (0 == xmlStrcmp(BAD_CAST"type", node->name)) {
                unit->unit_type_ = (enum drive_unit_type_t)covert_first_text_value_i(node);
            } else if (0 == xmlStrcmp(BAD_CAST"install", node->name)) { // 从 install 节读出装配属性
                fill_position_properties(&unit->install_, node);
            } else if (0 == xmlStrcmp(BAD_CAST"calibrated", node->name)) { // 从 calibrated 节读出校准属性
                fill_position_properties(&unit->calibrated_, node);
            } else if (0 == xmlStrcmp(BAD_CAST"wheels", node->name)) {
                lower = node->children;
                while (lower) {
                    if (0 == xmlStrcmp(BAD_CAST"wheel", lower->name)) {
                        const char *value = get_first_text_value(lower);
                        if (value) {
                            associated_object = malloc(sizeof ( associated_object_t));
                            associated_object->object_id_ = strtol(value, NULL, 10);
                            list_add_tail(&associated_object->link_, &unit->associated_);
                            ++unit->associated_cnt_;
                        }
                    }
                    lower = lower->next;
                }
            }
            node = node->next;
        }

        // 插入本地管理列表
        list_add_tail(&object->local_, &__local);
        __object_count++;
        
        // 插入全局的对象管理列表
        var__insert_object(object);
        element = element->next;
    }

    return 0;
}

int var__load_driveunit() {

    INIT_LIST_HEAD(&__local);

    return xml__read_data("var", "driveunit.xml", "drives", &vari__drive_unit_loaded_handler);
}

const struct list_head *var__get_driveunit(int *count) {
    if (count) {
        *count = __object_count;
    }
    return &__local;
}

var__drive_unit_t *var__create_driveunit_dup(objhld_t hld) {
    var__functional_object_t *oobj, *dobj;
    var__drive_unit_t *ounit, *dunit;
    struct list_head *loop;
    associated_object_t *associated_d, *associated_o;

    oobj = (var__functional_object_t *) objrefr(hld);
    if (!oobj) return NULL;
    ounit = var__object_body_ptr(var__drive_unit_t, oobj);

    dunit = NULL;
    dobj = (var__functional_object_t *) malloc(oobj->length_);
    if (dobj) {
        dunit = var__object_body_ptr(var__drive_unit_t, dobj);

        var__acquire_lock(oobj);
        memcpy(dobj, oobj, oobj->length_);
        dobj->dup_ = oobj->handle_;

        // associated_ 不适用于普通拷贝
        INIT_LIST_HEAD(&dunit->associated_);

        list_for_each(loop, &ounit->associated_) {
            associated_o = containing_record(loop, associated_object_t, link_);
            associated_d = (associated_object_t *) malloc(sizeof ( associated_object_t));
            if (associated_d) {
                associated_d->object_id_ = associated_o->object_id_;
                list_add_tail(&associated_d->link_, &dunit->associated_);
            }
        }
        var__release_lock(oobj);

        // 初始化赋值的对象锁
        posix__pthread_mutex_init(&dobj->locker_);

        // 原始对象的链表不需要拷贝
        INIT_LIST_HEAD(&dobj->pagefault_);
        INIT_LIST_HEAD(&dobj->global_);
    }

    objdefr(hld);
    return dunit;
}

void var__release_driveunit_dup(var__drive_unit_t *unit) {
    var__functional_object_t *dobj;
    struct list_head *loop;
    associated_object_t *associated;

    if (unit) {
        dobj = var__object_head_ptr(unit);

        // 释放 associated

        list_for_each(loop, &unit->associated_) {
            associated = containing_record(loop, associated_object_t, link_);
            free(associated);
        }
        posix__pthread_mutex_release(&dobj->locker_);
        free(dobj);
    }
}

int var__driveunit_parse_to_functional(const var__drive_unit_t *unit, void **functionals, int *count) {
    struct list_head *loop;
    associated_object_t *assobj;
    int i = 0;

    if (!unit || !functionals) return -1;

    if (unit->associated_cnt_ <= 0) return -1;

    list_for_each(loop, &unit->associated_) {
        assobj = containing_record(loop, associated_object_t, link_);
        functionals[i++] = var__create_wheel_dup_byid(assobj->object_id_);
    }

    if (count) {
        *count = i;
    }
    return 0;
}