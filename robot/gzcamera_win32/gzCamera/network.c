/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
//避免Windows.h和winsock.h中重复的
#if !defined WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <winsock.h>

#include <stdio.h>
#include "ncb.h"
#include "cache.h"
#include "imgdef.h"
#include "atomic.h"
#include "libgzcamera.h"
#include <string.h>
#include <time.h>
#include "logger.h"
#include "posix_naos.h"
#include <malloc.h>


#define CAMERA_CTRL_PORT (0x5050)
#define CAMERA_DATA_PORT (0X5151)

static struct ncb_t* local_ncb;
static struct sockaddr_in camera_addr, camera_control_addr;
#define INFINITE_LOOP		(1)

extern
void *routine_handler(void *p);



uint64_t getts() {

	time_t realtime;
	return time(&realtime);
   /*uint64_t ptime;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ptime = (uint64_t) ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
    return ptime;*/
}

static int udp_update_opts(int fd) {
    static const int RECV_BUFFER_SIZE = 0xFFFF;
    static const int SEND_BUFFER_SIZE = 0xFFFF;
    struct linger lgr;

    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const void *) &RECV_BUFFER_SIZE, sizeof (RECV_BUFFER_SIZE));
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const void *) &SEND_BUFFER_SIZE, sizeof (SEND_BUFFER_SIZE));
    lgr.l_onoff = 0;
    lgr.l_linger = 1;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *) &lgr, sizeof ( struct linger));
    return 0;
}
static int ack_frame_completed(uint16_t image_id, uint32_t fatal_cnt) {
	struct packet_frame_ack ack;
	ack.image_id = image_id;
	ack.fatal_frame_count = fatal_cnt;
	return sendto(local_ncb->sockfd, (const void *)&ack, sizeof(ack), 0, & camera_addr, sizeof(camera_addr));
}
static int recv_image_head_msg(struct packet_zero_frame *frame, struct gzimage_t *image, unsigned char *data_storage_ptr, int *if_last_sub_frame) {
    int zero_frame_data_length;

    if (!frame || !image || !data_storage_ptr || !if_last_sub_frame) {
        return -1;
    }

    // 第0帧数据长度 = 帧长 - (第0帧包结构长度 - 包头长度)
    zero_frame_data_length = frame->head.frame_size - sizeof (struct packet_zero_frame) + sizeof (struct packet_head);

    image->image_id = frame->head.image_id;

    // 当前拷贝长度
    image->image_current_bytes = 0;

    // 图片总长度
    image->image_total_bytes = frame->imgsize;
    
#if TRACE_CAMERA_DEBUG
    log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "receive image. frame=%u, len=%u zero=%u", image->image_id, image->image_total_bytes, zero_frame_data_length);
#endif
    
    image->image_width = frame->width;
    image->image_high = frame->high;
    image->image_top_recv_ts = getts();
    image->image_pkt_adjust_ts = frame->timestamp;
    image->image_timestamp = image->image_top_recv_ts - frame->timestamp; // 当前系统时间 - 相机内部相对时间差， 得到绝对时间戳

    // 拷贝第0帧的数据到缓冲区
	memcpy(data_storage_ptr, frame->data, zero_frame_data_length);

    // 增加已经拷贝的长度记录
    image->image_current_bytes += zero_frame_data_length;

    // 如果仅头帧即可将数据传输完成
    if (image->image_current_bytes == image->image_total_bytes) {
        *if_last_sub_frame = 1;
    } else {
        *if_last_sub_frame = 0;
    }
    return zero_frame_data_length;
}

static int recv_image_body_msg(struct packet_frame *frame, struct gzimage_t *image, unsigned char *data_storage_ptr, int *if_last_sub_frame) {
    //static const socklen_t camera_addr_len = sizeof (camera_addr);
	static const int camera_addr_len = sizeof(camera_addr);

    if (!frame || !image || !data_storage_ptr || !if_last_sub_frame) {
        return -1;
    }

    // 拷贝数据
	memcpy(data_storage_ptr, frame->data, frame->head.frame_size);

    // 调整拷贝进度
    image->image_current_bytes += frame->head.frame_size;

#if TRACE_CAMERA_DEBUG
    log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "receive body copy len=%u, current=%u", frame->head.frame_size, image->image_current_bytes);
