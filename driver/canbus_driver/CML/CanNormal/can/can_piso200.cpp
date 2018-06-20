
#ifdef _WIN32
#include <windows.h>

#endif
#include "can/can_piso200.h"

#ifndef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <libgen.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>

#include <sstream>
#include <stdio.h>

#ifdef WIN32
#  include <windows.h>
#  include <winioctl.h>
#endif

#include "CML.h"

CML_NAMESPACE_USE();

#ifdef WIN32
typedef int32 int32_t;
typedef uint32 uint32_t;
typedef uint8 uint8_t;
#else
#  include <stdint.h>
#endif

#include "copley_internal.h"

using namespace std;

#define PF_CAN 29

PisoCan200::PisoCan200(void) :
CanInterface() {
	baud = COPLEYCAN_BITRATE_1000000;
	open = 0;
}

PisoCan200::PisoCan200(const char *port) :
CanInterface(port) {
	baud = COPLEYCAN_BITRATE_1000000;
	open = 0;
}

PisoCan200::~PisoCan200(void) {
	Close();
}

const Error *PisoCan200::Open(void) {

	const Error *err;
	struct can_filter rfilter;
	struct sockaddr_can addr;
	struct ifreq ifr;

	printf("try to open pisocan200 : %d\n", open);

	if (open)
	return &CanError::AlreadyOpen;

	if ((m_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
		return &CanError::BadPortName;
	}

	rfilter.can_id = 0;
	rfilter.can_mask = 0;

	setsockopt(m_socket, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter,
			sizeof(rfilter));

	addr.can_family = PF_CAN;
	strcpy(ifr.ifr_name, portName);
	ioctl(m_socket, SIOCGIFINDEX, &ifr);
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(m_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("bind error");
		return &CanError::Unknown;
	}

	open = 1;

	return 0;
}

const Error *PisoCan200::Close(void) {
	if (!open)
	return 0;

	open = 0;
	close(m_socket);
	return 0;
}

const Error *PisoCan200::SetBaud(int32 b) {

	printf("try to set baud %d\n", open);
	if (open)
	return &CanError::AlreadyOpen;

	stringstream ss;
	ss << "ip link set " << portName << " down";

	system(ss.str().c_str());
	stringstream ss_up;
	switch (b) {
		case 1000000:
		baud = COPLEYCAN_BITRATE_1000000;
		ss_up << "ip link set " << portName << " up type can bitrate 1000000";
		system(ss_up.str().c_str());
		break;
		case 800000:
		baud = COPLEYCAN_BITRATE_800000;
		ss_up << "ip link set " << portName << " up type can bitrate 800000";
		system(ss_up.str().c_str());
		break;
		case 500000:
		ss_up << "ip link set " << portName << " up type can bitrate 500000";
		system(ss_up.str().c_str());
		baud = COPLEYCAN_BITRATE_500000;
		break;
		case 250000:
		ss_up << "ip link set " << portName << " up type can bitrate 250000";
		system(ss_up.str().c_str());
		baud = COPLEYCAN_BITRATE_250000;
		break;
		case 125000:
		ss_up << "ip link set " << portName << " up type can bitrate 125000";
		system(ss_up.str().c_str());
		baud = COPLEYCAN_BITRATE_125000;
		break;
		case 100000:
		ss_up << "ip link set " << portName << " up type can bitrate 100000";
		system(ss_up.str().c_str());
		baud = COPLEYCAN_BITRATE_100000;
		break;
		case 50000:
		ss_up << "ip link set " << portName << " up type can bitrate 50000";
		system(ss_up.str().c_str());
		baud = COPLEYCAN_BITRATE_50000;
		break;
		case 20000:
		ss_up << "ip link set " << portName << " up type can bitrate 20000";
		system(ss_up.str().c_str());
		baud = COPLEYCAN_BITRATE_20000;
		break;

		default:
		return &CanError::BadParam;
	}

	return 0;
}

const Error *PisoCan200::RecvFrame(CanFrame &frame, int32 timeout) {

	if (!open)
	return &CanError::NotOpen;

	int nbytes;
	struct can_frame recv_frame;

	if ((nbytes = read(m_socket, &recv_frame, sizeof(recv_frame))) < 0) {
		return &CanError::Driver;
	}

	if (recv_frame.can_id & CAN_RTR_FLAG) {
		frame.type = CAN_FRAME_REMOTE;
		frame.id = recv_frame.can_id;
		frame.length = 0;
		//printf("remote request");
	} else {
		//printf("Receive CAN Message ID : 0x%x Length : %d\n", recv_frame.can_id,recv_frame.can_dlc);
		frame.type = CAN_FRAME_DATA;
		frame.id = recv_frame.can_id;
		frame.length = recv_frame.can_dlc;
		for (int i = 0; i < recv_frame.can_dlc; i++) {
			frame.data[i] = recv_frame.data[i];
			//printf("Data[%d] : %02X\n", i, recv_frame.data[i]);
		}
		//printf("\n");
	}

	return 0;

}
const Error *PisoCan200::XmitFrame(CanFrame &frame, int32 timeout) {

	if (!open)
	return &CanError::NotOpen;

	if (frame.length > 8)
	return &CanError::BadParam;

	struct can_frame send_frame;

	send_frame.can_id = frame.id;
	send_frame.can_dlc = frame.length;
	if (send_frame.can_id > 0x800)
	send_frame.can_id |= CAN_EFF_FLAG;
	if (frame.type == CAN_FRAME_REMOTE) {
		send_frame.can_id |= CAN_RTR_FLAG;
	}

	for (int i = 0; i < frame.length; i++)
	send_frame.data[i] = frame.data[i];

	write(m_socket, &send_frame, sizeof(send_frame));

#if 0
	printf("send frame : id = %x", send_frame.can_id);
	for (int i = 0; i < send_frame.can_dlc; i++) {

		printf("Data[%d] : %02X\n", i, send_frame.data[i]);
	}
	printf("\n");
#endif

	return 0;
}

#else

CML_NAMESPACE_USE();


#include <stdio.h>

#include "CML.h"

#ifdef WIN32
typedef int32 int32_t;
typedef uint32 uint32_t;
typedef uint8 uint8_t;
#else
#  include <stdint.h>
#endif

#include "copley_internal.h"

using namespace std;

PisoCan200::PisoCan200(void) :
	CanInterface() {
	baud = COPLEYCAN_BITRATE_1000000;
	open = 0;
}

PisoCan200::PisoCan200(const char *port) :
	CanInterface(port) {
	baud = COPLEYCAN_BITRATE_1000000;
	open = 0;
}

PisoCan200::~PisoCan200(void) {
	Close();
}

const Error *PisoCan200::Open(void) {

	printf("try to open pisocan200 : %d\n", open);

	/*if (open)
		return &CanError::AlreadyOpen;

	int port = 0;

	port += 1;

	m_port = port;

	printf("CAN port Number : %d \n", port);

	int ret = CAN_ActiveBoard(0);
	if (ret) {
		return &CanError::Unknown;
	}

	ret = CAN_Reset(0, port);
	if (ret) {
		printf("CAN_Reset Port 1 error!  Code:%d \n", ret);
		return &CanError::Unknown;
	}

	ret = CAN_Init(0, port);
	if (ret) {
		printf("CAN_Init Port 1 error!  Code:%d \n", ret);
		return &CanError::Unknown;
	}

	ret = CAN_Config(0, port, &CanConfig);
	if (ret) {
		return &CanError::Unknown;
	}

	ret = CAN_EnableRxIrq(0, port);
	if (ret) {
		return &CanError::Unknown;
	}

	ret = CAN_InstallIrq(0);
	if (ret) {
		return &CanError::Unknown;
	}

	open = 1;*/

	return 0;
}

const Error *PisoCan200::SetBaud(int32 b) {

	for (int i = 0; i <= 3; i++) {
		CanConfig.AccCode[i] = 0;
		CanConfig.AccMask[i] = 0xff;
	}

	switch (b) {
	case 1000000:
		CanConfig.BaudRate = 8;
		break;
	case 800000:
		CanConfig.BaudRate = 7;
		break;
	case 500000:
		CanConfig.BaudRate = 6;
		break;
	case 250000:
		CanConfig.BaudRate = 5;
		break;
	case 125000:
		CanConfig.BaudRate = 4;
		break;
	case 100000:
		CanConfig.BaudRate = 3;
		break;
	case 50000:
		CanConfig.BaudRate = 2;
		break;
	case 20000:
		CanConfig.BaudRate = 1;
		break;

	default:
		return &CanError::BadParam;
	}

	return NULL;
}

const Error *PisoCan200::RecvFrame(CanFrame &frame, int32 timeout) {

	//int ret ;
	//if (!open)
	//	return &CanError::NotOpen;

	//PacketStruct CANRx;
	//int i;
	//CountTime count_time;
	//count_time.begin();
	//while (1) {
	//	i = CAN_RxMsgCount(0, m_port);
	//	if (i != 0) {
	//		break;
	//	} else {
	//		if (timeout > 0) {
	//			count_time.end();
	//			if (count_time.getTime() > timeout)
	//				return &CanError::Timeout;
	//		}
	//		SLEEP(0.1);
	//	}
	//}

	//ret = CAN_ReceiveMsg(0, m_port, &CANRx);

	//if (ret) {
	//	return &CanError::Driver;
	//}

	//frame.id = CANRx.id;


	//if(CANRx.rtr){
	//	frame.type = CAN_FRAME_REMOTE;
	//	frame.length = 0;
	//}
	//else{
	//	frame.type = CAN_FRAME_DATA;
	//	frame.length = CANRx.len;
	//	for (int i = 0; i < frame.length; i++) {
	//		frame.data[i] = CANRx.data[i];
	//	}
	//}

	////printf(
	////		"Mode:%d  ID:%X  RTR:%d  DLC:%d  Data(hex):%02X %02X %02X %02X %02X %02X %02X %02X  TimeStamp=%f \n",
	////		CANRx.mode, CANRx.id, CANRx.rtr, CANRx.len, CANRx.data[0],
	////		CANRx.data[1], CANRx.data[2], CANRx.data[3], CANRx.data[4],
	////		CANRx.data[5], CANRx.data[6], CANRx.data[7]);

	return 0;

}

const Error *PisoCan200::XmitFrame(CanFrame &frame, int32 timeout) {

	//if (!open)
	//return &CanError::NotOpen;

	//if (frame.length > 8)
	//return &CanError::BadParam;

	//PacketStruct CANTx;

	//CANTx.id = frame.id;

	//if (frame.type == CAN_FRAME_REMOTE){
	//	CANTx.mode = 0;
	//	CANTx.rtr = 1;
	//}
	//else{
	//	CANTx.mode = 0;
	//	CANTx.rtr = 0;
	//}

	//CANTx.len = frame.length;

	//for (int i = 0; i < frame.length; i++)
	//	CANTx.data[i] = frame.data[i];


	//int ret = CAN_SendMsg(0, m_port, &CANTx);

	//if (ret) {
	//	printf("CAN_SendMsg error!  Code:%d \n", ret);
	//	return &CanError::Unknown;
	//}

	return 0;
}

const Error *PisoCan200::Close(void) {
	if (!open)
	return 0;

	open = 0;
	return 0;
}


#endif

