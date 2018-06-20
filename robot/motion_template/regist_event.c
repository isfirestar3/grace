#include "regist_event.h"
#include "clist.h"
#include "entry.h"
#include "logger.h"
#include "nsp.h"
#include "posix_atomic.h"
#include "posix_time.h"
#include "proto.h"
#include "var.h"
#include "vartypes.h"

//////////////////////////////////////// 内部结构 ////////////////////////////////////////////////////////////
typedef struct {
	struct list_head index_;
	int var_id_;					//订阅事件 var_id_ 
	int offset_;					//数据偏移 
	int length_;					//订阅数据长度 
	uint32_t event_cycle_;			//周期 
	uint32_t remaind_time_;			//剩余时间 
} st_regist_event_item;

typedef struct {
	struct list_head index_;
	HTCPLINK link_;
	nsp__packet_head_t head_;		//最近包包头  
	struct list_head list_item_;	//订阅的事件列表 st_regist_event_item 
} st_client_regist_event;

typedef struct {
	uint32_t enable_;				//是否被订阅标识 atomic 
	int var_id_;
	int var_type_;
	int body_length_;
	char body_[0];
} st_var_obj_info;

typedef struct {
	int total_length_;				//var 对象个数 
	char item[0];					//st_var_obj_info 
} all_var_obj;

typedef struct {
	struct list_head index_;
	int var_id_;
	int offset_;
	int length_;
} st_reached_event;

//////////////////////////////////////// 全局数据 ////////////////////////////////////////////////////////////

static 
struct list_head g_client_event;	//客户端订阅事件集合  st_client_regist_event 
static
posix__pthread_mutex_t client_mutex;

static 
all_var_obj *g_all_vars;			//全量对象集合
static
posix__pthread_mutex_t vars_mutex;

static
posix__pthread_t copy_timer;
static
posix__pthread_t rce_timer;


//////////////////////////////////////// implement ////////////////////////////////////////////////////////////
int nspi__interval_control(posix__waitable_handle_t *waiter, uint64_t begin_tick, uint32_t maximum_delay) {
    uint32_t tsc;
    int interval = (int) (posix__gettick() - begin_tick);
    if (interval < 0) {
        return interval;
    }

    if (interval >= (int) maximum_delay) {
        return interval;
    }

    tsc = (uint32_t) (maximum_delay - interval);
    if (waiter) {
        posix__waitfor_waitable_handle(waiter, tsc);
    }

    return tsc;
}

int nspi__check_param(int id, int len) {
	int i = 0, add_len;
	st_var_obj_info* cur_obj = (st_var_obj_info*)(g_all_vars->item);
	for ( ; i < g_all_vars->total_length_; ++i) {
		if (id == cur_obj->var_id_ && len <= cur_obj->body_length_) {
			return 1;
		}
		add_len = sizeof (st_var_obj_info) + cur_obj->body_length_;
		cur_obj = (st_var_obj_info*)((char*)cur_obj + add_len);
	}
	
	return 0;
}

int nspi__regist_to_vars(const int id) {
	int i = 0, add_len;
	st_var_obj_info* cur_obj = (st_var_obj_info*)(g_all_vars->item);
	for ( ; i < g_all_vars->total_length_; ++i) {
		if (id == cur_obj->var_id_ ) {
			posix__atomic_inc( &cur_obj->enable_ );
			return 1;
		}
		add_len = sizeof (st_var_obj_info) + cur_obj->body_length_;
		cur_obj = (st_var_obj_info*)((char*)cur_obj + add_len);
	}
	
	return 0;
}

int nspi__cancle_from_vars(const int id) {
	int i = 0, add_len;
	st_var_obj_info* cur_obj = (st_var_obj_info*)(g_all_vars->item);
	for ( ; i < g_all_vars->total_length_; ++i) {
		if (id == cur_obj->var_id_ ) {
			posix__atomic_dec( &cur_obj->enable_ );
			return 1;
		}
		add_len = sizeof (st_var_obj_info) + cur_obj->body_length_;
		cur_obj = (st_var_obj_info*)((char*)cur_obj + add_len);
	}
	
	return 0;
}

