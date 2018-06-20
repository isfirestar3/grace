#if !defined NSPTST_HEAD
#define NSPTST_HEAD

#include "nis.h"

#pragma pack(push, 1)

typedef struct {
    uint32_t op_;
    uint32_t cb_;
} nsp__tst_head_t;

typedef struct {
	uint16_t transaction_identifier_;
	uint16_t protocol_identifier_;
	uint16_t length_;
	uint8_t	 unit_identifier_;
}nsp__modbus_tst_head_t;

#pragma pack(pop)

int STD_CALL nsp__tst_parser(void *dat, int cb, int *pkt_cb);
int STD_CALL nsp__tst_builder(void *dat, int cb);
int STD_CALL nsp__modbus_tst_parser( void *dat, int cb, int *pkt_cb );
int STD_CALL nsp__modbus_tst_builder( void *dat, int cb );

#endif