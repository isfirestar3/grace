#include "camera_net_impls.h"
#include <stdlib.h>
#include <stdio.h>
#include "network.h"
#include "imgdef.h"

EXP(int) init_net_session(const char *module_address, int(*camera_rcv_callback_t)(void *image, const unsigned char *pic))
{
	return camera_start_service(module_address, camera_rcv_callback_t);
}

EXP(int) post_config_info(packet_config_parameter config_info)
{	
	return camera_config_parameters(&config_info);
}

EXP(int) post_control_info(uint8_t control_info)
{
	return camera_control_firmware(control_info);
}

//EXP(void) register_image_callback(int(*notify_image_callback)(const unsigned char* bytes, int id, int len, uint32_t width, uint32_t high, uint64_t time_stamp))
//{
//	if (notify_image_callback) {
//		register_notify_callback(notify_image_callback);
//	}
//}

EXP(int) post_parameter_info(char* parameter_path, parameter_data_t parameter_data)
{
	parameter_post_t parameter_post;
	parameter_post.comm_code_ = CMD_CAMREA_PARAMETER;
	parameter_post.length_ = 72;
	memcpy(parameter_post.model_, "IPC36M", 32);
	if (!parameter_path)
	{
		return -1;
	}
	FILE *fp = NULL;
	if (fopen_s(&fp, parameter_path, "r") != 0){
		return -1;
	}
	long size;
	parameter_data_t __parameter_data;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char*ptr = (char*)malloc(size);
	fread(ptr, size, 1, fp);
	char* ptr1 = strstr(ptr, "FocalLength: [");
	if (!ptr1) return -1;
	ptr1 = &ptr1[14];
	char* ptr2 = strstr(ptr1, "]");
	if (!ptr2) return -1;
	int t1 = ptr2 - ptr1 + 1;
	char*FocalLength1 = (char*)malloc(t1);
	memcpy(FocalLength1, ptr1, t1);

	char* data1 = strstr(FocalLength1, " ");
	if (!data1) return -1;
	t1 = data1 - FocalLength1;
	char*tmp1 = (char*)malloc(t1);
	if (!tmp1) return -1;
	memcpy(tmp1, FocalLength1, t1);
	double data = atof(tmp1);
	__parameter_data.FocalLength_1 = data;
	data = atof(data1 + 1);
	__parameter_data.FocalLength_2 = data;


	char* ptr3 = strstr(ptr, "PrincipalPoint: [");
	if (!ptr3) return -1;
	ptr3 = &ptr3[17];
	char*ptr4 = strstr(ptr3, "]");
	if (!ptr4) return -1;
	t1 = ptr4 - ptr3 + 1;
	char*PrincipalPoint = (char*)malloc(t1);
	memcpy(PrincipalPoint, ptr3, t1);
	char* data2 = strstr(PrincipalPoint, " ");
	if (!data2) return -1;
	t1 = data2 - PrincipalPoint;
	char*tmp2 = (char*)malloc(t1);
	memcpy(tmp2, PrincipalPoint, t1);
	data = atof(tmp2);
	__parameter_data.PrincipalPoint_1 = data;
	data = atof(data2 + 1);
	__parameter_data.PrincipalPoint_2 = data;

	char* ptr5 = strstr(ptr, "RadialDistortion: [");
	if (!ptr5) return -1;
	ptr5 = &ptr5[19];
	char*ptr6 = strstr(ptr5, "]");
	if (!ptr6) return -1;
	t1 = ptr6 - ptr5 + 1;
	char*RadialDistortion = (char*)malloc(t1);
	memcpy(RadialDistortion, ptr5, t1);
	char* data3 = strstr(RadialDistortion, " ");
	if (!data3) return -1;
	t1 = data3 - RadialDistortion;
	char*tmp3 = (char*)malloc(t1);
	memcpy(tmp3, RadialDistortion, t1);
	data = atof(tmp3);
	__parameter_data.RadialDistortion_1 = data;
	char* data4 = strstr(data3 + 1, " ");
	if (!data4) return -1;
	t1 = data4 - data3 - 1;
	char*tmp4 = (char*)malloc(t1);
	memcpy(tmp4, data3 + 1, t1);
	data = atof(tmp4);
	__parameter_data.RadialDistortion_2 = data;
	data = atof(data4 + 1);
	__parameter_data.RadialDistortion_3 = data;


	char* ptr7 = strstr(ptr, "TangentialDistortion: [");
	ptr7 = &ptr7[23];
	char*ptr8 = strstr(ptr7, "]");
	t1 = ptr8 - ptr7 + 1;
	char*TangentialDistortion = (char*)malloc(t1);
	memcpy(TangentialDistortion, ptr7, t1);
	char* data5 = strstr(TangentialDistortion, " ");
	t1 = data5 - TangentialDistortion;
	char*tmp5 = (char*)malloc(t1);
	memcpy(tmp5, TangentialDistortion, t1);
	data = atof(tmp5);
	__parameter_data.TangentialDistortion_1 = data;
	data = atof(data5 + 1);
	__parameter_data.TangentialDistortion_2 = data;

	free(ptr);
	free(tmp1);
	free(tmp2);
	free(tmp3);
	free(tmp4);
	free(tmp5);
	free(FocalLength1);
	free(PrincipalPoint);
	free(RadialDistortion);
	free(TangentialDistortion);
	fclose(fp);
	memcpy(&parameter_post.parameter_data_[0], &__parameter_data.FocalLength_1, 8);
	memcpy(&parameter_post.parameter_data_[8], &__parameter_data.FocalLength_2, 8);
	memcpy(&parameter_post.parameter_data_[16], &__parameter_data.PrincipalPoint_1, 8);
	memcpy(&parameter_post.parameter_data_[24], &__parameter_data.PrincipalPoint_2, 8);
	memcpy(&parameter_post.parameter_data_[32], &__parameter_data.RadialDistortion_1, 8);
	memcpy(&parameter_post.parameter_data_[40], &__parameter_data.RadialDistortion_2, 8);
	memcpy(&parameter_post.parameter_data_[48], &__parameter_data.RadialDistortion_3, 8);
	memcpy(&parameter_post.parameter_data_[56], &__parameter_data.TangentialDistortion_1, 8);
	memcpy(&parameter_post.parameter_data_[64], &__parameter_data.TangentialDistortion_2, 8);

	camera_parameter_data(parameter_post, parameter_data);
	return 0;
}

EXP(void) query_firmware_info(char *name, int *namelen, uint32_t *checksum)
{
	camera_query_firmware_info(name, namelen, checksum);
}

EXP(uint8_t) query_status()
{
	camera_query_status();
}