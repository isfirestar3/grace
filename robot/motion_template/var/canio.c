#include "canio.h"
#include "logger.h"
#include "args.h"

#include "posix_atomic.h"
#include "posix_string.h"
#include "posix_ifos.h"

#include "version.h"

/* canbus_driver 的链接文件，该文件可以通过  [-l | --link] 在启动阶段进行变更 */
static char __file[RUN_FILENAME_LENGTH] = {0};

static
void *canio__getdll() {
#if !_WIN32
    return NULL;
#endif
    static char path[MAXPATH] = {0};
    void *this_dll;
    static void *curr_dll = NULL;

    if (curr_dll) {
        return curr_dll;
    }

    if (0 == __file[0]) {
        run__getarg_canio_driver(__file);
    }

    /* 执行安装包后， /usr/local/lib/canbus_driver.so /usr/local/lib/nshost.so  都是符号链接
     * 执行安装包后， /usr/local/lib/canbus_driver.so /usr/local/lib/nshost.so  都是符号链接
     * 执行安装包后， /usr/local/lib/canbus_driver.so /usr/local/lib/nshost.so  都是符号链接
     * 重要的话说三遍
     */
#if _WIN32
    posix__sprintf(path, cchof(path), "%s"POSIX__DIR_SYMBOL_STR"%s", posix__getpedir(), __file);
#else
    if (sizeof(uint32_t)== sizeof(long)){
        posix__sprintf(path, cchof(path), "/usr/local/lib/%s", __file);
    }else if (sizeof(uint64_t) == sizeof(long)){
        posix__sprintf(path, cchof(path), "/usr/local/lib64/%s", __file);
    }else{
        return NULL;
    }
#endif

    this_dll = posix__dlopen(path);
    if (!this_dll) {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                "failed to open shared object %s, error message:%d", path, errno); //posix__strerror());
    }
    if (this_dll && !curr_dll && (this_dll != curr_dll)) {
        log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                "exchange canbus_driver shared object from 0x%08X to 0x%08X ", curr_dll, this_dll);

        curr_dll = this_dll;
    }

    return curr_dll;
}

int STD_CALL canio__rw(const canio__busarg_t *args, void **vars, int cnt) {
#if !_WIN32
    return -1;
#endif
    void *handle;
    static void *sym = NULL;

    handle = canio__getdll();
    if (!handle) {
        return -1;
    }

    if (!sym) {
        sym = posix__dlsym(handle, "canio__rw");
    }

    if (sym) {
        return ( (canio__rw_t) sym)(args, vars, cnt);
    }
    return -1;
}

int STD_CALL canio__create_bus(const canio__busarg_t * args, int baud) {
#if !_WIN32
    return -1;
#endif
    void *handle;
    static void *sym = NULL;

    handle = canio__getdll();
    if (!handle) {
        return -1;
    }

    if (!sym) {
        sym = posix__dlsym(handle, "canio__create_bus");
    }

    if (sym) {
        return ( (canio__create_canbus_t) sym)(args, baud);
    }
    return -1;
}

int STD_CALL canio__create_node(const canio__busarg_t * args, const void *vars, int cnt) {
#if !_WIN32
    return -1;
#endif
    
    void *handle;
    static void *sym = NULL;

    handle = canio__getdll();
    if (!handle) {
        return -1;
    }

    if (!sym) {
        sym = posix__dlsym(handle, "canio__create_node");
    }

    if (sym) {
        return ( (canio__create_node_t) sym)(args, vars, cnt);
    }
    return -1;
}

int STD_CALL canio__register_event_callback(void(*canbus_event_callback)(void *functional_object)) {
#if !_WIN32
    return -1;
#endif
    void *handle;
    static void *sym = NULL;

    handle = canio__getdll();
    if (!handle) {
        return -1;
    }

    if (!sym) {
        sym = posix__dlsym(handle, "canio__register_event_callback");
    }

    if (sym) {
        return ( (canio__register_event_callback_t) sym)(canbus_event_callback);
    }
    return -1;
}

int STD_CALL canio__set_error_ptr(void *functional_object) {
#if !_WIN32
    return -1;
#endif
    void *handle;
    static void *sym = NULL;

    handle = canio__getdll();
    if (!handle) {
        return -1;
    }

    if (!sym) {
        sym = posix__dlsym(handle, "canio__set_error_ptr");
    }

    if (sym) {
        return ( (canio__set_error_ptr_t) sym)(functional_object);
    }
    return -1;
}
