#include "gesn.h"
#include "nis.h"

#include "posix_thread.h"
#include "posix_ifos.h"
#include "posix_string.h"
#include "posix_atomic.h"
#include "posix_wait.h"
#include "posix_time.h"

#include "tst.h"
#include "clist.h"
#include "nsp.h"
#include "logger.h"

enum logessn_tasktype {
    kLogessnTaskType_Connect = 0,
    kLogessnTaskType_Publish,
    kLogessnTaskType_Destroy,
};

enum logessn_connection_status {
    kLogessnConnStatus_NoConnected = 0,
    kLogessnConnStatus_Connecting,
    kLogessnConnStatus_Established,
};

struct proto_getp {
    uint32_t cbstr;
    char context[0];
};

struct logessn_task_node {
    struct list_head link;
    int task_type;
    int cb;
    char *context;
};

struct simple_endpoint {
    char ipaddr[16];
    uint16_t port;
};

static struct {
    HTCPLINK client;
    int status;
    posix__pthread_t thread;
    posix__pthread_mutex_t lock;
    posix__waitable_handle_t waiter;
    struct list_head head;
    int list_size;
    posix__boolean_t exit;
} logessn_config;

static int inited = 0;

static void STDCALL gesn__callback(const nis_event_t *naio_event, const void *pParam2) {
    tcp_data_t *tcp_data = (tcp_data_t *) pParam2;

    switch (naio_event->Event) {
        case EVT_CLOSED:
            posix__atomic_xchange(&logessn_config.client, INVALID_HTCPLINK);
            posix__atomic_xchange(&logessn_config.status, kLogessnConnStatus_NoConnected);
            break;
        case EVT_RECEIVEDATA:
            log__save("motion_template", kLogLevel_Warning, kLogTarget_Stdout | kLogTarget_Filesystem, "gesn message:%s", tcp_data->e.Packet.Data + 4);
            break;
        default:
            break;
    }
}

static int gesn__add_task(struct logessn_task_node *task) {
    static const int MAXIMUM_LIST_SIZE = 100;
    posix__pthread_mutex_lock(&logessn_config.lock);
    if (logessn_config.list_size == MAXIMUM_LIST_SIZE) {
        posix__pthread_mutex_unlock(&logessn_config.lock);
        if (task) {
            if (task->context) {
                free(task->context);
            }
            free(task);
        }
        return -EFBIG;
    } else {
        list_add_tail(&task->link, &logessn_config.head);
        ++logessn_config.list_size;
        posix__pthread_mutex_unlock(&logessn_config.lock);
        posix__sig_waitable_handle(&logessn_config.waiter);
        return 0;
    }
}

static void on_connect(struct logessn_task_node *task) {
    struct simple_endpoint *target;
    int retval;

    if (!task) {
        return;
    }

    if (!task->context) {
        free(task);
        return;
    }

    do {
        if (task->cb != sizeof (struct simple_endpoint)) {
            break;
        }

        target = (struct simple_endpoint *) task->context;

        /* 检查创建 */
        if (logessn_config.client == INVALID_HTCPLINK) {
            logessn_config.client = tcp_create(&gesn__callback, NULL, 0);
            if (INVALID_HTCPLINK == logessn_config.client) {
                break;
            }
        }

        /* 开始连接 */
        posix__atomic_xchange(&logessn_config.status, kLogessnConnStatus_Connecting);
        retval = tcp_connect(logessn_config.client, target->ipaddr, target->port);
        if (retval < 0) {
            posix__atomic_xchange(&logessn_config.status, kLogessnConnStatus_NoConnected);
            tcp_destroy(logessn_config.client);
            break;
        } else {
            /* 指定 TCP 下层协议模板 */
            tst_t tst;
            tst.parser_ = &nsp__tst_parser;
            tst.builder_ = &nsp__tst_builder;
            tst.cb_ = sizeof ( nsp__tst_head_t);
            tcp_settst(logessn_config.client, &tst);
        }
        posix__atomic_xchange(&logessn_config.status, kLogessnConnStatus_Established);
    } while (0);

    free(task->context);
    free(task);
}

static void on_publish(struct logessn_task_node *task) {
    struct proto_getp *getp;
    int cb, command_cb, off;
    static const char op[] = "publish;";
    static const char en[] = ";\r\n;";
    static const int oplen = sizeof (op) - 1; /* 去除杠零后记住实长 */
    static const int enlen = sizeof (en) - 1;

    if (!task) {
        return;
    }

    if (!task->context) {
        return;
    }

    do {
        /* 状态必须是已经连接 */
        if (logessn_config.status != kLogessnConnStatus_Established) {
            break;
        }

        /* 按实际需求上下文长度分配协议包 */
        command_cb = task->cb + oplen + enlen;
        cb = command_cb + sizeof (struct proto_getp);
        getp = (struct proto_getp *) malloc(cb);
        if (!getp) {
            break;
        }
        getp->cbstr = command_cb;

        /* 按事件协议组包 */
        off = 0;
        memcpy(&getp->context[off], op, oplen);
        off += oplen;
        memcpy(&getp->context[off], task->context, task->cb);
        off += task->cb;
        memcpy(&getp->context[off], en, enlen);

        tcp_write(logessn_config.client, cb, &nsp__packet_maker, getp);

    } while (0);

    free(task->context);
    free(task);
}

