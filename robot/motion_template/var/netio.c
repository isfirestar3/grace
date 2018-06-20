#include "netio.h"
#include "logger.h"

#include "posix_atomic.h"
#include "posix_string.h"
#include "posix_ifos.h"

static
void *netio__getdll() {
    static char path[MAXPATH] = {0};
    void *this_dll;
    static void *curr_dll = NULL;

    if (curr_dll) {
        return curr_dll;
    }

    /* 执行安装包后， /usr/local/lib/netdev_driver.so /usr/local/lib/nshost.so  都是符号链接
     * 执行安装包后， /usr/local/lib/netdev_driver.so /usr/local/lib/nshost.so  都是符号链接
     * 执行安装包后， /usr/local/lib/netdev_driver.so /usr/local/lib/nshost.so  都是符号链接
     * 重要的话说三遍
     */
#if _WIN32
    posix__sprintf(path, cchof(path), "%s\\netdev_driver.dll", posix__getpedir());
#else
    posix__strcpy(path, cchof(path), "/usr/local/lib/netdev_driver.so");
#endif

    this_dll = posix__dlopen(path);
    if (!this_dll) {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                "failed to open shared object %s, error message:%d", path, errno);
    }
    if (this_dll && !curr_dll && (this_dll != curr_dll)) {
        log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                "exchange netdev_driver shared object from 0x%08X to 0x%08X ", curr_dll, this_dll);
        curr_dll = this_dll;
    }

    return curr_dll;
}

int STD_CALL netdev__create(var__functional_object_t *object) {
    void *handle;
    static void *sym = NULL;

    handle = netio__getdll();
    if (!handle) {
        return -1;
    }

    if (!sym) {
        sym = posix__dlsym(handle, "netdev_create");
    }

    if (sym) {
        return ( (netdev__create_t) sym)(object);
    }
    return -1;
}

int STD_CALL netdev__uinit(){
    void *handle;
    static void *sym = NULL;

    handle = netio__getdll();
    if (!handle) {
        return -1;
    }

    if (!sym) {
        sym = posix__dlsym(handle, "netdev_uninit");
    }

    if (sym) {
        return ( (netdev__uninit_t) sym)();
    }
    return -1;
}
