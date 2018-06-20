
#include "canbus_manager.h"
#include "object_switcher.hpp"

canbus_manager::canbus_manager() {

}

canbus_manager::~canbus_manager() {
}

canbus_manager* canbus_manager::instance() {
	static canbus_manager g_obj;
	return &g_obj;
}

int canbus_manager::canio__create_canbus( const _canbus_argv *can, int baud ) {
	if (nullptr == can)
	{
		return -1;
	}
	int key = (( (short)can->type_ & 0xFFFF ) << 16) + can->port_;
	std::lock_guard<decltype( mtx_bus_ )> guard( mtx_bus_ );

	std::map<int, std::shared_ptr<canbus_base>>::iterator it = map_canbus_.find( key );
	if ( it == map_canbus_.end() ) {
		std::shared_ptr<canbus_base> canbus = ::get_canbus_ptr( can, baud );
		if ( canbus == nullptr ) {
			return -1;
		}
		int e = canbus->open_can_interface();
		if ( e ) {
			return -1;
		}
		canbus->open_canopen();
		map_canbus_[key] = canbus;
		map_dirver_[key];
	}

	return 0;
}

int canbus_manager::canio__create_node( const _canbus_argv * can, var__functional_object_t *var, int cnt ) {
	if (nullptr == can || nullptr == var || cnt==0)
	{
		return -1;
	}
	int key = (((short)can->type_ & 0xFFFF) << 16) + can->port_;

	std::shared_ptr<canbus_base> bus = nullptr;

	{
		std::lock_guard<decltype( mtx_bus_ )> guard( mtx_bus_ );
		std::map<int, std::shared_ptr<canbus_base>>::iterator it = map_canbus_.find( key );
		if ( it != map_canbus_.end() ) {
			bus = it->second;
		}
	}

	if ( bus == nullptr ) {
		return -1;
	}

	for ( int i = 0; i < cnt; ++i ) {
		std::lock_guard<decltype( mtx_driver_ )> guard( mtx_driver_ );
		std::map<int, std::map<int, std::shared_ptr<driver_base>>>::iterator itr_bus = map_dirver_.find( key );
		if ( itr_bus == map_dirver_.end() ) {
			return -1;
		} else {
			int obj_id = var[i].object_id_;
			std::shared_ptr<driver_base> drv = ::get_driver_ptr( &var[i] );
			if ( drv == nullptr ) {
				return -1;
			}
			std::map<int, std::shared_ptr<driver_base>>::iterator itr_drv = itr_bus->second.find( obj_id );
			if ( itr_drv == itr_bus->second.end() ) {
				CML::CanOpen* canopen;
				int e = bus->get_canopen( canopen );
				if ( e ) {
					return -1;
				}
				e = drv->add_node( canopen, &var[i] );
				if ( e ) {
					return -1;
				}
				drv->set_driver_callback(driver_call_back_);
                drv->canio__set_error_ptr((var__functional_object_t *)functional_object_);
				itr_bus->second[obj_id] = drv;
			}
		}
	}
	return 0;
}

int canbus_manager::canio__rw(const _canbus_argv * can, var__functional_object_t **vars, int cnt) {
	if (nullptr == can || nullptr == vars || cnt == 0)
	{
		return -1;
	}
	int key = (((short)can->type_ & 0xFFFF) << 16) + can->port_;

	std::lock_guard<decltype(mtx_driver_)> guard(mtx_driver_);
	for (int i = 0; i < cnt; ++i) {
		std::map<int, std::map<int, std::shared_ptr<driver_base>>>::iterator itr_bus = map_dirver_.find(key);
		if (itr_bus == map_dirver_.end()) {
			return -1;
		}
		else {
			int obj_id = vars[i]->object_id_;
			std::map<int, std::shared_ptr<driver_base>>::iterator itr_drv = itr_bus->second.find(obj_id);
			if (itr_drv == itr_bus->second.end()) {
				return -1;
			}
			else {
                itr_drv->second->rw(vars[i]);
			}
		}
	}

	return 0;
}

int canbus_manager::canio__register_event_callback(void(*driver_call_back)(void *))
{
	driver_call_back_ = driver_call_back;
	return 0;
}

int canbus_manager::canio__set_error_ptr(void *functional_object)
{
    functional_object_ = functional_object;
    return 0;
}