#endif
    
    // 结束帧将最高位置1, 这里清除最高位，并判断是否应答最后一帧
    *if_last_sub_frame = 0;
    if (frame->head.frame_id & 0x8000) {
		ack_frame_completed(frame->head.image_id, 0);

        image->image_buttom_recv_ts = getts();
        *if_last_sub_frame = 1;
    }
    
    if ((image->image_current_bytes == image->image_total_bytes) && (0 == *if_last_sub_frame)){
        log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "size overflow.head.id=%u", frame->head.frame_id);
    }
    return frame->head.frame_size;
}

static int image_receiver(unsigned char *cache_ptr) {
    int rcvcb;
    struct sockaddr_in from_addr;
    int from_len;
    int err;
    char rcvbuf[2048];
    struct gzimage_t *image = (struct gzimage_t *) cache_ptr;
    unsigned char *data_storage_ptr;
    int offset;
    int if_last_sub_frame;

    if (!cache_ptr) {
        return -1;
    }

    /*
     * 说明:
     * cache 整块的布局方式为:
     * [gzimage_t][数据包中的图片数据][(如果需要解压)解压后的图片数据]......
     * 因此有:
     * cache[sizeof(gzimage_t)] 为图片数据的起始偏移
     * cache::using == (sizeof(gzimage_t) + gzimage_t::image_total_bytes)
     */
    offset = 0;
    image = (struct gzimage_t *) cache_ptr;
    data_storage_ptr = cache_ptr + sizeof (struct gzimage_t);

    while (1) {
        from_len = sizeof (from_addr);
        rcvcb = recvfrom(local_ncb->sockfd, rcvbuf, sizeof (rcvbuf), 0, & from_addr, &from_len);
        err = errno;
        if (rcvcb < 0) {
			err = WSAGetLastError();
            if ((err != EINTR) && (err != EAGAIN)) {
                log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "camera rcv thread fatal syscall recvfrom, error=%d", err);
                return -1;
            }
            continue;
        }

        if (0 == rcvcb) {
            log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "zero size recv by camera.");
            return -1;
        }

        // 锁定 数据来源必须是指定的模块IP和端口
        if (from_addr.sin_addr.s_addr != camera_addr.sin_addr.s_addr || from_addr.sin_port != camera_addr.sin_port) {
            char addr_source[16], addr_config[16];
            posix__ipv4tos(from_addr.sin_addr.s_addr, addr_source, sizeof (addr_source));
            posix__ipv4tos(camera_addr.sin_addr.s_addr, addr_config, sizeof (addr_config));
            log__save("libgzcamera", kLogLevel_Warning, kLogTarget_Filesystem, "camera image data access decline.source endpoint %s:%u, config endpoint %s:%u",
                    addr_source, from_addr.sin_port, addr_config, camera_addr.sin_port);
            return -1;
        }

        // ncb 中的硬件控制状态不符合运行时规则， 不对任何数据做实际处理
               posix__pthread_mutex_lock(&local_ncb->mange_lock);
                if ((0 == (local_ncb->firmware_status & FIRMWARE_STATUS_RUNNING)) ||
                        (1 == (local_ncb->firmware_status & FIRMWARE_STATUS_ERROR)) ||
                        (local_ncb->config_parameter_status != CONFIG_PARAMETER_STATUS_READY)) {
                    log__save("libgzcamera", kLogLevel_Warning, kLogTarget_Filesystem, "local_ncb->firmware_status=%u, local_ncb->config_parameter_status=%u",
                           local_ncb->firmware_status,local_ncb->config_parameter_status);
                    posix__pthread_mutex_unlock(&local_ncb->mange_lock);
                    return -1;
                }
               posix__pthread_mutex_unlock(&local_ncb->mange_lock);

        struct packet_head *phead = (struct packet_head *) rcvbuf;
        if (0 == phead->frame_id) {
            // 收到头帧，但是偏移已经被错误调整过
            if (0 != offset) {
                return -1;
            }
            err = recv_image_head_msg((struct packet_zero_frame *) phead, image, data_storage_ptr, &if_last_sub_frame);

        } else {
            // 还没有得到头帧(或者子帧不属于头帧标记的图片),数据抛弃
            if ((image->image_id != phead->image_id) || (0 == offset)) {
                return -1;
            }
            err = recv_image_body_msg((struct packet_frame *) phead, image, data_storage_ptr + offset, &if_last_sub_frame);
        }

        if (err <= 0) {
            log__save("libgzcamera", kLogLevel_Warning, kLogTarget_Filesystem,"recv image failed.");
            return -1;
        } else {
            offset += err;
        }

        // log__save("libgzcamera", kLogLevel_Info, kLogTarget_Filesystem, "next image data copy offset=%u image=%u if_last_sub_frame=%u", offset, image->image_id, if_last_sub_frame);
        if (if_last_sub_frame) {
            break;
        }
    }

    return 0;
}

