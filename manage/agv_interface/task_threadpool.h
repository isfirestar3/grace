#pragma once

#include "agv_base.hpp"
#include "singleton.hpp"
#include "task_scheduler.hpp"

class task_threadpool
{
public:
	task_threadpool();
	~task_threadpool();

	friend class nsp::toolkit::singleton<task_threadpool>;

	nsp::toolkit::task_thread_pool<agv_tp_task_base> g_task_threadpool;
};

