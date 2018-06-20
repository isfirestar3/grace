#include "pkthrd.h"

#include <memory.h>

unsigned char * proto__serialize_head(nsp__packet_head_t *head, unsigned char *bytes) {
    memcpy(bytes, head, sizeof (nsp__packet_head_t));
    return ( bytes + sizeof ( nsp__packet_head_t));
}

const unsigned char * proto__build_head(nsp__packet_head_t *head, const unsigned char *data) {
    memcpy(head, data, sizeof ( nsp__packet_head_t));
    return ( data + sizeof ( nsp__packet_head_t));
}

int proto__head_length() {
    return sizeof ( uint32_t) * 6;
}