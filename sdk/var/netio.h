#if !defined NETIO_H
#define NETIO_H

#if defined __cplusplus
#define STD_C_FORMAT extern "C"
#else
#define STD_C_FORMAT
#endif

#if !defined STD_CALL
#if WIN32
#define STD_CALL __stdcall
#else
#define STD_CALL
#endif
#endif

#if !defined interface_format
#define interface_format(_Ty) STD_C_FORMAT _Ty STD_CALL
#endif

#include "var.h"

#if defined __cplusplus
extern "C" {
#endif

    interface_format(int) netdev__create(var__functional_object_t *object);
    interface_format(int) netdev__uinit();

    typedef int( STD_CALL *netdev__create_t)(var__functional_object_t *object);
    typedef int( STD_CALL *netdev__uninit_t)();

#if defined __cplusplus
}
#endif

#endif