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

#include "posix_wait.h"
#include "posix_thread.h"
#include "libgzcamera.h"

struct ncb_t {
	int sockfd;
	int ctrlfd;

	posix__pthread_t thread_rcv;
	posix__pthread_t thread_control_rcv;
	posix__pthread_t thread_image_handler;

	posix__waitable_handle_t evt_image_handler;
	posix__waitable_handle_t evt_config_parameter_completed;
	posix__waitable_handle_t evt_firmware_control_completed;
	posix__waitable_handle_t evt_parameter_completed;

	posix__pthread_mutex_t mange_lock;

	struct sockaddr_in local_addr;
	struct sockaddr_in local_control_addr;

	void *callback;

	uint8_t firmware_status;
	struct packet_config_parameter parameter;
	struct packet_config_parameter parameter_request;
	struct parameter_post_t para_meter;

	int config_parameter_status;
	int parameter_status;
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