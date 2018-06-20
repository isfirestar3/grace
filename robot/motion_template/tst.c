#include "tst.h"

#include <string.h>

#include "posix_atomic.h"
#include "posix_naos.h"

static const unsigned char NSPDEF_OPCODE[4] = {'N', 's', 'p', 'd'};

int STD_CALL nsp__tst_parser(void *dat, int cb, int *pkt_cb) {
    nsp__tst_head_t *head = (nsp__tst_head_t *) dat;

    if (!head) return -1;

    if (0 != memcmp(NSPDEF_OPCODE, &head->op_, sizeof (NSPDEF_OPCODE))) {
        return -1;
    }

    *pkt_cb = head->cb_;
    return 0;
}

int STD_CALL nsp__tst_builder(void *dat, int cb) {
    nsp__tst_head_t *head = (nsp__tst_head_t *) dat;

    if (!dat || cb <= 0) {
        return -1;
    }

    memcpy(&head->op_, NSPDEF_OPCODE, sizeof (NSPDEF_OPCODE));
    head->cb_ = cb;
    return 0;
}

////////////////////////////////////		modbus - TCP		/////////////////////////////////////////////////////////////////////////////
static long __transaction_identifier = 1;

int STD_CALL nsp__modbus_tst_parser( void *dat, int cb, int *pkt_cb ) {
	if ( !dat ) {
		return -1;
	}

	nsp__modbus_tst_head_t *head = ( nsp__modbus_tst_head_t * ) dat;
	*pkt_cb = posix__chord16( head->length_ ) - 1;/* 排除 unit_identifier_ 长度*/
	return 0;
}

int STD_CALL nsp__modbus_tst_builder( void *dat, int cb ) {
	if ( !dat || cb <= 0 ) {
		return -1;
	}

	nsp__modbus_tst_head_t *head = ( nsp__modbus_tst_head_t * ) dat;
	head->transaction_identifier_ = posix__chord16( ( uint16_t ) posix__atomic_inc( &__transaction_identifier ) );
	head->protocol_identifier_ = 0;
	head->unit_identifier_ = 1;
	head->length_ = posix__chord16( ( uint16_t ) ( cb + 1 ) );/*unit_identifier_ 计入后续字节计数*/
	return 0;
}