static void *gesn__worker_prco(void *parameter) {
    int retval;
    struct logessn_task_node *task;

    while (!logessn_config.exit) {
        retval = posix__waitfor_waitable_handle(&logessn_config.waiter, 5000);
        if (retval < 0) {
            break;
        }

        while (!logessn_config.exit) {
            posix__pthread_mutex_lock(&logessn_config.lock);
#if _WIN32
            if (list_empty(&logessn_config.head)) {
                posix__pthread_mutex_unlock(&logessn_config.lock);
                break;
            }
            task = list_first_entry(&logessn_config.head, struct logessn_task_node, link);
#else
            task = list_first_entry_or_null(&logessn_config.head, struct logessn_task_node, link);
            if (!task) {
                posix__pthread_mutex_unlock(&logessn_config.lock);
                break;
            }
#endif
            list_del(&task->link);
            --logessn_config.list_size;
            posix__pthread_mutex_unlock(&logessn_config.lock);

            switch (task->task_type) {
                case kLogessnTaskType_Connect:
                    on_connect(task);
                    break;
                case kLogessnTaskType_Publish:
                    on_publish(task);
                    break;
                case kLogessnTaskType_Destroy:
                    break;
                default:
                    break;
            }
        }
    }
    return NULL;
}

void gesn__stop() {
    struct logessn_task_node *task;
    
    logessn_config.exit = 1;
    posix__sig_waitable_handle(&logessn_config.waiter);
    posix__pthread_join(&logessn_config.thread, NULL);
    
    posix__pthread_mutex_lock(&logessn_config.lock);
    while (!list_empty(&logessn_config.head)) {
        task = list_first_entry(&logessn_config.head, struct logessn_task_node, link);
        if (task) {
            if (task->context) {
                free(task->context);
            }
            free(task);
            list_del(&task->link);
        }
    }
    posix__pthread_mutex_unlock(&logessn_config.lock);
    
    posix__pthread_mutex_release(&logessn_config.lock);
    posix__uninit_waitable_handle(&logessn_config.waiter);
    
    if (logessn_config.client != INVALID_HTCPLINK) {
        tcp_destroy(logessn_config.client);
    }
}

int gesn__config_server(const char *ipv4, uint16_t port) {
    struct logessn_task_node *task;
    struct simple_endpoint *target;

    if (!ipv4 || 0 == port) {
        return -EINVAL;
    }

    if (posix__atomic_inc(&inited) == 1) {
        memset(&logessn_config, 0, sizeof (logessn_config));
        posix__pthread_mutex_init(&logessn_config.lock);
        INIT_LIST_HEAD(&logessn_config.head);
        logessn_config.list_size = 0;
        logessn_config.client = INVALID_HTCPLINK;
        
        /* 直接启动工作线程 */
        posix__init_synchronous_waitable_handle(&logessn_config.waiter); 
        posix__pthread_create(&logessn_config.thread, &gesn__worker_prco, NULL);
    } else {
        posix__atomic_dec(&inited);
    }

    /* 投递一个改变连接的任务 */
    task = (struct logessn_task_node *) malloc(sizeof (struct logessn_task_node));
    if (!task) {
        return -ENOMEM;
    }
    task->task_type = kLogessnTaskType_Connect;
    task->cb = sizeof (struct simple_endpoint);
    task->context = (char *) malloc(sizeof (struct simple_endpoint));
    if (!task->context) {
        free(task);
        return -ENOMEM;
    }
    target = (struct simple_endpoint *) task->context;
    posix__strcpy(target->ipaddr, 16, ipv4);
    target->port = port;
    return gesn__add_task(task);
}

int gesn__publish(const char *context, int size) {
    struct logessn_task_node *task;

    /* 状态必须是已经连接 */
    if ((0 == inited) || !context || (logessn_config.status != kLogessnConnStatus_Established)) {
        return -EINVAL;
    }

    task = (struct logessn_task_node *) malloc(sizeof (struct logessn_task_node));
    if (!task) {
        return -ENOMEM;
    }
    task->task_type = kLogessnTaskType_Publish;
    if (size <= 0) {
        task->cb = strlen(context);
    } else {
        task->cb = size;
    }
    task->context = (char *) malloc(task->cb);
    if (!task->context) {
        free(task);
        return -ENOMEM;
    }
    memcpy(task->context, context, task->cb);
    return gesn__add_task(task);
}

int gesn__write_event(const char *module, enum log__levels level, uint32_t code, const char *format, ...) {
    va_list ap;
    char command[2048];
    char logstr[2048];
    int pos;

    if (!format) {
        return -EINVAL;
    }
    
    static const char *LOG__LEVEL_TXT[] = {
        "info",
        "warning",
        "error",
        "fatal",
        "trace",
    };

    pos = 0;
    if (module) {
        pos += posix__sprintf(&command[pos], cchof(command) - pos, "channel=%s;", module);
    }
#if _WIN32
    pos += posix__sprintf(&command[pos], cchof(command) - pos, "atime=%I64u;", posix__clock_epoch());
#else
    pos += posix__sprintf(&command[pos], cchof(command) - pos, "atime=%llu;", posix__clock_epoch());
#endif
    pos += posix__sprintf(&command[pos], cchof(command) - pos, "level=%s;", LOG__LEVEL_TXT[level]);
    if (code > 0) {
        pos += posix__sprintf(&command[pos], cchof(command) - pos, "code=%u;", code);
    }
    va_start(ap, format);
    posix__vsnprintf(&logstr[0], cchof(logstr), format, ap);
    va_end(ap);
    pos += posix__sprintf(&command[pos], cchof(command) - pos, "%s", logstr);

    log__save(module, level, kLogTarget_Filesystem | kLogTarget_Stdout, "%s", logstr);
    return gesn__publish(command, pos);
}