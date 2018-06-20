// memdjpeg - A super simple example of how to decode a jpeg in memory
// Kenneth Finnegan, 2012
// blog.thelifeofkenneth.com
//
// After installing jpeglib, compile with:
// cc memdjpeg.c -ljpeg -o memdjpeg
//
// Run with:
// ./memdjpeg filename.jpg
//
// Version	   Date		Time		  By
// -------	----------	-----		---------
// 0.01		2012-07-09	11:18		Kenneth Finnegan
//
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include<string.h>
#include<stdbool.h>
#include<stdint.h>
#include <syslog.h>
#include <time.h>
#include <fcntl.h>

#include <jpeglib.h>
#include <setjmp.h>

#define MAX_JPEG_SIZE 360960 //752 * 480

struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

struct my_error_mgr jerr;

static uint32_t row_stride, width, height, pixel_size;

uint32_t jpg_size; //input size
uint32_t bmp_size = 752 * 480 * 1; //output size

struct jpeg_decompress_struct cinfo;

void my_error_exit(j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

void jpeg_init(void) {
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    jpeg_create_decompress(&cinfo);
}

void jpeg_exit(void) {
    jpeg_destroy_decompress(&cinfo);
}

int jpeg_decompress(uint8_t *buf_compressed, uint32_t jpg_size, uint8_t *buf_decompressed) {
    if (jpg_size > MAX_JPEG_SIZE) {
        return -1;
    }

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        jpeg_create_decompress(&cinfo);
    }

    jpeg_mem_src(&cinfo, buf_compressed, jpg_size);

    int rc;
    rc = jpeg_read_header(&cinfo, TRUE);

    if (rc != 1) {
        return -1;
    }

    cinfo.out_color_space = JCS_GRAYSCALE;

    jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
    height = cinfo.output_height;
    pixel_size = cinfo.output_components;

    row_stride = width * pixel_size;

    unsigned char *buffer_array[1];

    while (cinfo.output_scanline < cinfo.output_height) {
        buffer_array[0] = buf_decompressed + (cinfo.output_scanline) * row_stride;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    jpeg_finish_decompress(&cinfo);
    return 0;
}

