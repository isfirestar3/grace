/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <errno.h>

#include "ncb.h"

void ncbclose(struct ncb_t *ncb) {
    if (ncb){
        if (ncb->sockfd > 0){
            shutdown(ncb->sockfd, SHUT_RDWR);
            close(ncb->sockfd);
            ncb->sockfd = -1;
        }
        
        if (ncb->ctrlfd > 0){
            shutdown(ncb->ctrlfd, SHUT_RDWR);
            close(ncb->ctrlfd);
            ncb->ctrlfd = -1;
        }
        
        posix__uninit_waitable_handle(&ncb->evt_config_parameter_completed);
        posix__uninit_waitable_handle(&ncb->evt_image_handler);
        posix__uninit_waitable_handle(&ncb->evt_firmware_status_report);
        
    }
}