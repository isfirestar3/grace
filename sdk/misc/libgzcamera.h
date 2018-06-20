/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   network.h
 * Author: anderson
 *
 * Created on July 27, 2017, 3:04 PM
 */
#pragma once
#ifndef LIBGZCAMERA_H
#define LIBGZCAMERA_H

#include "imgdef.h"

#if !defined __extern__
	#if defined __cplusplus
		#define __extern__ extern "C"
	#else
		#define __extern__ extern
	#endif
#endif

#if !defined EXP
	#if _WIN32
		#if defined _USRDLL
			#define METH dllexport
		#else
			#define METH dllimport
		#endif // !_USRDLL
		#define EXP(type)  __extern__ _declspec(METH) type 
	#else
		#define EXP(type)  __extern__ type
	#endif // !_WIN32
#endif // !EXP


EXP(int) camera_start_service(const char *module_address, camera_rcv_callback_t callback);

EXP(int)  camera_start_service2(const char *module_address, uint16_t data_port, uint16_t ctrl_port, camera_rcv_callback_t callback);

EXP(int) camera_control_firmware(uint8_t control_code);

EXP(int) camera_config_parameters(struct packet_config_parameter *parameter);

EXP(uint8_t) camera_query_status();

EXP(void) camera_query_firmware_info(char *name, int *namelen, uint32_t *checksum);

#ifdef WIN32
EXP(int) camera_parameter_data(char* parameter_path, struct parameter_data_t *parameter_data);
#endif

#endif /* LIBGZCAMERA_H */