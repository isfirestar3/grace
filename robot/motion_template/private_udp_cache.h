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

#ifndef CACHE_H
#define CACHE_H

#include "version.h"

#include <stddef.h>
#include <stdint.h>

#if !defined CACHE_PRE_PIECES_SIZE
#define CACHE_PRE_PIECES_SIZE           (2048)         
#endif

#if !defined CACHE_PIECES_COUNT
#define CACHE_PIECES_COUNT              (64)           
#endif

enum camera_cache_access {
    kCameraCacheAccess_ReceiverWriteable = 0,
    kCameraCacheAccess_HandlerReadable,
};

#if !_WIN32

int select_cache_memory(int access, unsigned char **buffer);
int select_cache_memory_dec(int access, unsigned char **buffer);
void update_cache_memory(int pic_id, int update_size, uint64_t update_weight, int access_set);

#endif /* !_WIN32 */

#endif /* CACHE_H */