int nspi__regist_cycle_event(HTCPLINK link, const char *data, int cb) {
	nsp__initiactive_protocol_t *pkt_initiactive = (nsp__initiactive_protocol_t *) data;
	int i = 0, retval;
	
	//创建返回包 
	nsp__packet_head_t ack_initiactive;
	memcpy(&ack_initiactive, &pkt_initiactive->head_, sizeof(nsp__packet_head_t));
	ack_initiactive.size_ = sizeof(nsp__packet_head_t);
	ack_initiactive.type_ = PKTTYPE_INITIACTIVE_COMMON_READ_ACK;
	
	if (!g_all_vars) {
		ack_initiactive.err_ = -1;
		retval = tcp_write(link, ack_initiactive.size_, &nsp__packet_maker, &ack_initiactive);
		return retval;
	}
	
	nsp__initiactive_protocol_item_t *items = (nsp__initiactive_protocol_item_t *) pkt_initiactive->items;
	for ( ; i < pkt_initiactive->count_; ++i) {
		log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, 
					"regist event, varid[%d], offset[%d], length[%d], cylce=[%d], vartype=[%d].",
					items[i].var_id_, items[i].offset_, items[i].length_, items[i].event_cycle_, items[i].vartype_);
		//校验var_id, offset 和length 合法性 
		if (nspi__check_param(items[i].var_id_, (items[i].offset_ + items[i].length_)) <= 0 || items[i].event_cycle_ <= 10) {
			//校验失败，返回错误包
			log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, 
						"regist event, varid[%d], offset[%d], length[%d], cylce=[%d], vartype=[%d].",
						items[i].var_id_, items[i].offset_, items[i].length_, items[i].event_cycle_, items[i].vartype_);
			ack_initiactive.err_ = 0 - items[i].var_id_;
			retval = tcp_write(link, ack_initiactive.size_, &nsp__packet_maker, &ack_initiactive);
			return retval;
		}
	}
	
	struct list_head *loop;
	struct list_head *item_loop;
	int exist_client = 0, exit_event = 0;
	st_client_regist_event* client_item;
	st_regist_event_item* event_item;
	
	list_for_each(loop, &g_client_event) {
		client_item = containing_record(loop, st_client_regist_event, index_);
		if (link != client_item->link_) continue;
		
		exist_client = 1;
		memcpy(&client_item->head_, &pkt_initiactive->head_, sizeof ( nsp__packet_head_t));
		
		for (i = 0; i < pkt_initiactive->count_; ++i) {
			exit_event = 0;
			list_for_each(item_loop, &client_item->list_item_) {
				event_item = containing_record(item_loop, st_regist_event_item, index_);
				if (event_item->var_id_ == items[i].var_id_) {
					//change exist one
					event_item->offset_ = items[i].offset_;
					event_item->length_ = items[i].length_;
					event_item->event_cycle_ = items[i].event_cycle_;
					event_item->remaind_time_ = items[i].event_cycle_;
					exit_event = 1;
					break;
				}
			}
			
			if(!exit_event) {
				//add new event
				st_regist_event_item* new_event = (st_regist_event_item*) malloc ( sizeof(st_regist_event_item) );
				if (new_event) {
					new_event->var_id_ = items[i].var_id_;
					new_event->offset_ = items[i].offset_;
					new_event->length_ = items[i].length_;
					new_event->event_cycle_ = items[i].event_cycle_;
					new_event->remaind_time_ = items[i].event_cycle_;
					 
					posix__pthread_mutex_lock(&client_mutex);
					list_add_tail(&new_event->index_, &client_item->list_item_);
					posix__pthread_mutex_unlock(&client_mutex);
					
					if (nspi__regist_to_vars(new_event->var_id_ ) <= 0) {
						log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
							"add new var error, var id[%d] doesn't exist in vars.", new_event->var_id_);
					}
				}
			}
		}
		
		break;
	}
	
	if (!exist_client) {
		//add new client 
		st_client_regist_event* new_client = (st_client_regist_event*) malloc ( sizeof(st_client_regist_event) );
		if (!new_client) {
			log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "malloc new client failed.");
			ack_initiactive.err_ = -1;
			retval = tcp_write(link, ack_initiactive.size_, &nsp__packet_maker, &ack_initiactive);
			return retval;
		}
		
		new_client->link_ = link;
		memcpy(&new_client->head_, &pkt_initiactive->head_, sizeof ( nsp__packet_head_t));
		INIT_LIST_HEAD(&new_client->list_item_);
		
		for (i = 0; i < pkt_initiactive->count_; ++i) {
			//add new event
			st_regist_event_item* new_event = (st_regist_event_item*) malloc ( sizeof(st_regist_event_item) );
			new_event->var_id_ = items[i].var_id_;
			new_event->offset_ = items[i].offset_;
			new_event->length_ = items[i].length_;
			new_event->event_cycle_ = items[i].event_cycle_;
			new_event->remaind_time_ = items[i].event_cycle_;
			if (nspi__regist_to_vars(new_event->var_id_ ) <= 0) {
				log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
					"add new var error, var id[%d] doesn't exist in vars.", new_event->var_id_);
			}
			
			INIT_LIST_HEAD(&new_event->index_);
			list_add_tail(&new_event->index_, &new_client->list_item_);
		}
		
		posix__pthread_mutex_lock(&client_mutex);
		list_add_tail(&new_client->index_, &g_client_event);
		posix__pthread_mutex_unlock(&client_mutex);
	}
	
	log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
		"client link[%d] regist event successful.", link);
	
	retval = tcp_write(link, ack_initiactive.size_, &nsp__packet_maker, &ack_initiactive);
	
	return retval;
}