static void *routine_rcv(void *p) {
    unsigned char *cache_ptr = NULL;
    int pic_id;
    int retval = 0;
    struct gzimage_t *image;

    log__save("libgzcamera", kLogLevel_Info, kLogTarget_Filesystem, "libgzcamera image data receiver thread startup.");

    while (1) {
        if (retval >= 0) {
            pic_id = select_cache_memory(kCameraCacheAccess_ReceiverWriteable, &cache_ptr);
            if (pic_id < 0) {
                log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "failed to get cache node memory for camera data recviver");
                // 有可能出现的极端情况：接收缓冲区瞬间填满,处理线程响应不及时， 导致一轮 select_cache_memory_dec(kCameraCacheAccess_HandlerReadable, &buffer)) 失败
                // 此时处理线程等待事件，但是无法得到有效通知
                posix__sig_waitable_handle(&local_ncb->evt_image_handler);
                // 10 毫秒等待空出来的缓存块，这里很大可能会导致丢失数据
				Sleep(10);// *1000);
                continue;
            }
        }

#if TRACE_CAMERA_DEBUG
        log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "recv thread get memory block=%d", pic_id);
#endif
        
        // 如果操作成功， 且整图数据已经填满， 则将该片区变更为处理线程可读状态，投递线程消息，同时，下一个循环周期要求重新分配片区
        // 其他任何情况都被视为失败， 什么都不用做， 继续使用这一个片区执行下一个图片的接收
        retval = image_receiver(cache_ptr);
        if (retval >= 0) {
            image = (struct gzimage_t *) cache_ptr;
            if (image->image_current_bytes == image->image_total_bytes) {
                update_cache_memory(pic_id, image->image_top_recv_ts, kCameraCacheAccess_HandlerReadable);
#if TRACE_CAMERA_DEBUG
                log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "recv thread update memory block=%d, frame=%u, len=%u", pic_id, image->image_id, image->image_total_bytes);
#endif
                posix__sig_waitable_handle(&local_ncb->evt_image_handler);
            } else {
                retval = -1;
            }
        }
    }
    return NULL;
}

static void on_config_parameter_ack(const struct packet_config_parameter_ack *cfgpar) {
    if (!cfgpar) {
        return;
    }
    // 使用收到的配置参数填充本地结构
    posix__pthread_mutex_lock(&local_ncb->mange_lock);
    if (0 == cfgpar->error) {
        strcpy(local_ncb->parameter.module, cfgpar->module);
        local_ncb->parameter.checksum = cfgpar->checksum;
        posix__atomic_xchange(&local_ncb->config_parameter_status, CONFIG_PARAMETER_STATUS_READY);
        log__save("libgzcamera", kLogLevel_Info, kLogTarget_Filesystem, "config parameter ack package successful.");
    } else {
		ZeroMemory(local_ncb->parameter.module, sizeof(local_ncb->parameter.module));
        local_ncb->parameter.checksum = 0;
        posix__atomic_xchange(&local_ncb->config_parameter_status, CONFIG_PARAMETER_STATUS_FAULT);
        log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "config parameter ack package fault error=%u", cfgpar->error);
    }
    posix__pthread_mutex_unlock(&local_ncb->mange_lock);

    // 通知 comfig parameter 的调用线程， 配置完成
    posix__sig_waitable_handle(&local_ncb->evt_config_parameter_completed);
}

