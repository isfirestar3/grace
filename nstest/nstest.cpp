#include "net_impls.h"
#include "vartypes.h"
#include "navigation.h"
#include "os_util.hpp"
#include "log.h"
#include "icom/compiler.h"
#include "vehicle.h"

#include "icom/posix_thread.h"
#include "icom/posix_wait.h"

#include <stdint.h>
#include <thread>
#include<ctime>

void mn_notify( uint32_t robot_id, const void *data, enum mn::notift_type_t type) {

}

void wproc(int robot) {
	struct mn::common_data_item item;
	item.varid = kVarFixedObject_Navigation;
	item.offset = offsetof(var__navigation_t, pos_);

	position_t pos;
	pos.x_ = 0.0;
	pos.y_ = 0.0;
	pos.w_ = 0.0;
	item.data += (char *)&pos;

	uint64_t timestamp = nsp::os::clock_gettime();
	item.data += (char *)&timestamp;

	uint32_t pos_status = 0;
	item.data += (char *)&pos_status;

	double confidence = 0.0;
	item.data += (char *)&confidence;

	mn::common_data data;
	data.items.push_back(item);

	uint64_t interval, o_tick, r_tick;
	nsp::os::waitable_handle w(0);
	while (1) {
		o_tick = nsp::os::clock_gettime();
		if (post_common_write_request_by_id(robot, data, [&]( uint32_t, const void * ){
			r_tick = nsp::os::clock_gettime();
			w.sig();
		} ) < 0 ) {
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			continue;
		}

		w.wait(-1);
		w.reset();

		// cover to milliseconds
		interval = r_tick - o_tick;
		interval /= 10000;
		if (interval < 20) {
			std::this_thread::sleep_for(std::chrono::milliseconds(20 - interval));
		} else{
			loerror("nstest") << "send timeout." << interval;
		}
	}
}

double random_double(){
	return rand() / double(RAND_MAX);
}

void random_nav_data(var__navigation_t *nav_p ){
	if (nav_p){
		nav_p->max_speed_ = random_double();
		nav_p->creep_speed_ = random_double();
		nav_p->max_w_ = random_double();
		nav_p->creep_w_ = random_double();
		nav_p->slow_down_speed_ = random_double();
		nav_p->acc_ = random_double();
		nav_p->dec_ = random_double();
		nav_p->dec_estop_ = random_double();
		nav_p->acc_w_ = random_double();
		nav_p->dec_w_ = random_double();
		nav_p->creep_distance_ = random_double();
		nav_p->creep_theta_ = random_double();
		nav_p->upl_mapping_angle_tolerance_ = random_double();
	}
}

void random_vehice(var__vehicle_t *veh_p){
	if (veh_p){

	}
}

void * init_data(int type, int &length){
	void * pbuffer = nullptr;
	try{
		if (1== type){
			pbuffer = (void *)new var__navigation_t;
			length = sizeof(var__navigation_t);
		}
		else{
			pbuffer = (void *)new var__vehicle_t;
			length = sizeof(var__vehicle_t);
		}
	}
	catch(...){
		loerror("nstest") << "new falure";
		return nullptr;
	}
	return pbuffer;
}

