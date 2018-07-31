#include "net_impls.h"
#include "os_util.hpp"
#include "navigation.h"

EXP( int ) py_init_net(){
	return mn::init_net();
}
	
EXP( int ) py_login_to_host( uint32_t robot_id, const char *epstr, nsp__controlor_type_t type ) {
	return mn::login_to_host(robot_id, epstr, type);
}

EXP( int ) py_read_navigation_sync( uint32_t robot_id, var__navigation_t *nav) {
	struct mn::common_title_item item;
	item.varid = kVarFixedObject_Navigation;
	item.offset = 0;
	item.length = sizeof(var__navigation_t);

	struct mn::common_title title;
	title.items.push_back(item);

	nsp::os::waitable_handle w(0);
	int retval = mn::post_common_read_request_by_id(robot_id, title, [&]( uint32_t, const void *p ){
		mn::asio_t *asio = (mn::asio_t *)p;
		retval = asio->err_;
		if (retval >= 0) {
			struct mn::common_data *data = (struct  mn::common_data *)p;
			if (data->items.size() == 1) {
				struct mn::common_data_item *item = &data->items[0];

				uint32_t cpcb = sizeof(var__navigation_t);
				if (item->data.size() < cpcb) {
					cpcb = item->data.size();
				}
				//printf("cpcb=%u\n", cpcb);
				memcpy(nav, item->data.data(), cpcb);
				// printf("%p %d %d %d %ld %ld\n", nav,
				// 	nav->track_status_.command_, nav->track_status_.middle_, nav->track_status_.response_,
				// 	nav->user_task_id_, nav->ato_task_id_);
				printf("%ld\n", nav->pos_time_stamp_);
			}
		}
		w.sig();
	});

	if (retval < 0) {
		return -1;
	}

	w.wait(-1);
	return 0;
}

EXP( void ) py_test_navigation_data ( var__navigation_t *nav) {
	printf("%p %d %d %d %ld %ld\n", nav,
					nav->track_status_.command_, nav->track_status_.middle_, nav->track_status_.response_,
					nav->user_task_id_, nav->ato_task_id_);
}

#pragma pack(push, 1)

struct test {
	int a;
	int b;
};

#pragma pack(pop)

EXP( void ) py_test_pointer_array(struct test *pt, int count) {
	for (int i = 0; i < count; i++) {
		printf("%d %d\n", pt[i].a, pt[i].b);
	}
}

EXP( void ) py_test_pointer_stream(int(*pfunc)(const unsigned char *, int, int *)) {
	unsigned char ptr[10];
	ptr[0] = 'A';
	ptr[1] = 'B';
	ptr[2] = 'C';
	ptr[3] = 0;
	ptr[4] = '1';
	ptr[5] = '2';
	ptr[6] = '3';
	ptr[7] = 7;
	ptr[8] = 8;
	ptr[9] = 9;

	int n;
	int w = pfunc(ptr, sizeof(ptr), &n);
	printf("%d %d\n", n, w);
}