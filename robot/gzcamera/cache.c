/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "cache.h"
#include "atomic.h"
#include "posix_thread.h"
#include "logger.h"
#include "imgdef.h"

#pragma pack(push, 1)

struct cache_node_t {
    int access; // 访问权限
    uint64_t weight; // 节点权重， Handler 线程永远都取出权重最小的一个节点(当前应用中， 权重为节点image的头帧Epol时间戳)
    //unsigned char buffer[CACHE_PRE_PIECES_SIZE]; // 数据区
    unsigned char *buffer;
};

struct cache_pool {
    struct cache_node_t nodes[CACHE_PIECES_COUNT];
};

#pragma pack(pop)

static posix__pthread_mutex_t cache_locker;
static struct cache_pool *camera_cache = NULL;

void display_cache_blocks(const char *proc) {
    int i;
    struct cache_node_t *node;
    struct gzimage_t *image;
    char dump_blocks_string[2048];
    int off;
    
#if !TRACE_CAMERA_DEBUG
    return;
#endif // !TRACE_CAMERA_DEBUG

    off = 0;
    off += sprintf(&dump_blocks_string[off], "%s blocks dump:\n", proc);

    posix__pthread_mutex_lock(&cache_locker);

    for (i = 0; i < CACHE_PIECES_COUNT; i++) {
        node = &camera_cache->nodes[i];

        image = (struct gzimage_t *) node->buffer;
        off += sprintf(&dump_blocks_string[off], "[%d] access=%d weight=%llu image_id=%u image_current_bytes=%u image_total_bytes=%u image_top_ts=%llu\n",
                i, node->access, node->weight,
                image->image_id, image->image_current_bytes, image->image_total_bytes, image->image_top_recv_ts);
    }
    log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "%s", dump_blocks_string);
    posix__pthread_mutex_unlock(&cache_locker);
}

int init_cache_memory() {
    int i;
    
    if (!camera_cache) {
        log__save("libgzcamera", kLogLevel_Info, kLogTarget_Filesystem, "allocate cache pool.");
        if (NULL == (camera_cache = (struct cache_pool *) malloc(sizeof (struct cache_pool)))) {
            return -ENOMEM;
        }
        bzero(camera_cache, sizeof (struct cache_pool));
        posix__pthread_mutex_init(&cache_locker);
        
        for (i = 0; i < CACHE_PIECES_COUNT; i++){
            camera_cache->nodes[i].buffer = (unsigned char *)malloc(CACHE_PRE_PIECES_SIZE);
            bzero(camera_cache->nodes[i].buffer, CACHE_PRE_PIECES_SIZE);
        }
    }
    return 0;
}

void uninit_cache_memory(){
    int i;
    
    if (camera_cache) {
        for (i = 0; i < CACHE_PIECES_COUNT; i++){
            if (camera_cache->nodes[i].buffer) {
                free(camera_cache->nodes[i].buffer);
                camera_cache->nodes[i].buffer = NULL;
            }
        }
        
        free(camera_cache);
    }
}

int select_cache_memory(int access, unsigned char **buffer) {
    int i;
    int index;
    struct cache_node_t *node;

    if (!buffer || !camera_cache) {
        return -1;
    }

    display_cache_blocks("select_cache_memory");

    posix__pthread_mutex_lock(&cache_locker);
    index = -1;
    for (i = 0; i < CACHE_PIECES_COUNT; i++) {
        node = &camera_cache->nodes[i];
        //log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "select_cache_memory[%d] access=%d[%d] weight=%llu ", i, node->access, access, node->weight);
        if (access == node->access) {
            //*buffer = &node->buffer[0];
            *buffer = node->buffer;
            index = i;
            break;
        }
    }
    posix__pthread_mutex_unlock(&cache_locker);

    return index;
}

int select_cache_memory_dec(int access, unsigned char **buffer) {
    int i;
    int min_weight_index = -1;
    uint64_t current_minimum_weitht = (uint64_t) (~0);
    struct cache_node_t *node;

    if (!buffer || !camera_cache) {
        return -1;
    }

    display_cache_blocks("select_cache_memory_dec");

    posix__pthread_mutex_lock(&cache_locker);
    for (i = 0; i < CACHE_PIECES_COUNT; i++) {
        node = &camera_cache->nodes[i];
        //log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "select_cache_memory_dec[%d] access=%d[%d] weight=%llu ", i, node->access, access, node->weight);
        if ((access == node->access) && (node->weight > 0)) {
            if (node->weight < current_minimum_weitht) {
                current_minimum_weitht = camera_cache->nodes[i].weight;
                min_weight_index = i;
            }
        }
    }

    if (min_weight_index >= 0) {
        //*buffer = &camera_cache->nodes[min_weight_index].buffer[0];
        *buffer = camera_cache->nodes[min_weight_index].buffer;
    }
    posix__pthread_mutex_unlock(&cache_locker);

    return min_weight_index;
}

void update_cache_memory(int pic_id, uint64_t weight, int access) {
    display_cache_blocks("update_cache_memory step0");
    posix__pthread_mutex_lock(&cache_locker);
    if (pic_id < CACHE_PIECES_COUNT) {

        /* 更新节点权重 */
        // posix__atomic_xchange(&camera_cache->nodes[pic_id].weight, update_weight);
        camera_cache->nodes[pic_id].weight = weight;

        /* 更新使用权限 */
        //posix__atomic_xchange(&camera_cache->nodes[pic_id].access, access);
        camera_cache->nodes[pic_id].access = access;
    }
    posix__pthread_mutex_unlock(&cache_locker);
    display_cache_blocks("update_cache_memory step1");
}