int nspi__unregist_cycle_event(HTCPLINK link, const char *data, int cb) {
	int retval = -1;
	if (!g_all_vars) {
		return retval;
	}
	
	struct list_head *loop;
	struct list_head *item_loop;
	struct list_head *cursor, *cursor2;
	st_client_regist_event* client_item;
	st_regist_event_item* event_item;
	
	list_for_each_safe(loop, cursor, &g_client_event) {
		client_item = containing_record(loop, st_client_regist_event, index_);
		if (link != client_item->link_) continue;
		
		posix__pthread_mutex_lock(&client_mutex);
		list_for_each_safe(item_loop, cursor2, &client_item->list_item_) {
			event_item = containing_record(item_loop, st_regist_event_item, index_);
			if(!event_item) break;
			
			log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
					"unregist varid[%d].", event_item->var_id_);
			if (nspi__cancle_from_vars(event_item->var_id_ ) <= 0) {
				log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
					"cancled var error, var id[%d] doesn't exist in vars.", event_item->var_id_);
			}
			
			if(!list_empty_careful(item_loop)) list_del(item_loop);
			INIT_LIST_HEAD(item_loop);
			if(event_item) free(event_item);
			event_item = NULL;
		}
		
		list_del(loop);
        INIT_LIST_HEAD(loop);
		if (client_item) free(client_item);
		posix__pthread_mutex_unlock(&client_mutex);
		
		retval = 0;
		break;
	}
	
	if(data) {
		nsp__packet_head_t *unregist_pkg = (nsp__packet_head_t *) data;
		
		//构建返回包 
		nsp__packet_head_t *ack_initiactive = (nsp__packet_head_t*) malloc ( sizeof(nsp__packet_head_t) );
		if (!ack_initiactive) {
			log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
	                "unregist cycle event create ack package failed.");
	        return -1;
	    }
		memcpy(ack_initiactive, unregist_pkg, sizeof ( nsp__packet_head_t));
		ack_initiactive->type_ = PKTTYPE_INITIACTIVE_COMMON_READ_CANCLE_ACK;
		
		retval = tcp_write(link, ack_initiactive->size_, &nsp__packet_maker, ack_initiactive);
		free(ack_initiactive);		
	}
	
	log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
		"client link[0x%08X] unregist event successful.", link);
	
	return retval;
}


//线程循环
void *nspi__copy_vars_proc(void* argv ) {
	posix__waitable_handle_t copy_waiter;
	posix__init_synchronous_waitable_handle(&copy_waiter);
	static const uint32_t copy_interval = 20;
	static uint64_t begin_tick;
	st_var_obj_info* loop_var;
	objhld_t hld;
	var__functional_object_t *obj;
	char* p_var;
	int i=0;
	int off_len;
	
	while ( 1 ) {
		begin_tick = posix__gettick();
		
		//loop 周期性事件集合 
		loop_var = (st_var_obj_info*)(g_all_vars->item);
		for (i = 0; i < g_all_vars->total_length_; ++i) {
			if(loop_var->enable_ > 0) {
				//取出事件数据by var_id_ 
				hld = var__getobj_handle_byid(loop_var->var_id_ );
				obj = (var__functional_object_t *) objrefr(hld);
				p_var = (char *) obj->body_;
				posix__pthread_mutex_lock(&vars_mutex); 
				
				var__acquire_lock(obj);
				memcpy(loop_var->body_, p_var, obj->body_length_);
				var__release_lock(obj);
				
				posix__pthread_mutex_unlock(&vars_mutex);
				p_var = NULL;
			}
			off_len = sizeof (st_var_obj_info) + loop_var->body_length_;
			loop_var = (st_var_obj_info*)((char*)loop_var + off_len);
		}
		
		nspi__interval_control(&copy_waiter, begin_tick, copy_interval);
	}
	
	log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem | kLogTarget_Stdout, "copy vars loop exit.");
	posix__uninit_waitable_handle(&copy_waiter);
	
	return NULL;
}

