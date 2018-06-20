/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ncb.h
 * Author: anderson
 *
 * Created on July 27, 2017, 2:41 PM
 */

#ifndef NCB_H
#define NCB_H

#include <netdb.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>

#include "imgdef.h"
#include "posix_wait.h"
#include "posix_thread.h"

struct ncb_t {
    int sockfd;
    int ctrlfd;
    
    posix__pthread_t thread_rcv;
    posix__pthread_t thread_control_rcv;
    posix__pthread_t thread_image_handler;
    
    posix__waitable_handle_t evt_image_handler;
    posix__waitable_handle_t evt_config_parameter_completed;
    posix__waitable_handle_t evt_firmware_status_report;
    
    posix__pthread_mutex_t mange_lock;
    
    struct sockaddr_in local_addr;
    struct sockaddr_in local_control_addr;
    
    void *callback;
    
    uint8_t firmware_status;
    struct packet_config_parameter parameter;
    struct packet_config_parameter parameter_request;
    
    int config_parameter_status;
};

#define CONFIG_PARAMETER_STATUS_INIT            (0)
#define CONFIG_PARAMETER_STATUS_WAITTING        (1)
#define CONFIG_PARAMETER_STATUS_READY           (2)
#define CONFIG_PARAMETER_STATUS_FAULT           (3)

#define FIRMWARE_STATUS_HANG                    (-1)
#define FIRMWARE_STATUS_STOPED                  (0)
#define FIRMWARE_STATUS_RUNNING                 (1)
#define FIRMWARE_STATUS_WARNING                 (0100)
#define FIRMWARE_STATUS_ERROR                   (0200)

void ncbclose(struct ncb_t *ncb);

#endif /* NCB_H */