static void on_firmware_status_report(const struct packet_firmware_status_report *firmware_report) {
    if (!firmware_report) {
        return;
    }
    posix__pthread_mutex_lock(&local_ncb->mange_lock);
    if (0 != strcmp(local_ncb->parameter.module, firmware_report->module) || firmware_report->checksum != local_ncb->parameter.checksum) {
        log__save("libgzcamera", kLogLevel_Warning, kLogTarget_Filesystem, "firmware report package error, config[%s:%u] report[%s:%u]",
                local_ncb->parameter.module, local_ncb->parameter.checksum, firmware_report->module, firmware_report->checksum);
        local_ncb->firmware_status = FIRMWARE_STATUS_HANG;
    } else {
        local_ncb->firmware_status = firmware_report->data;
        if (local_ncb->firmware_status & FIRMWARE_STATUS_ERROR) {
            log__save("libgzcamera", kLogLevel_Warning, kLogTarget_Filesystem, "warnning status report from firmware,code=%u", local_ncb->firmware_status);
        } else {
            if (local_ncb->firmware_status & FIRMWARE_STATUS_WARNING) {
                log__save("libgzcamera", kLogLevel_Warning, kLogTarget_Filesystem, "error status report from firmware,code=%u", local_ncb->firmware_status);
            }
        }
    }
    posix__pthread_mutex_unlock(&local_ncb->mange_lock);
}

static void on_parameter_report(const struct parameter_post_t *para_report) {
	if (!para_report) {
		return;
	}
	// 使用收到的内参填充本地结构
	posix__pthread_mutex_lock(&local_ncb->mange_lock);

	strcpy(local_ncb->para_meter.model_, para_report->model_);
	local_ncb->para_meter.comm_code_ = para_report->comm_code_;
	local_ncb->para_meter.length_ = para_report->length_;
	strcpy(local_ncb->para_meter.parameter_data_, para_report->parameter_data_);

	posix__atomic_xchange(&local_ncb->parameter_status, CONFIG_PARAMETER_STATUS_READY);
		
	posix__pthread_mutex_unlock(&local_ncb->mange_lock);
	// 通知 内参parameter 的调用线程， 配置完成
	posix__sig_waitable_handle(&local_ncb->evt_parameter_completed);
	
}

