// canbus_driver.cpp : Defines the exported functions for the DLL application.
//

#include "canbus_manager.h"
#include "../var/error.h"

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

interface_format( int )		canio__register_event_callback( void( *functional_object)( void * ) ) 
{
	return canbus_manager::instance()->canio__register_event_callback(functional_object);
}
interface_format(int) canio__set_error_ptr(void *functional_object)
{
	//var__functional_object_t *object = ( var__functional_object_t * ) functional_object;
	//var__error_handler_t *err = var__object_body_ptr( var__error_handler_t, object );
	//err->error_[10].hardware_ = 100;
    return 0;
}