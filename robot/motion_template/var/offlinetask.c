#include "offlinetask.h"
#include "var.h"
#include "proto.h"
#include "logger.h"

//free heap memory when next incoming offline task  arrived
//notify customer / rex navigation completed
//decline post_navigation_task(not customer) / post_add_navigation_task_traj / post_allocate_operation_task when offline task is existed
//decline offline task when either navigation task or operation task is existed

static
objhld_t __local = -1;

var_offline_task_t *var__get_offline_task() {
	var__functional_object_t *obj;
	var_offline_task_t *task;

	if (__local > 0) {
		obj = objrefr(__local);
		if (obj) {
			task = var__object_body_ptr(var_offline_task_t, obj);
			var__acquire_lock(obj);
			return task;
		}
	}

	return NULL;
}

int var__set_offline_task(const char *data, int cb) {
	var_offline_task_t *task_new = NULL;
	var_offline_task_t *task_old = NULL;
	var__functional_object_t *object_new = NULL;
	var__functional_object_t *object_old = NULL;
	nsp__allocate_offline_task_t *src = (nsp__allocate_offline_task_t *)data;

	if (__local > 0) {
		task_old = var__get_offline_task();
		if (task_old->track_status_.command_ != kStatusDescribe_Idle) {
			var__release_object_reference(task_old);
			return -EBUSY;
		}
	}

	int len = cb;
	if (cb < sizeof(nsp__allocate_offline_task_t)) {
		log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
			"invalid request size for nsp__allocate_offline_task_t: %d", cb);
		return -EINVAL;
	}
	if (var__allocate_functional_object(len, kVarType_OfflineTask, &object_new) < 0) {
		return -1;
	}
	object_new->object_id_ = kVarFixedObject_OfflineTask;
	task_new = var__object_body_ptr(var_offline_task_t, object_new);
	var__init_status_describe(&task_new->track_status_);
	if (task_old) {
		task_new->ato_task_id_ = src->task_id_;
		// 从全局的对象管理列表中删除 
		var__delete_functional_object(object_old);
		return 0;
	}

	// 插入本地管理列表 
	__local = object_new->handle_;
	// 插入全局的对象管理列表 
	var__insert_object(object_new);
	return 0;
}

