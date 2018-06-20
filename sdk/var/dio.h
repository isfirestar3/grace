#if !defined DIO_H
#define DIO_H

#include "vartypes.h"

#if !defined MAXIMUM_DIO_BLOCK_COUNT
#define MAXIMUM_DIO_BLOCK_COUNT         (10)
#endif /*!MAXIMUM_DIO_BLOCK_COUNT */

#if !defined DIO_BLOCK_DATACB
#define DIO_BLOCK_DATACB            (16)
#endif /*!DIO_BLOCK_DATACB */

#pragma pack (push, 1)

typedef struct {
    int channel_;       // 通道号， 相当于下标索引
    int norm_;          // 模拟输入输出
    int scale_coef_;    // 模拟输入Scale
    int bias_coef_;     // 模拟输入Bias
} var__dio_base_config_t;

typedef struct {
    int start_address_; /* 起始地址 start_address_  0 则该块无效 */
    int effective_count_of_index_;      /* 地址下生效的索引个数， 指明了 @data_ 的实际有效长度 */
    int internel_type_;          /* 内部自定义类型, 指明 @data_ 元素类型是几位 */
    int data_[DIO_BLOCK_DATACB];
}var__dio_canbus_usrdef_block_t;

typedef struct {
    /*+000*/ var__can_device_t candev_head_;

    /*+424*/ int di_channel_num_; 
    /*+428*/ int do_channel_num_; 

    /*+440*/ var__status_describe_t status_;

    /*+452*/ int do_;
    /*+456*/ var__dio_canbus_usrdef_block_t ao_[MAXIMUM_DIO_BLOCK_COUNT];

    union {
#if __cplusplus
        struct {
#else
        struct _i_dio_internal_t {
#endif 
            /*+520*/ int bus_state_; // CAN OPEN STATES
            /*+524*/ int error_code_;
            /*+528*/ uint64_t time_stamp_;

            /*+536*/ int enabled_;
            /*+540*/ int di_;
            /*+544*/ var__dio_canbus_usrdef_block_t ai_[MAXIMUM_DIO_BLOCK_COUNT];

            /*+608*/ int do2_;
            /*+612*/ var__dio_canbus_usrdef_block_t ao2_[MAXIMUM_DIO_BLOCK_COUNT];
        } i;

        char u_;
    };
} var__dio_t; /*+676*/

#pragma pack(pop)

#if !defined __cplusplus

#include "object.h"

extern
int var__load_dio();

extern
var__dio_t *var__get_dio(objhld_t hld);

extern
var__dio_t *var__get_dio_byid(int id);

extern
var__dio_t *var__create_dio_dup(objhld_t hld);

extern
var__dio_t *var__create_dio_dup_byid(int id);

extern
void var__release_dio_dup(var__dio_t *dio);

extern
int var__commit_dio_dup(const var__dio_t *dio);

#endif

#endif