void* nspi__regist_event_proc(void* argv) {
	posix__waitable_handle_t waiter;
	posix__init_synchronous_waitable_handle(&waiter);
	static uint32_t next_min_time = 1000;
	static uint64_t begin_tick;
	struct list_head *loop;
	struct list_head *item_loop;
	int runned_time = 0;
	struct list_head reached_event_list;
	int total_ack_length = 0, reached_num = 0;
	int i = 0, add_len = 0, t_last_time;
	
	
	while ( 1 ) {
		begin_tick = posix__gettick();
		next_min_time = 1000;
		
		posix__pthread_mutex_lock(&client_mutex);
		list_for_each(loop, &g_client_event) {
			st_client_regist_event* node = containing_record(loop, st_client_regist_event, index_);
		
			total_ack_length = 0;
			reached_num = 0;
			INIT_LIST_HEAD(&reached_event_list);  //到达事件列表 
			
			//查找达到时间事件 
			list_for_each(item_loop, &node->list_item_) {
				st_regist_event_item* event_item = containing_record(item_loop, st_regist_event_item, index_);
				if(!event_item) break;
				
				t_last_time = (int) event_item->remaind_time_ - runned_time;
				
				//剩余到达时间 - 上一次运行间隔时间 <= 0, 事件到达
				if(t_last_time <= 0) {
					//时间到达, 组发送数据包长度 
					total_ack_length += sizeof ( nsp__initiactive_protocol_item_t);
					total_ack_length += event_item->length_;
					++reached_num;
					event_item->remaind_time_ = event_item->event_cycle_;
					
					st_reached_event* reached_e = (st_reached_event*) malloc (sizeof(st_reached_event) );
					reached_e->var_id_ = event_item->var_id_;
					reached_e->offset_ = event_item->offset_;
					reached_e->length_ = event_item->length_;
					list_add_tail(&reached_e->index_, &reached_event_list);
					
				}
				else {
					//更新 剩余到达时间 
					event_item->remaind_time_ = t_last_time;
				}
				next_min_time = next_min_time > event_item->remaind_time_ ? event_item->remaind_time_ : next_min_time;
			}
			
			//该客户端有订阅事件到达 
			if(total_ack_length > 0) {
				//组包, 发送订阅事件数据
				total_ack_length += sizeof ( nsp__initiactive_protocol_t);
				nsp__initiactive_protocol_t *ack_common_read = (nsp__initiactive_protocol_t *) malloc (total_ack_length);
				if (!ack_common_read) {
					continue;
				}
				
				memcpy(&ack_common_read->head_, &node->head_, sizeof ( nsp__packet_head_t));
				ack_common_read->head_.size_ = total_ack_length;
				ack_common_read->head_.type_ = PKTTYPE_INITIACTIVE_READ_ACK;
				ack_common_read->ack_time_stamp_ = (uint64_t) ( posix__clock_gettime() / 10000.0);
				ack_common_read->count_ = reached_num;
				nsp__initiactive_protocol_item_t *ack_item = (nsp__initiactive_protocol_item_t *) ack_common_read->items;
				
				//loop 到达事件集合
				while (1) {
#if _WIN32
					if ( list_empty( &reached_event_list ) ) {
						break;
					}
					st_reached_event *e = list_first_entry( &reached_event_list, st_reached_event, index_ );
#else
					st_reached_event *e = list_first_entry_or_null(&reached_event_list, st_reached_event, index_);
					if (!e) {
						break;
					}
#endif
					ack_item->var_id_ = e->var_id_;
					ack_item->offset_ = e->offset_;
					ack_item->length_ = e->length_;
					
					//从全量中取数据 
					st_var_obj_info* cur_obj = (st_var_obj_info*)(g_all_vars->item);
					for (i = 0; i < g_all_vars->total_length_; ++i) {
						if(cur_obj->enable_ > 0 && cur_obj->var_id_ == ack_item->var_id_) {
							if (ack_item->offset_ + ack_item->length_ <= cur_obj->body_length_) {
								ack_item->vartype_ = cur_obj->var_type_;
								
								posix__pthread_mutex_lock(&vars_mutex);
								memcpy(ack_item->data_, (char *)cur_obj->body_ + sizeof(st_var_obj_info) + ack_item->offset_, ack_item->length_);
								posix__pthread_mutex_unlock(&vars_mutex);
								
							} else {
								log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
									"initiactive read memory error, offset=%u, length=%u, body=%u", ack_item->offset_, ack_item->length_, cur_obj->body_length_);
							}
							break;
						}
						
						add_len = sizeof (st_var_obj_info) + cur_obj->body_length_;
						cur_obj = (st_var_obj_info*)((char*)cur_obj + add_len);
					}
					
					add_len = sizeof ( nsp__initiactive_protocol_item_t) + ack_item->length_;
					ack_item = (nsp__initiactive_protocol_item_t *) (((char *) ack_item) + add_len);
					list_del(&e->index_);
					if (e) free(e);
				}
				
				tcp_write(node->link_, ack_common_read->head_.size_, &nsp__packet_maker, ack_common_read); // 发包完成
				
				if(ack_common_read) free(ack_common_read);
				ack_common_read = NULL;
			}
		}
		
		posix__pthread_mutex_unlock(&client_mutex);
		runned_time = nspi__interval_control(&waiter, begin_tick, next_min_time);
		
	}
	
	log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem | kLogTarget_Stdout, "regist event process loop exit.");
	posix__uninit_waitable_handle(&waiter);
	
	return NULL;
}

