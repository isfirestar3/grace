/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   jpeg.h
 * Author: Stefan
 *
 * Created on May 2, 2017, 9:42 AM
 */

#ifndef JPEG_H
#define JPEG_H

#ifdef __cplusplus
extern "C" {
#endif
    extern void jpeg_init(void);
    extern void jpeg_exit(void);
    extern int jpeg_decompress(uint8_t *buf_compressed, uint32_t jpg_size, uint8_t *buf_decompressed);
#ifdef __cplusplus
}
#endif

#endif /* JPEG_H */

