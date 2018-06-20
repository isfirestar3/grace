#if !defined PKTHRD_HEAD
#define PKTHRD_HEAD

#include "compiler.h"

#pragma pack(push, 1)

typedef struct {
    uint32_t id_;
    uint32_t type_;
    uint32_t size_;
    int32_t  err_;
    uint32_t fd_;
    uint32_t token_;
} nsp__packet_head_t;

#pragma pack(pop)

extern
unsigned char * nsp__serialize_head(nsp__packet_head_t *head, unsigned char *bytes);
extern
const unsigned char *nsp__build_head(nsp__packet_head_t *head, const unsigned char *data);
extern
int nsp__head_length();

#endif