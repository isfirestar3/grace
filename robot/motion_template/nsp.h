#if !defined NSP_NETWORK_HEAD
#define NSP_NETWORK_HEAD

#include "compiler.h"
#include "nis.h"
#include "vartypes.h"

extern
int nsp__init_network();
extern
void nsp__uninit_network();

extern
void nsp__udpsrv_endpoint(uint32_t *local_ipv4, uint16_t *local_port);
extern
void nsp__tcpsrv_endpoint(uint32_t *local_ipv4, uint16_t *local_port);

extern
int STD_CALL nsp__packet_maker(void *data, int cb, void *context);

extern
int nsp__on_login(HTCPLINK link,
        enum nsp__controlor_type_t type,
        int access,
        const unsigned char *login_buffer,
        int login_buffer_size,
        const unsigned char *login_md5);

extern int nsp__check_link_access_tcp(HTCPLINK link);
extern
enum nsp__controlor_type_t nsp__get_link_type(HTCPLINK link);

extern
void nsp__refurbish_keepalive_status(HTCPLINK lnk);

extern
int nsp__report_status(uint64_t task_id, 
        int vartype,
        enum status_describe_t status, 
        const void *usrdat,
        int usrdatcb);

extern
int nsp__write_log(enum nsp__controlor_type_t identity,
        const char *logstr, 
        uint32_t length);

extern
void nsp__canio_msg(void *functional_object);

#endif