static
int get_all_objects_length(var__functional_object_t *obj, void *usrdat) {
	int* len = (int*)usrdat;
	*len += obj->body_length_;
	return 0;
}

void nspi__uninit_regist_cycle_event() {
	if(g_all_vars) {
		posix__pthread_mutex_lock(&vars_mutex);
		free(g_all_vars);
		posix__pthread_mutex_unlock(&vars_mutex);
	}
	
	struct list_head *loop;
	struct list_head *item_loop;
	st_client_regist_event* client_item;
	st_regist_event_item* event_item;
	
	posix__pthread_mutex_lock(&client_mutex);
	list_for_each(loop, &g_client_event) {
		client_item = containing_record(loop, st_client_regist_event, index_);
		
		list_for_each(item_loop, &client_item->list_item_) {
			event_item = containing_record(item_loop, st_regist_event_item, index_);
			
			list_del(item_loop);
			INIT_LIST_HEAD(item_loop);
			if(event_item) free(event_item);
			event_item = NULL;
		}
		
		list_del(loop);
        INIT_LIST_HEAD(loop);
		if (client_item) free(client_item);
	}
	
	INIT_LIST_HEAD(&g_client_event);
	posix__pthread_mutex_unlock(&client_mutex);
	
	posix__pthread_mutex_release(&vars_mutex);
	posix__pthread_mutex_release(&client_mutex);
}

int nspi__init_regist_cycle_event() {
	int total_len = 0, i = 0;
	var__traverse_global_objects(get_all_objects_length, (void*)&total_len);
	
	int varcnt = var__query_global_count();
	int *ids = (int *) malloc(varcnt * sizeof (int));
	var__query_global_object_ids(varcnt, ids);
	g_all_vars = (all_var_obj *) malloc(sizeof(all_var_obj) + varcnt * sizeof (st_var_obj_info) + total_len);
	if (!g_all_vars) {
		return -1;
	}
	
	objhld_t hld;
	var__functional_object_t *obj;
	g_all_vars->total_length_ = varcnt;
	st_var_obj_info* cur_obj = (st_var_obj_info*)(g_all_vars->item);
	for (i = 0; i < varcnt; ++i) {
		cur_obj->enable_ = 0;
		cur_obj->var_id_ = ids[i];
		hld = var__getobj_handle_byid(ids[i] );
		obj = (var__functional_object_t *) objrefr(hld);
		cur_obj->var_type_ = obj->type_;
		cur_obj->body_length_ = obj->body_length_;
		cur_obj = (st_var_obj_info*)((char*)cur_obj + sizeof (st_var_obj_info) + obj->body_length_);
	}
	if (ids) free(ids);
	obj = NULL;
	
	log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "global vars count %d, total data len[%d]", varcnt, total_len);
	
	INIT_LIST_HEAD(&g_client_event);
	
	posix__pthread_mutex_init(&client_mutex);
	posix__pthread_mutex_init(&vars_mutex);
	
	// 获取数据线程
	posix__pthread_create(&copy_timer, &nspi__copy_vars_proc, NULL);
	// 推送数据线程
	posix__pthread_create(&rce_timer, &nspi__regist_event_proc, NULL);
	
	return 0;
}

