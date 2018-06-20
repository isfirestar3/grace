/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "ncb.h"
#include "cache.h"
#include "imgdef.h"
//#include "jpeg.h"
#include "libgzcamera.h"
//#include <unistd.h>

#include "posix_wait.h"
#include "logger.h"

extern
uint64_t getts();

static int image_decompress(uint8_t *jpeg, int len, uint8_t *bmp) {
//    jpeg_init();
//    if (jpeg_decompress(jpeg, len, bmp) < 0) {
//        return -1;
//    }
//    jpeg_exit();
   return 0;
}

void *routine_handler(void *p) {
	struct ncb_t *ncb = (struct ncb_t*)malloc(sizeof(struct ncb_t));
	ncb = (struct ncb_t *) p;
    int pic_id;
    unsigned char *buffer;
    struct gzimage_t *image;
    camera_rcv_callback_t callback = (camera_rcv_callback_t) ncb->callback;
    
    uint64_t ts0,ts1,ts2;
    ts0 = ts1 = ts2 = 0;

    log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "libgzcamera image handler thread startup.");

    while (1) {
        ts0 = ts1 = ts2 = 0;
        posix__waitfor_waitable_handle(&ncb->evt_image_handler, 1000);

        while ((pic_id = select_cache_memory_dec(kCameraCacheAccess_HandlerReadable, &buffer)) >= 0) {
            ts0 = ts1 = ts2 = 0;
            image = (struct gzimage_t *) buffer;

            image->imgae_exec_ts = getts();
            ts0 = getts();
             
#if TRACE_CAMERA_DEBUG
            log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "handler thread get memory block=%d, image=%d.len=%d", pic_id,image->image_id, image->image_total_bytes );
#endif
            
            // 使用 jpeg 格式传输数据， 需要解压
            //if (ncb->parameter_request.output_type == OUTPUT_TYPE_JPEG) {
               // if (image_decompress(&buffer[sizeof (struct gzimage_t)], image->image_total_bytes, &buffer[sizeof (struct gzimage_t) +image->image_total_bytes]) >= 0) {
                    ts1 = getts();
                   // callback((struct gzimage_t *) buffer, (const unsigned char *) &buffer[sizeof (struct gzimage_t)]);
               // }else{
                 //   log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "libgzcamera decompress JPEG image failed.");
              //  }
           // } else {
                callback((struct gzimage_t *) buffer, (const unsigned char *) &buffer[sizeof (struct gzimage_t)]);
           // }
            ts2 = getts();
            update_cache_memory(pic_id, 0, kCameraCacheAccess_ReceiverWriteable);
            
#if TRACE_CAMERA_DEBUG
            log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "handler thread update memory block=%d", pic_id );
#endif
            if ((ts2 - ts0) > 15000) {
                log__save("libgzcamera", kLogLevel_Warning, kLogTarget_Filesystem, "image handler timeout.%llu %llu %llu", ts0, ts1, ts2);
            }
        }
    }

    log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "libgzcamera image handler thread terminated.");
    return NULL;
}