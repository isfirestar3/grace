#if !defined AL_MD5_H
#define AL_MD5_H

#include <stdint.h>

#pragma pack (push, 1)
typedef struct {
    uint32_t state[4];
    uint32_t count[2];
    unsigned char buffer[64];
    unsigned char PADDING[64];
} MD5_CTX;

#pragma pack(pop)

#if !defined MD5_ENCODE_CB
#define MD5_ENCODE_CB   (16)
#endif

void MD5__Init(MD5_CTX *md5ctx);
void MD5__Update(MD5_CTX *md5ctx, const unsigned char *input, uint32_t inputLen);
void MD5__Final(MD5_CTX *md5ctx, unsigned char digest[MD5_ENCODE_CB]);

#endif