void *routine_control_rcv(void *p) {
    unsigned char rcvbuf[2048];
    int rcvcb;
    struct sockaddr_in from_addr;
	//struct sockaddr from_addr;
    int from_len;
    uint8_t command;
    int err;

    log__save("libgzcamera", kLogLevel_Info, kLogTarget_Filesystem, "libgzcamera control receiver thread startup.");

    while (1) {
        from_len = sizeof (from_addr);
        rcvcb = recvfrom(local_ncb->ctrlfd, rcvbuf, sizeof (rcvbuf), 0, & from_addr, & from_len);
        err = errno;
		err = WSAGetLastError();
        if (rcvcb == 0) {
            log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "zero size recv by control.");
            break;
        }

        if (rcvcb < 0) {
            if ((err != EINTR) && (err != EAGAIN)) {
                log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "camera control thread fatal syscall recvfrom, error=%d", err);
                break;
            }
            continue;
        }

        // 锁定 数据来源必须是指定的模块IP和端口
        if (from_addr.sin_addr.s_addr != camera_control_addr.sin_addr.s_addr || from_addr.sin_port != camera_control_addr.sin_port) {
            char addr_source[16], addr_config[16];
            posix__ipv4tos(from_addr.sin_addr.s_addr, addr_source, sizeof (addr_source));
            posix__ipv4tos(camera_control_addr.sin_addr.s_addr, addr_config, sizeof (addr_config));
            log__save("libgzcamera", kLogLevel_Warning, kLogTarget_Filesystem, "camera control request access decline.source endpoint %s:%u, config endpoint %s:%u",
                    addr_source, from_addr.sin_port, addr_config, camera_control_addr.sin_port);
            continue;
        }

        command = *(uint8_t *) rcvbuf;
        switch (command) {
            case CMD_CFGPAR_ACK:
                if (sizeof (struct packet_config_parameter_ack) != rcvcb) {
                    log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "config parameter ack package acquire size=%u, recv size=%u",
                            sizeof (struct packet_config_parameter_ack), rcvcb);
                    break;
                }
                on_config_parameter_ack((const struct packet_config_parameter_ack *) rcvbuf);
                break;
            case CMD_FIRMWARE_STATUS_REPORT:
                if (sizeof (struct packet_firmware_status_report) != rcvcb) {
                    log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "firmware status report package acquire size=%u, recv size=%u",
                            sizeof (struct packet_firmware_status_report), rcvcb);
                    break;
                }
                on_firmware_status_report((const struct packet_firmware_status_report *) rcvbuf);
                break;
			case CMD_PARAMETER_ACK:
				if (sizeof(struct parameter_post_t) != rcvcb) {
					log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "parameter package acquire size=%u, recv size=%u",
						sizeof(struct parameter_post_t), rcvcb);
					break;
				}
				on_parameter_report((const struct parameter_post_t*)rcvbuf);
            default:
                log__save("libgzcamera", kLogLevel_Warning, kLogTarget_Filesystem, "unknown control package type from module: %u.", command);
                break;
        }
    }

    log__save("libgzcamera", kLogLevel_Warning, kLogTarget_Filesystem, "control rcv thread has been terminated.");
    return NULL;
}

static int create_sockfd(const char *local_addr, uint16_t port, struct sockaddr_in *output) {
    int fd;
    int optval;
    int retval;
    struct sockaddr_in addr;
    int addrlen;
	WSADATA wsaData;    
	WSAStartup(MAKEWORD(1, 1), &wsaData);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
		log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "failed create services socket.errno=%d.", WSAGetLastError());
        return -1;
    }

    optval = 1;
	retval = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(optval));

    addr.sin_addr.s_addr = INADDR_ANY;
    if (local_addr) {
        addr.sin_addr.s_addr = inet_addr(local_addr);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    retval = bind(fd, (struct sockaddr *) &addr, sizeof ( struct sockaddr));
    if (retval < 0) {
		log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "failed bind local port %u.errno=%d.", port, WSAGetLastError());
        return -1;
    }

    /* setsockopt */
    if (udp_update_opts(fd) < 0) {
        return -1;
    }

    /* 获取本地地址 */
    getsockname(fd, (struct sockaddr *) output, &addrlen);
    return fd;
}
EXP(int)camera_start_service(const char *module_address, camera_rcv_callback_t callback) {
	return camera_start_service2(module_address, CAMERA_DATA_PORT, CAMERA_CTRL_PORT, callback);
}

