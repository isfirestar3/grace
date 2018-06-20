#if !defined BUS_HEADER_H
#define BUS_HEADER_H

#include "vartypes.h"
#include "canio.h"
#include "object.h"

typedef void *(*drive_create_dup_t)(objhld_t);
typedef void( *drive_release_dup_t)(void *);

#pragma pack(push, 1)

typedef struct _var__canbus {
    /*+000*/ int canbus_id_; // port
    /*+004*/ int canbus_type_;
    /*+008*/ int baud_rate_;
    /*+012*/ struct list_head mnt_; // 这条总线下,所挂载的具体的驱动(ID)列表
    /*+020*/ int mnt_cnt_; // mnt 个数
    /*+024*/ posix__pthread_t thrw_; // 驱动器IO线程
} var__canbus_t;

#pragma pack(pop)

#if !defined __cplusplus

extern
int var__load_canbus();
extern
int var__get_canio_arg(int bus, canio__busarg_t *argv);
extern
void var__attach_busrw(int bus_object_id, drive_create_dup_t create_dup_proc, drive_release_dup_t release_dup_proc, objhld_t drive_handle);

#endif
#endif