void write_data(int robot, nsp::os::waitable_handle & w, int type) {
	uint64_t interval, o_tick, r_tick;
	int length = 0;
	void * pbuffer = nullptr;
	mn::common_data data;
	mn::common_data_item item;
	int err = 0;
	item.offset = 0;

	if (1 == type){
		item.varid = kVarFixedObject_Navigation;
	}
	else{
		item.varid = kVarFixedObject_Vehide;
	}
	
	if (!(pbuffer =init_data(type, length))){
		return ;
	}

	int error = 0;
	while (true) {
		if (!error){
			data.items.clear();
			if (1 == type){
				random_nav_data((var__navigation_t *)pbuffer);
			}
			else{
				random_vehice((var__vehicle_t *)pbuffer);
			}
			item.data.assign((char *)pbuffer, length);
			data.items.push_back(item);
		}

		o_tick = nsp::os::clock_gettime();
		int retval = mn::post_common_write_request_by_id(robot, data, [&](uint32_t, const void *p){
			mn::asio_t *asio = (mn::asio_t *)p;
			err = asio->err_;
			w.sig();
		});

		r_tick = nsp::os::clock_gettime();
		interval = r_tick - o_tick;
		error = 1;
		if (retval < 0) {
			printf("failed interface call.\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			continue;
		}

		w.wait(-1);
		w.reset();

		if (err < 0) {
			printf("failed acknowledge.\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			continue;
		}
		
		printf("%ld %ld %ld\n", r_tick, o_tick, interval);
		error = 0;
		// cover to milliseconds
		interval /= 10000;
		if (interval < 20) {
			std::this_thread::sleep_for(std::chrono::milliseconds(20 - interval));
		}
		else{
			loerror("nstest") << "receiver timeout." << interval;
		}
	}
}

int main(int argc, char **argv) {

	printf("%d\n", sizeof(posix__pthread_mutex_t));

	int order_kill = 0;
	int order_lsvar = 0;
	char *target_epstr;
	nsp::os::waitable_handle w(0);

	if (argc < 2) {
		printf("usage: nstest [endpoint] [command]");
		return 1;
	}
	target_epstr = argv[1];

	if (argc > 2) {
		// using protocol to kill MT process
		if (0 == strcmp(argv[2], "kill")) {
			order_kill = 1;
		}else if (0 == strcmp(argv[2], "lsvar")) {
			order_lsvar = 1;
		}else{
			;
		}
	}

	int robot = mn::init_net();
	if (robot < 0) {
		return 1;
	}

	int retval = mn::login_to_host(robot, target_epstr, kControlorType_Dispatcher);
	if (retval < 0) {
		std::this_thread::sleep_for(std::chrono::seconds(2));
		return 1;
	}

	// std::thread wth(std::bind(&wproc, robot));
	mn::register_callback_to_notify(robot, &mn_notify);

	// using protocol to kill MT process
	if (order_kill) {
		mn::post_dbg_segmentfault(robot, [&]( uint32_t, const void *p ){} );
		return 0;
	}

	if (order_lsvar) {
		mn::post_dbg_varls_request(robot, [&]( uint32_t, const void *p ){
				mn::asio_t *asio = (mn::asio_t *)p;
				retval = asio->err_;
				if (asio->err_ >= 0) {
					mn::var_list *lsvar = (mn::var_list *)p;
					for (auto &iter : lsvar->items_) {
						printf("var id:%d var type:0x%04X\n", iter.id_, iter.type_);
					}
				}
				w.sig();
			});
		w.wait();
		return 0;
	}

#ifdef comon_read
	struct mn::common_title_item item;
	item.varid = kVarFixedObject_Navigation;
	item.offset = 0;
	item.length = sizeof(var__navigation_t);

	struct mn::common_title title;
	title.items.push_back(item);
	uint64_t interval, o_tick, r_tick;
	while (1) {
		o_tick = nsp::os::clock_gettime();
		retval = mn::post_common_read_request_by_id(robot, title, [&]( uint32_t, const void *p ){
			mn::asio_t *asio = (mn::asio_t *)p;
			retval = asio->err_;
			w.sig();
		});

		r_tick = nsp::os::clock_gettime();
		interval = r_tick - o_tick;

		if (retval < 0) {
			printf("failed interface call.\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			continue;
		}

		w.wait(-1);
		w.reset();

		if (retval < 0) {
			printf("failed acknowledge.\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			continue;
		}

		printf("%ld %ld %ld\n", r_tick, o_tick, interval);

		// cover to milliseconds
		interval /= 10000;
		if (interval < 20) {
			std::this_thread::sleep_for(std::chrono::milliseconds(20 - interval));
		} else{
			loerror("nstest") << "receiver timeout." << interval;
		}	
	}
#else
	srand((unsigned)time(NULL));
	write_data(robot, w,1);
#endif
	return 0;
}
