// canbus_driver.cpp : Defines the exported functions for the DLL application.
//
#include "canbus_manager.h"

interface_format(int) canio__create_bus(const _canbus_argv *can, int baud)
{
	return canbus_manager::instance()->canio__create_canbus(can, baud);
}

interface_format(int) canio__create_node(const _canbus_argv * can, const /*struct var__functional_object_t*/void *var, int cnt)
{
	return canbus_manager::instance()->canio__create_node(can, (var__functional_object_t*)var, cnt);
}

// interface_format(int) canio__write(const _canbus_argv * can, const /*struct var__functional_object_t*/void **vars, int cnt)
// {
// 	return canbus_manager::instance()->canio__write(can, (var__functional_object_t**)vars, cnt);
// }
// 
// interface_format(int) canio__read(const _canbus_argv * can, /*struct var__functional_object_t*/void **vars, int cnt)
// {
// 	return canbus_manager::instance()->canio__read(can, (var__functional_object_t**)vars, cnt);
// }

interface_format(int) canio__rw(const _canbus_argv * can, /*struct var__functional_object_t*/void **vars, int cnt)
{
	return canbus_manager::instance()->canio__rw(can, (var__functional_object_t**)vars, cnt);
}

interface_format(int) canio__register_event_callback(void(*driver_call_back)(void *))
{
	return canbus_manager::instance()->canio__register_event_callback(driver_call_back);
}

interface_format(int) canio__set_error_ptr(void *functional_object)
{
    return canbus_manager::instance()->canio__set_error_ptr(functional_object);
}