#if !defined VAR_USER_DEFINED_H
#define VAR_USER_DEFINED_H

#if !defined MAXIMUM_USRBUF_SIZE
#define MAXIMUM_USRBUF_SIZE   (1024)
#endif

typedef struct {
    /*+000*/ char usrbuf_[MAXIMUM_USRBUF_SIZE];
} var__usrdef_buffer_t;

#if !defined __cplusplus

#include "object.h"

extern
int var__load_usrbuf();

/* 根据设备句柄或者设备ID获取 opt 设备所指向的 opt 对象原始数据区指针，必须手动  var__release_object_reference 释放
 */
extern
var__usrdef_buffer_t *var__get_usrbuf();

/* 创建一份指定句柄或者ID的 opt 设备对象的拷贝, 使用完成后必须手动 var__release_usrbuf_dup 释放
 */
extern
var__usrdef_buffer_t *var__create_usrbuf_dup();

/* 将籍由 var__create_usrbuf_dup 获得的 opt 对象拷贝提交到原始数据区
 */
extern
int var__commit_usrbuf_dup(const var__usrdef_buffer_t *opt);

/* 释放 var__create_usrbuf_dup  所得的 opt 对象拷贝
 */
extern
void var__release_usrbuf_dup(var__usrdef_buffer_t *opt);

#endif

#endif