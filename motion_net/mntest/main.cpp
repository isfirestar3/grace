/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: anderson
 *
 * Created on September 27, 2017, 8:45 PM
 */

#include <cstdlib>
#include <iostream>
#include <thread>

#include "net_impls.h"

#include "os_util.hpp"

#include "vehicle.h"
#include "navigation.h"

using namespace std;

bool enable = true;

void network_event_callback( uint32_t robot_id, const void *data, int type ) {
	switch ( type ) {
		case mn::kNET_STATUS:
			{
				mn::net_status_t *stat = ( mn::net_status_t * )data;
				if ( *stat == mn::kNetworkStatus_Closed ) {
					enable = false;
				}
			}
			break;
		case mn::kPERIODIC_REPORT:
			{
				mn::periodic_data *pers = ( mn::periodic_data * )data;
				if ( pers->items.size() > 0 ) {
					var__navigation_t *nav = ( var__navigation_t * ) pers->items[0].data.data();
					std::cout << nav->pos_time_stamp_ << std::endl;
				}

				mn::unregistry_periodic_report(robot_id,  [] ( uint32_t, const void * ) {} );
			}
			break;
		default:
			break;
	}
}

int robot_id;

void read_test_callback( uint32_t, const void *data ) {
	mn::asio_t *asio = ( mn::asio_t * )data;
	if ( !asio ) {
		return;
	}

	if ( asio->err_ != 0 ) {
		std::cout << "asio error:" << asio->err_ << std::endl;
		return;
	}

	mn::common_data *ack = ( mn::common_data * )data;
	if ( 0 == ack->items.size() ) {
		return;
	}

	var__vehicle_t *veh = ( var__vehicle_t * ) ack->items[0].data.data();
	std::cout << "[" << nsp::os::gettid() << "]" << veh->i.time_stamp_ << std::endl;
}

void test_read( int robot_id ) {
	mn::common_title_item title_item;
	title_item.varid = kVarFixedObject_Vehide;
	title_item.offset = 0;
	title_item.length = sizeof( var__vehicle_t );

	mn::common_title title;
	title.items.push_back( title_item );

	while ( 1 ) {
		int retval = mn::post_common_read_request_by_id( robot_id, title, &read_test_callback );
		//std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	}
}

void test_periodic_get() {
	struct mn::periodic_title_item item;
	item.varid = kVarFixedObject_Navigation;
	item.offset = 0;
	item.length = sizeof( var__vehicle_t );
	item.interval = 20;

	struct mn::periodic_title title;
	title.items.push_back( item );

	registry_periodic_report( robot_id, title, [] ( uint32_t, const void *data ) {} );
}

extern void locOutput( int robot_id );

int main( int argc, char** argv ) {
	robot_id = mn::init_net();
	if ( robot_id < 0 ) {
		return 1;
	}

	mn::register_callback_to_notify( robot_id, &network_event_callback );
	if ( mn::login_to_host( robot_id, "127.0.0.1:4409", kControlorType_Dispatcher ) < 0 ) {
		return 1;
	}

	mn::post_query_mtver(robot_id, [&](uint32_t, const void *data) {
		mn::mtver_t *mtver = (mn::mtver_t *)data;
		if (mtver->err_ < 0) {
			std::cout << "failed get mtver." << std::endl;
			return;
		}

		std::cout << "v" << mtver->major << "." << mtver->sub << std::endl;
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	//locOutput( robot_id );
	while (1) {
		test_read(robot_id);
		usleep(10 * 1000);
	}
	//test_periodic_get();

	nsp::os::pshang();
	return 0;
}

