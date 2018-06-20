#include "navigation.h"
#include "var.h"
#include "xml_loaded.h"

static
objhld_t __local = -1;

var__navigation_t *var__create_navigation_dup(int ignore_trajs) {

    var__functional_object_t *obj, *dobj;
    var__navigation_t *onav, *dnav;

    obj = objrefr(__local);
    if (!obj) {
        return NULL;
    }

    dnav = NULL;
    dobj = (var__functional_object_t *) malloc(obj->length_);
    if (dobj) {
        var__acquire_lock(obj);
        memcpy(dobj, obj, obj->length_);

        // 记录复制来源
        dobj->dup_ = obj->handle_;

        dnav = var__object_body_ptr(var__navigation_t, dobj);
        dnav->traj_ref_.data_ = NULL;
        dnav->traj_ref_.count_ = 0;

        // traj_ref_ 不适用于直接拷贝
        if (ignore_trajs <= 0) {
            onav = var__object_body_ptr(var__navigation_t, obj);
            if (onav->traj_ref_.data_ && onav->traj_ref_.count_ > 0) {
                dnav->traj_ref_.count_ = onav->traj_ref_.count_;
                dnav->traj_ref_.data_ = (trail_t *) malloc(dnav->traj_ref_.count_ * sizeof ( trail_t));
                if (dnav->traj_ref_.data_) {
                    memcpy(dnav->traj_ref_.data_, onav->traj_ref_.data_, dnav->traj_ref_.count_ * sizeof ( trail_t));
                }
            }
        }

        var__release_lock(obj);

        // 初始化赋值的对象锁
        posix__pthread_mutex_init(&dobj->locker_);

        // 原始对象的链表不需要拷贝
        INIT_LIST_HEAD(&dobj->pagefault_);
        INIT_LIST_HEAD(&dobj->global_);
    }

    objdefr(__local);
    return dnav;
}

int var__commit_navigation_dup(const var__navigation_t *nav) {

    var__functional_object_t *oobj, *obj;
    var__navigation_t *onav;
    char *cpydst;
    const char *cpysrc;

    if (!nav) return -1;

    obj = var__object_head_ptr(nav);
    if (!obj) return -1;

    oobj = objrefr(obj->dup_);
    if (!oobj) return -1;

    onav = var__object_body_ptr(var__navigation_t, oobj);
    var__acquire_lock(oobj);

    cpydst = oobj->body_;
    cpydst += offsetof(var__navigation_t, u_);
    cpysrc = (const char *) nav;
    cpysrc += offsetof(var__navigation_t, u_);

    // 提交数据拷贝
    memcpy(cpydst, cpysrc, sizeof ( struct _i_navigation_internal_t));

    // 提交状态迁移
    var__commit_status_describe_dup(&onav->track_status_, &nav->track_status_);

    var__release_lock(oobj);
    objdefr(obj->dup_);
    return 0;
}

void var__release_navigation_dup(var__navigation_t *nav) {

    var__functional_object_t *dobj;

    if (!nav) return;

    dobj = var__object_head_ptr(nav);
    if (dobj) {
        posix__pthread_mutex_release(&dobj->locker_);
        if (nav->traj_ref_.data_ && nav->traj_ref_.count_ > 0) {
            free(nav->traj_ref_.data_);
        }
        free(dobj);
    }
}

var__navigation_t *var__get_navigation() {
    var__functional_object_t *obj;
    var__navigation_t *nav;

    if (__local > 0) {
        obj = objrefr(__local);
        if (obj) {
            nav = var__object_body_ptr(var__navigation_t, obj);
            var__acquire_lock(obj);
            return nav;
        }
    }

    return NULL;
}

int vari__navigation_loaded_handler(xmlNodePtr element) {
    var__navigation_t *nav;
    var__functional_object_t *object;
    xmlNodePtr node = element;

    if (var__allocate_functional_object(sizeof ( var__navigation_t), kVarType_Navigation, &object) < 0) {
        return -1;
    }
    object->object_id_ = kVarFixedObject_Navigation;
    
    nav = var__object_body_ptr(var__navigation_t, object);
    var__init_status_describe(&nav->track_status_);

    while (node) {
        if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
            covert_first_text_value_s(node, object->name_, cchof(object->name_));
        } else if (0 == xmlStrcmp(BAD_CAST"max_speed", node->name)) {
            nav->max_speed_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"creep_speed", node->name)) {
            nav->creep_speed_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"max_w", node->name)) {
            nav->max_w_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"creep_w", node->name)) {
            nav->creep_w_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"slow_down_speed", node->name)) {
            nav->slow_down_speed_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"acc", node->name)) {
            nav->acc_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"dec", node->name)) {
            nav->dec_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"dec_estop", node->name)) {
            nav->dec_estop_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"acc_w", node->name)) {
            nav->acc_w_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"dec_w", node->name)) {
            nav->dec_w_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"creep_distance", node->name)) {
            nav->creep_distance_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"creep_theta", node->name)) {
            nav->creep_theta_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"upl_mapping_angle_tolerance", node->name)) {
            nav->upl_mapping_angle_tolerance_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"upl_mapping_dist_tolerance", node->name)) {
            nav->upl_mapping_dist_tolerance_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"upl_mapping_angle_weight", node->name)) {
            nav->upl_mapping_angle_weight_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"upl_mapping_dist_weight", node->name)) {
            nav->upl_mapping_dist_weight_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"tracking_error_tolerance_dist", node->name)) {
            nav->tracking_error_tolerance_dist_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"tracking_error_tolerance_angle", node->name)) {
            nav->tracking_error_tolerance_angle_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"aim_dist", node->name)) {
            nav->aim_dist_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"predict_time", node->name)) {
			nav->predict_time_ = ( float ) covert_first_text_value_d( node );
        } else if (0 == xmlStrcmp(BAD_CAST"aim_angle_p", node->name)) {
            nav->aim_angle_p_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"aim_angle_i", node->name)) {
            nav->aim_angle_i_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"aim_angle_d", node->name)) {
            nav->aim_angle_d_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"stop_tolerance", node->name)) {
            nav->stop_tolerance_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"stop_tolerance_angle", node->name)) {
            nav->stop_tolerance_angle_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"stop_point_trim", node->name)) {
            nav->stop_point_trim_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"aim_ey_p", node->name)) {
            nav->aim_ey_p_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"aim_ey_i", node->name)) {
            nav->aim_ey_i_ = covert_first_text_value_d(node);
        } else if (0 == xmlStrcmp(BAD_CAST"aim_ey_d", node->name)) {
            nav->aim_ey_d_ = covert_first_text_value_d(node);
        } else {
            ;
        }

        node = node->next;
    }

    __local = object->handle_;
    var__insert_object(object);
    return 0;
}

int var__load_navigation() {

    if (xml__read_data("var", "navigation.xml", "navigation", vari__navigation_loaded_handler)) {
        return -1;
    }
    return 0;
}