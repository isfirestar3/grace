#if !defined CANIO_H
#define CANIO_H

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

#if !defined CML_TYPES
#define CML_TYPES
typedef void *CML_CanOpenPtr;
typedef void *CML_CanInterfacePtr;
#endif

#if defined __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

    typedef struct _canbus_argv {
        int port_;
        int type_;
    } canio__busarg_t;

#pragma pack(pop)

#define kCanDrvType_Unknown (0)
#define kCanDrvType_Kvaser (1)
#define kCanDrvType_CanCard (2)
#define kCanDrvType_IxxatV3 (3)

    interface_format(int) canio__create_bus(const canio__busarg_t * args, int baud);
    interface_format(int) canio__create_node(const canio__busarg_t * args, const void *vars, int cnt);
    interface_format(int) canio__rw(const canio__busarg_t *args, void **vars, int cnt);
    interface_format(int) canio__register_event_callback(void(*)(void *functional_object));
    interface_format(int) canio__set_error_ptr(void *functional_object);

    typedef int( STD_CALL *canio__create_canbus_t)(const canio__busarg_t *, int);
    typedef int( STD_CALL *canio__create_node_t)(const canio__busarg_t *, const void *, int);
    typedef int( STD_CALL *canio__rw_t)(const canio__busarg_t *, void **, int);
    typedef int( STD_CALL *canio__register_event_callback_t)(void( *)(void *functional_object));
    typedef int( STD_CALL *canio__set_error_ptr_t)(void *);
#if defined __cplusplus
}
#endif

#if !defined DRIVE_IO_INTERVAL
#define DRIVE_IO_INTERVAL  (15)
#endif

#endif