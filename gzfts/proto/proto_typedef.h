#if !defined PROTO_TYPEDEF_H
#define PROTO_TYPEDEF_H

#define PKTTYPE_ACK							  (0x80000000)

#define PKTYPE_GET_CATALOG_INFO_REQUEST		  (0x00000101)
#define PKTYPE_GET_CATALOG_INFO_REQUEST_ACK		PKTYPE_GET_CATALOG_INFO_REQUEST | PKTTYPE_ACK

#define PKTYPE_PUSH_FILE_HEAD_REQUEST		 (0x00000102)
#define PKTYPE_PUSH_FILE_HEAD_REQUEST_ACK	 PKTYPE_PUSH_FILE_HEAD_REQUEST | PKTTYPE_ACK

#define PKTYPE_PUSH_FILE_BLOCK_REQUEST		 (0x00000103)
#define PKTYPE_PUSH_FILE_BLOCK_REQUEST_ACK	 PKTYPE_PUSH_FILE_BLOCK_REQUEST | PKTTYPE_ACK

#define PKTYPE_READ_FILE_HEAD_REQUEST		(0x00000104)
#define PKTYPE_READ_FILE_HEAD_REQUEST_ACK	 PKTYPE_READ_FILE_HEAD_REQUEST | PKTTYPE_ACK

#define PKTYPE_READ_FILE_BLOCK_REQUEST		(0x00000105)
#define PKTYPE_READ_FILE_BLOCK_REQUEST_ACK	 PKTYPE_READ_FILE_BLOCK_REQUEST | PKTTYPE_ACK

#define PKTYPE_FILE_NXET_BLOCK_REQUEST		(0x00000106)
#define PKTYPE_FILE_NXET_BLOCK_REQUEST_ACK	 PKTYPE_FILE_NXET_BLOCK_REQUEST|PKTTYPE_ACK

//更新文件状态（完成，发生错误等状态）
#define PKTYPE_RW_FILE_STATUS				(0x00000107)
#define PKTYPE_RW_FILE_STATUS_ACK			PKTYPE_RW_FILE_STATUS | PKTTYPE_ACK

#define PKTYPE_DELETE_FILE					(0x00000108)
#define PKTYPE_DELETE_FILE_ACK				PKTYPE_DELETE_FILE | PKTTYPE_ACK

//传输状态
#define PKTYPE_TRANSMIT_STATUS				(0x00000109)
#define PKTYPE_TRANSMIT_STATUS_ACK			(PKTYPE_TRANSMIT_STATUS | PKTTYPE_ACK)

#define PKTYPE_GET_CURRENT_CATALOG_REQUEST	(0x0000010A)
#define PKTYPE_GET_CURRENT_CATALOG_REQUEST_ACK (PKTYPE_GET_CURRENT_CATALOG_REQUEST | PKTTYPE_ACK)

//长连接操作
#define PKTYPE_READ_FILE_LONG_LINK_REQUEST	(0x0000010B)
#define PKTYPE_READ_FILE_LONG_LINK_REQUEST_ACK	(PKTYPE_READ_FILE_LONG_LINK_REQUEST | PKTTYPE_ACK)

#define PKTYPE_READ_FILE_BLOCK_LONG_REQUEST (0x0000010C)
#define PKTYPE_READ_FILE_BLOCK_LONG_REQUEST_ACK	(PKTYPE_READ_FILE_BLOCK_LONG_REQUEST | PKTTYPE_ACK)

//长连接更新文件状态（完成，发生错误等状态）
#define PKTYPE_RW_FILE_LONG_LNK_STATUS		(0x0000010D)
#define PKTYPE_RW_FILE_LONG_LNK_STATUS_ACK	PKTYPE_RW_FILE_LONG_LNK_STATUS | PKTTYPE_ACK

//心跳包，在长连接过程中需要心跳包的存在
#define PKTYPE_LONG_LINK_ALIVE				(0x0000010E)
#define PKTYPE_LONG_LINK_ALIVE_ACK			PKTYPE_LONG_LINK_ALIVE|PKTTYPE_ACK

#endif