int camera_start_service2(const char *module_address, uint16_t data_port, uint16_t ctrl_port, camera_rcv_callback_t callback) {
	local_ncb = (struct ncb_t*)malloc(sizeof(struct ncb_t));
    if (!callback || !module_address) {
        log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "invalid module address:%s", module_address);
        return -EINVAL;
    }

    static int camera_inited = 0;
    if (1 != posix__atomic_inc(&camera_inited)) {
        log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "multiple initialize");
        return -1;
    }
	//ZeroMemory(&local_ncb, sizeof(local_ncb));
    local_ncb->firmware_status = FIRMWARE_STATUS_HANG;

    init_cache_memory();

	local_ncb->sockfd = create_sockfd(NULL, data_port, &local_ncb->local_addr);
    if (local_ncb->sockfd < 0) {
        log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "failed to create socket for camera frame data");
        ncbclose(local_ncb);
        return -1;
    }
	local_ncb->ctrlfd = create_sockfd(NULL, ctrl_port, &local_ncb->local_control_addr);
    if (local_ncb->ctrlfd < 0) {
        log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "failed to create socket for control protocol");
        ncbclose(local_ncb);
        return -1;
    }
	posix__pthread_mutex_init(&local_ncb->mange_lock);
    local_ncb->callback = callback;

    /* 指定相机地址/控制地址端口 */
    camera_addr.sin_addr.s_addr = inet_addr(module_address);
    camera_addr.sin_family = AF_INET;
    camera_addr.sin_port = htons(CAMERA_DATA_PORT);
    camera_control_addr.sin_addr.s_addr = inet_addr(module_address);
    camera_control_addr.sin_family = AF_INET;
    camera_control_addr.sin_port = htons(CAMERA_CTRL_PORT);

    /* 启动工作线程和工作线程通知事件 */
    posix__init_synchronous_waitable_handle(&local_ncb->evt_config_parameter_completed);
    posix__init_synchronous_waitable_handle(&local_ncb->evt_image_handler);
    posix__init_synchronous_waitable_handle(&local_ncb->evt_firmware_control_completed);
	posix__init_synchronous_waitable_handle(&local_ncb->evt_parameter_completed);

    /* 开立接收线程 */
    posix__pthread_create(&local_ncb->thread_control_rcv, &routine_control_rcv, NULL);
    posix__pthread_create(&local_ncb->thread_image_handler, &routine_handler, local_ncb);
    posix__pthread_create(&local_ncb->thread_rcv, &routine_rcv, NULL);

    log__save("libgzcamera", kLogLevel_Info, kLogTarget_Filesystem, "libgzcamera startup successful.2017-09-20 8");
    return 0;
}

EXP(int)camera_config_parameters(struct packet_config_parameter *parameter) {
    static const int camera_addr_len = sizeof (camera_control_addr);
    uint32_t *calc_checksum;
    int i;

    if (!parameter || (local_ncb->ctrlfd <= 0) || (0 == parameter->module[0])) {
        return -EINVAL;
    }

    if (local_ncb->config_parameter_status == CONFIG_PARAMETER_STATUS_WAITTING) {
        log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "camera_config_parameters status is waitting.");
        return -EAGAIN;
    }

    posix__atomic_xchange(&local_ncb->config_parameter_status, CONFIG_PARAMETER_STATUS_WAITTING);

    // 配置参数的固有字段
    parameter->command = CMD_CFGPAR_ORDER;
    parameter->length = 12;

    // 计算命令参数校验和
    calc_checksum = (uint32_t *) & parameter->output_type;
    parameter->checksum = 0;
    for (i = 0; i < 3; i++) {
        parameter->checksum += *calc_checksum;
        calc_checksum++;
    }
    if (0 == parameter->checksum) {
        parameter->checksum = (uint32_t) (~0);
    }

    // 保存当前的参数配置
	memcpy(&local_ncb->parameter_request, parameter, sizeof(local_ncb->parameter_request));

    // 200ms/次， 5次内， 重试进行参数配置操作
    for (i = 0; i < 5; i++) {
        if (sendto(local_ncb->ctrlfd, (const void *) parameter, sizeof (struct packet_config_parameter), 0, & camera_control_addr, camera_addr_len) < 0) {
			log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "failed to send parameter config packet to target.errno=%d",WSAGetLastError());
            return -1;
        }
        posix__waitfor_waitable_handle(&local_ncb->evt_config_parameter_completed, 200);
        switch (local_ncb->config_parameter_status) {
            case CONFIG_PARAMETER_STATUS_READY:
                log__save("libgzcamera", kLogLevel_Info, kLogTarget_Filesystem, "config parameter successful.");
                return 0;
            case CONFIG_PARAMETER_STATUS_FAULT:
                log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "config parameter fault.");
                return -1;
            case CONFIG_PARAMETER_STATUS_WAITTING:
                break;
            default:
                break;
        }
    }

    log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "camera_config_parameters fault by timeout.");
    return -ETIMEDOUT;
}

