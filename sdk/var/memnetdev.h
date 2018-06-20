#if !defined MEMNETDEV_H
#define MEMNETDEV_H

#include "vartypes.h"

#if !defined MEM_BLOCK_REGION_DATA_SIZE
#define MEM_BLOCK_REGION_DATA_SIZE              (256)   /*实际使用中以字为单位*/
#endif

#if defined MEM_MAXIMUM_BLOCK
#undef MEM_MAXIMUM_BLOCK
#endif
#define MEM_MAXIMUM_BLOCK           (10)

#pragma pack(push,1)

typedef struct {
    /*+000*/ posix__boolean_t actived_; /* 该片区是否启用 */
    /*+004*/ int start_address_; /* 起始地址 */
    /*+008*/ int effective_count_in_word_; /* 生效的数据节点个数, 以"字"为配置单位 */
    /*+012*/ uint64_t time_stamp_;
    /*+016*/ posix__boolean_t period_; /* 此片区将被周期性写向模块 */
    /*+020*/ posix__boolean_t flush_; /* 此片区本周期将被写向模块 */
    /*+024*/ uint8_t data_[MEM_BLOCK_REGION_DATA_SIZE];
} var__memory_data_block; /*+280*/

typedef struct {
    /*+000*/ var__network_device_t netdev_head_;

    /*+044*/ int interval_for_read_thread_; /* 读线程的循环间隔 单位:毫秒 */
    /*+048*/ int interval_for_write_thread_; /* 写线程的循环间隔 单位:毫秒 */

    /*+052*/ var__memory_data_block writeable_blocks_[MEM_MAXIMUM_BLOCK]; /* 10片读写片区，驱动可以从设备读取数据，同时这些数据可以写给设备 */
    /*+332*/ var__memory_data_block readonly_blocks[MEM_MAXIMUM_BLOCK]; /* 10片只读片区， 由驱动层从设备获取 */
} var__memory_netdev_t; /* +612 */

#pragma pack(pop)

#if !defined __cplusplus

#include "object.h"

extern
int var__load_memory_netdev();

/* 根据设备句柄或者设备ID获取 omron_plc 设备所指向的 omron_plc 对象原始数据区指针，必须手动  var__release_object_reference 释放
 */
extern
var__memory_netdev_t *var__get_memory_netdev_byid(int id);
extern
var__memory_netdev_t *var__get_memory_netdev(objhld_t hld);

#endif

#endif