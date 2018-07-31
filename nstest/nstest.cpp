#include "net_impls.h"
#include "vartypes.h"
#include "navigation.h"
#include "os_util.hpp"
#include "log.h"
#include "icom/compiler.h"

#include "icom/posix_thread.h"
#include "icom/posix_wait.h"

#include <stdint.h>
#include <thread>

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

int proc() {
	for (int i = 0; i < 50; i++) {
		int robot = mn::init_net();
		if (robot < 0) {
			loerror("nstest") << "failed init_net.";
			return 1;
		}

		int retval = mn::login_to_host(robot, "127.0.0.1:4409", kControlorType_Localization);
		if (retval < 0) {
			loerror("nstest") << robot << " connect failed.";
			std::this_thread::sleep_for(std::chrono::seconds(2));
		}
	}
	return 0;
}

int main(int argc, char **argv) {
	
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

	// for (int i = 0; i < 2000; i++ ) {
	// 	int robot = mn::init_net();
	// 	if (robot < 0) {
	// 		printf("init failed. %d\n", i);
	// 		break;
	// 	}

	// 	int retval = mn::login_to_host(robot, target_epstr, kControlorType_Localization);
	// 	if (retval < 0) {
	// 		std::this_thread::sleep_for(std::chrono::seconds(2));
	// 		printf("login_to_host failed. %d\n", i);
	// 		continue;
	// 	}
	// }

	// nsp::os::pshang();

	int robot = mn::init_net();
	if (robot < 0) {
		return 1;
	}

	int retval = mn::login_to_host(robot, target_epstr, kControlorType_Calibration);
	// int retval = mn::login_to_host(robot, target_epstr, kControlorType_Localization);
	if (retval < 0) {
		std::this_thread::sleep_for(std::chrono::seconds(2));
		return 1;
	}

	unsigned char data[128];
	memset(data, 0xAA, 128);
	mn::post_localization_cfgwrite_request(robot, data, 0, 128, [&](uint32_t, const void *p) {
		w.sig();
	});
	w.wait();
	w.reset();

	mn::post_localization_cfgread_request(robot, [&](uint32_t, const void *p){
		mn::asio_t *asio = (mn::asio_t *)p;
		if (asio->err_ >= 0) {
			mn::loc_data_t *locdat = (mn::loc_data_t *)p;
			for (int i = 0; i < 128; i++) {
				printf("0x%02X ", locdat->data_[i]);
				if (i > 0 && i % 8 == 0 && i % 16 != 0) {
					printf("\t");
				}else{
					if (i % 16 == 0 && i > 0) {
						printf("\n");
					}
				}
			}
		}

		w.sig();
	});
	w.wait();
	return 0;

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

	struct mn::common_title_item item;
	item.varid = kVarFixedObject_Navigation;
	item.vartype = 2;
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
	return 0;
}