EXP(int)camera_control_firmware(uint8_t control_code) {
    struct packet_control_firmware control_command;
    static const int camera_addr_len = sizeof (camera_control_addr);
    int i;

    // socket 必须有效，参数设置必须已经成功完成
    if ((local_ncb->ctrlfd <= 0) || (0 == local_ncb->parameter.checksum) || (0 == local_ncb->parameter.module[0])) {
        return -1;
    }

    if (CONFIG_PARAMETER_STATUS_READY != local_ncb->config_parameter_status) {
        log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "camera_control_firmware, config parameter status is not ready");
        return -1;
    }

	ZeroMemory(&control_command, sizeof(control_command));
    control_command.command = CMD_CTRL_FIRMWARE;
    control_command.length = 1;
    strcpy(control_command.module, local_ncb->parameter.module);
    control_command.checksum = local_ncb->parameter.checksum;
    control_command.data = control_code;

	// 向相机的数据端口投递一个帧完成包，使相机可以重定向本地端口
	for (i = 0; i < 5; i++) {
		ack_frame_completed(0, 0);
		Sleep(1);
	}

	// 1s/次， 5次内， 重试进行硬件控制命令操作， 直到报告状态与命令状态一致
#if INFINITE_LOOP
	for (i = 0;; i++) {
#else
	for (i = 0; i < 120; i++) {
#endif
		if (sendto(local_ncb->ctrlfd, (const void *)&control_command, sizeof(struct packet_control_firmware), 0, &camera_control_addr, camera_addr_len) < 0) {
			return -1;
		}
		posix__waitfor_waitable_handle(&local_ncb->evt_firmware_control_completed, 1000);
		posix__pthread_mutex_lock(&local_ncb->mange_lock);
		if (local_ncb->firmware_status == control_code) {
			posix__pthread_mutex_unlock(&local_ncb->mange_lock);
			log__save("libgzcamera", kLogLevel_Info, kLogTarget_Filesystem, "camera_control_firmware successful.");
			return 0;
		}
		else{
			if (i % 10 == 0) {
				log__save("libgzcamera", kLogLevel_Info, kLogTarget_Filesystem, "waitting for control camera status response.");
			}
		}
		posix__pthread_mutex_unlock(&local_ncb->mange_lock);
	}

	log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "camera_control_firmware fault by timeout.");
	return -ETIMEDOUT;
}

EXP(uint8_t) camera_query_status() {
    return local_ncb->firmware_status;
}

EXP(void) camera_query_firmware_info(char *name, int *namelen, uint32_t *checksum) {
    posix__pthread_mutex_lock(&local_ncb->mange_lock);
    if (name && namelen) {
        strcpy(name, local_ncb->parameter.module);
        *namelen = strlen(local_ncb->parameter.module);
    }
    if (checksum) {
        *checksum = local_ncb->parameter.checksum;
    }
    posix__pthread_mutex_unlock(&local_ncb->mange_lock);
}

