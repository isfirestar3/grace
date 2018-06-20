#ifndef __CANBUS_MANAGER_H__
#define __CANBUS_MANAGER_H__
#include "elmo.h"
#include <map>
#include "canio.h"
#include "var.h"
#include "canbus_base.h"
#include "driver_base.h"
#include <mutex>
#include <memory>
class canbus_manager
{
public:
	canbus_manager();
	~canbus_manager();

	static canbus_manager* instance();

	int canio__create_canbus(const _canbus_argv *can, int baud);
	int canio__create_node(const _canbus_argv * can, var__functional_object_t *var, int cnt);
	//int canio__write(const _canbus_argv * can, var__functional_object_t **vars, int cnt);
	//int canio__read(const _canbus_argv * can, var__functional_object_t **vars, int cnt);
	int canio__rw(const _canbus_argv * can, var__functional_object_t **vars, int cnt);
        int canio__register_event_callback(void(*driver_call_back)(void *));
private:
	std::map<int, std::shared_ptr<canbus_base>> map_canbus_;
	std::map<int, std::map<int, std::shared_ptr<driver_base>>> map_dirver_;
	std::mutex mtx_bus_;
	std::mutex mtx_driver_;
        void(*driver_call_back_)(void *);
};


#endif

