/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   cache.h
 * Author: anderson
 *
 * Created on July 27, 2017, 3:07 PM
 */

#include <stddef.h>
#include <stdint.h>

#ifndef CACHE_H
#define CACHE_H

#if !defined CACHE_PRE_PIECES_SIZE
#define CACHE_PRE_PIECES_SIZE           (1 << 20)         // 单片缓冲区长度 1M,//(128<<10)128KB
#endif

#if !defined CACHE_PIECES_COUNT
#define CACHE_PIECES_COUNT              (8)            // 总共64片，组成缓冲区
#endif

#define TRACE_CAMERA_DEBUG      (0)

enum camera_cache_access {
    kCameraCacheAccess_ReceiverWriteable = 0,
    kCameraCacheAccess_HandlerReadable,
};

int init_cache_memory();
void uninit_cache_memory();

/* 查找第一个找到的可用缓存片， 返回缓存片索引，得到缓存片指针@*buffer */
int select_cache_memory(int access, unsigned char **buffer);

/* 查找权重最小的可用缓存片， 返回缓存片索引，得到缓存片指针@*buffer */
int select_cache_memory_dec(int access, unsigned char **buffer);

/* 将 @pic_id 索引所在的缓存片存储，并更改权限为 @access_set  */
void update_cache_memory(int pic_id, uint64_t weight, int access);

#endif /* CACHE_H */