EXP(int) camera_parameter_data(char* parameter_path , struct parameter_data_t parameter_data)
{
	struct parameter_post_t parameter_info;
	ZeroMemory(&parameter_info, sizeof(parameter_info));
	if (read_parameter_info(parameter_path, &parameter_info) < 0){ 
		log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "failed to read parameter file.");
		return -1; }
	static const int camera_addr_len = sizeof(camera_control_addr);
	int i;

	// socket 必须有效，参数设置必须已经成功完成
	if ((local_ncb->ctrlfd <= 0) || (0 == local_ncb->para_meter.model_[0])) {
		return -1;
	}
	if (local_ncb->parameter_status == CONFIG_PARAMETER_STATUS_WAITTING) {
		log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "camera_config_parameters status is waitting.");
		return -EAGAIN;
	}

	posix__atomic_xchange(&local_ncb->parameter_status, CONFIG_PARAMETER_STATUS_WAITTING);

	// 2s/次， 5次内， 重试进行硬件控制命令操作， 直到报告状态与命令状态一致
	for (i = 0; i < 5; i++) {
		if (sendto(local_ncb->ctrlfd, (const void *)&parameter_info, sizeof(struct parameter_post_t), 0, &camera_control_addr, camera_addr_len) < 0) {
			return -1;
		}
		posix__waitfor_waitable_handle(&local_ncb->evt_parameter_completed, 1000);
		switch (local_ncb->parameter_status) {
		case CONFIG_PARAMETER_STATUS_READY:
			log__save("libgzcamera", kLogLevel_Info, kLogTarget_Filesystem, "parameter successful.");
			return 0;
		case CONFIG_PARAMETER_STATUS_FAULT:
			log__save("libgzcamera", kLogLevel_Error, kLogTarget_Filesystem, "parameter fault.");
			return -1;
		case CONFIG_PARAMETER_STATUS_WAITTING:
			break;
		default:
			break;
		}
	}

	memcpy_s(&parameter_data.FocalLength_1, sizeof(parameter_data.FocalLength_1), parameter_info.parameter_data_, sizeof(parameter_data.FocalLength_1));
	memcpy_s(&parameter_data.FocalLength_2, sizeof(parameter_data.FocalLength_2), &parameter_info.parameter_data_[8], sizeof(parameter_data.FocalLength_2));
	memcpy_s(&parameter_data.PrincipalPoint_1, sizeof(parameter_data.PrincipalPoint_1), &parameter_info.parameter_data_[16], sizeof(parameter_data.PrincipalPoint_1));
	memcpy_s(&parameter_data.PrincipalPoint_2, sizeof(parameter_data.PrincipalPoint_2), &parameter_info.parameter_data_[24], sizeof(parameter_data.PrincipalPoint_2));
	memcpy_s(&parameter_data.RadialDistortion_1, sizeof(parameter_data.RadialDistortion_1), &parameter_info.parameter_data_[32], sizeof(parameter_data.RadialDistortion_1));
	memcpy_s(&parameter_data.RadialDistortion_2, sizeof(parameter_data.RadialDistortion_2), &parameter_info.parameter_data_[40], sizeof(parameter_data.RadialDistortion_2));
	memcpy_s(&parameter_data.RadialDistortion_3, sizeof(parameter_data.RadialDistortion_3), &parameter_info.parameter_data_[48], sizeof(parameter_data.RadialDistortion_3));
	memcpy_s(&parameter_data.TangentialDistortion_1, sizeof(parameter_data.TangentialDistortion_1), &parameter_info.parameter_data_[56], sizeof(parameter_data.TangentialDistortion_1));
	memcpy_s(&parameter_data.TangentialDistortion_2, sizeof(parameter_data.TangentialDistortion_2), &parameter_info.parameter_data_[64], sizeof(parameter_data.TangentialDistortion_2));
	return 0;
}

int read_parameter_info(char* parameter_path, struct parameter_post_t* parameter_post)
{
	//struct parameter_post_t parameter_post;
	parameter_post->comm_code_ = CMD_CAMREA_PARAMETER;
	parameter_post->length_ = 72;
	memcpy(parameter_post->model_, "IPC36M", 32);
	if (!parameter_path)
	{
		return -1;
	}
	FILE *fp = NULL;
	if (fopen_s(&fp, parameter_path, "r") != 0){
		return -1;
	}
	long size;
	struct parameter_data_t __parameter_data;

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
	memcpy(&parameter_post->parameter_data_[0], &__parameter_data.FocalLength_1, 8);
	memcpy(&parameter_post->parameter_data_[8], &__parameter_data.FocalLength_2, 8);
	memcpy(&parameter_post->parameter_data_[16], &__parameter_data.PrincipalPoint_1, 8);
	memcpy(&parameter_post->parameter_data_[24], &__parameter_data.PrincipalPoint_2, 8);
	memcpy(&parameter_post->parameter_data_[32], &__parameter_data.RadialDistortion_1, 8);
	memcpy(&parameter_post->parameter_data_[40], &__parameter_data.RadialDistortion_2, 8);
	memcpy(&parameter_post->parameter_data_[48], &__parameter_data.RadialDistortion_3, 8);
	memcpy(&parameter_post->parameter_data_[56], &__parameter_data.TangentialDistortion_1, 8);
	memcpy(&parameter_post->parameter_data_[64], &__parameter_data.TangentialDistortion_2, 8);

	return 0;
}