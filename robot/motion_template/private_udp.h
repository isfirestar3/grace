#ifndef PRIVATE_UDP_H
#define PRIVATE_UDP_H

#include "version.h"

#if !_WIN32

#include <stdint.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>

typedef int (*private_udp_callback_t)(const unsigned char *packet, int cb);

extern
int private_udp_startup(const char *module_address, private_udp_callback_t callback);
extern
int private_udp_write(const void *buffer, int cb);

#endif /* !_WIN32 */

#endif /* PRIVATE_UDP_H */

