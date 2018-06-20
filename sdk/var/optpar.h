#if !defined OPERATION_PARAMETER_H
#define OPERATION_PARAMETER_H

#include "vartypes.h"

#pragma pack(push, 1)

typedef struct {
    uint64_t ull00_;
    uint64_t ull01_;
    uint64_t ull02_;
    uint64_t ull03_;
    uint64_t ull04_;
    uint64_t ull05_;
    uint64_t ull06_;
    uint64_t ull07_;
    uint64_t ull08_;
    uint64_t ull09_;
    uint64_t ull10_;
    uint64_t ull11_;
    uint64_t ull12_;
    uint64_t ull13_;
    uint64_t ull14_;
    uint64_t ull15_;
    uint64_t ull16_;
    uint64_t ull17_;
    uint64_t ull18_;
    uint64_t ull19_;
} var__operation_parameter_t;

#pragma pack(pop)

extern
int var__load_optpar();

extern
var__operation_parameter_t *var__get_optpar();

#endif // OPERATION_TARGET_H