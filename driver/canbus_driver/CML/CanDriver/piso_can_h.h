
#if 0
#ifndef EXPORTS
//#ifdef  __cplusplus
//	#define EXPORTS extern "C" __declspec (dllimport)
//#else
	#define EXPORTS __declspec (dllimport)
//#endif 
#endif

#endif

#ifndef __PISO_CAN_HEADER__
#define __PISO_CAN_HEADER__

#define PISO_EXPORTS  

#if 0
#ifndef EXPORTS
//#ifdef  __cplusplus
//	#define EXPORTS extern "C" __declspec (dllimport)
//#else
	#define PISO_EXPORTS __declspec (dllimport)
	//#define EXPORTS __declspec (dllimport)
//#endif 
#else
	#define PISO_EXPORTS EXPORTS
#endif
#endif


//====================
#define CAN_NoError                    0
#define CAN_DriverError                1
#define CAN_ActiveBoardError           2
#define CAN_BoardNumberError           3
#define CAN_PortNumberError            4 
#define CAN_ResetError                 5
#define CAN_SoftResetError             6
#define CAN_InitError                  7       
#define CAN_ConfigError                8
#define CAN_SetACRError                9
#define CAN_SetAMRError                10
#define CAN_SetBaudRateError           11

#define CAN_EnableRxIrqFailure         12
#define CAN_DisableRxIrqFailure        13
#define CAN_InstallIrqFailure          14 
#define CAN_RemoveIrqFailure           15

#define CAN_TransmitBufferLocked       16
#define CAN_TransmitIncomplete         17
#define CAN_ReceiveBufferEmpty         18
#define CAN_DataOverrun                19
#define CAN_ReceiveError               20
#define CAN_SoftBufferIsEmpty          21
#define CAN_SoftBufferIsFull           22
#include <windows.h>
#define CAN_TimeOut                    23

#define CAN_InstallIsrError			   24

typedef struct config
{
	unsigned char AccCode[4];
	unsigned char AccMask[4];
	unsigned char BaudRate;
	unsigned char BT0,BT1;
} ConfigStruct;

//BaudRate 0:user define 1=10K   2=20K   3=50K  4=125K  5=250K  6=500K 7=800K  8=1Mbps

typedef struct packet
{	
	LONGLONG MsgTimeStamps;
	unsigned char  mode;
	unsigned long id;
	unsigned char  rtr;
	unsigned char  len;
	unsigned char  data[8];
} PacketStruct;

//mode 0 -> 11 bits ID    1 -> 29 bits ID 

PISO_EXPORTS unsigned short CALLBACK CAN_GetDllVersion();
PISO_EXPORTS int CALLBACK CAN_TotalBoard();
PISO_EXPORTS int CALLBACK CAN_GetBoardInf(unsigned char BoardNo, unsigned long *dwVID, unsigned long *dwDID,
									unsigned long *dwSVID, unsigned long *dwSDID,unsigned long *dwSAuxID,
						            unsigned long* dwIrqNo);
PISO_EXPORTS int CALLBACK CAN_GetCardPortNum(unsigned char BoardNo, unsigned char *bGetPortNum);

PISO_EXPORTS int CALLBACK CAN_ActiveBoard(unsigned char BoardNo);
PISO_EXPORTS int CALLBACK CAN_CloseBoard(unsigned char BoardNo);
PISO_EXPORTS int CALLBACK CAN_BoardIsActive(unsigned char BoardNo);

PISO_EXPORTS int CALLBACK CAN_Reset(unsigned char BoardNo,unsigned char Port);
PISO_EXPORTS int CALLBACK CAN_Init(unsigned char BoardNo,unsigned char Port);
PISO_EXPORTS int CALLBACK CAN_Config(unsigned char BoardNo, unsigned char Port,ConfigStruct *CanConfig);
PISO_EXPORTS int CALLBACK CAN_ConfigWithoutStruct(unsigned char BoardNo, unsigned char Port, unsigned long AccCode, unsigned long AccMask, unsigned char BaudRate, unsigned char BT0, unsigned char BT1);

PISO_EXPORTS int CALLBACK CAN_EnableRxIrq(unsigned char BoardNo,unsigned char Port);
PISO_EXPORTS int CALLBACK CAN_DisableRxIrq(unsigned char BoardNo,unsigned char Port);
PISO_EXPORTS int CALLBACK CAN_RxIrqStatus(unsigned char BoardNo,unsigned char Port,unsigned char *bStatus);
PISO_EXPORTS int CALLBACK CAN_InstallIrq(unsigned char BoardNo);
PISO_EXPORTS int CALLBACK CAN_RemoveIrq(unsigned char BoardNo);
PISO_EXPORTS int CALLBACK CAN_IrqStatus(unsigned char BoardNo,unsigned char *bStatus);

PISO_EXPORTS int CALLBACK CAN_Status(unsigned char BoardNo, unsigned char Port,unsigned char *bStatus);

PISO_EXPORTS int CALLBACK CAN_SendMsg(unsigned char BoardNo, unsigned char Port, PacketStruct *CanPacket);
PISO_EXPORTS int CALLBACK CAN_SendWithoutStruct(unsigned char BoardNo, unsigned char Port, unsigned char Mode, unsigned long Id, unsigned char Rtr, unsigned char Dlen, unsigned char *Data);

PISO_EXPORTS int CALLBACK CAN_RxMsgCount(unsigned char BoardNo, unsigned char Port);
PISO_EXPORTS int CALLBACK CAN_ReceiveMsg(unsigned char BoardNo, unsigned char Port,PacketStruct *CanPacket);
PISO_EXPORTS int CALLBACK CAN_ReceiveWithoutStruct(unsigned char BoardNo, unsigned char Port, unsigned char *Mode, unsigned long *Id, unsigned char *Rtr, unsigned char *Dlen, unsigned char *Data, unsigned long *H_MsgTimeStamps ,unsigned long *L_MsgTimeStamps);

PISO_EXPORTS int CALLBACK CAN_ClearSoftBuffer(unsigned char BoardNo, unsigned char Port);
PISO_EXPORTS int CALLBACK CAN_ClearDataOverrun(unsigned char BoardNo, unsigned char Port);

PISO_EXPORTS void CALLBACK CAN_OutputByte(unsigned char BoardNo,unsigned char Port,unsigned short wOffset,unsigned char bValue);
PISO_EXPORTS unsigned char CALLBACK CAN_InputByte(unsigned char BoardNo,unsigned char Port,unsigned short wOffset);
PISO_EXPORTS LONGLONG CALLBACK CAN_GetSystemFreq(void);

PISO_EXPORTS int CALLBACK CAN_InstallUserIsr(unsigned char BoardNo, void (*UserISR)(unsigned char BoardNo));
PISO_EXPORTS int CALLBACK CAN_RemoveUserIsr(unsigned char BoardNo);

#endif // end define __PISO_CAN_HEADER__

