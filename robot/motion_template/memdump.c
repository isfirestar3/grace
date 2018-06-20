#include "memdump.h"
#include "posix_thread.h"
#include "posix_time.h"
#include "posix_ifos.h"
#include "posix_string.h"
#include "compiler.h"

/* 记录内存dump工作周期为20ms/次， 跟随导航周期 */
#define MEMDUMP_TIME_PERIOD         (20)

/* 记录10分钟 */
#define MEMDUMP_TIME_WINDOW         (10 * 60 * 1000)

/* 10分钟内， 总共记录3万条 */
#define MEMDUMP_COUNT_WINDOW          (MEMDUMP_TIME_WINDOW / MEMDUMP_TIME_PERIOD)

/* 每一个内存dump的节点大小为2KB， 其中 epoch占8字节/导航308/底盘196 */
#define MEMDUMP_NODE_SIZE           (2048)

/* 10分钟内， 总共记录3万条 */
#define MEMDUMP_CACHE_SIZE          (MEMDUMP_COUNT_WINDOW  * MEMDUMP_NODE_SIZE)

struct memdup_node {
    int current_cp_offset;
    unsigned char cache[MEMDUMP_NODE_SIZE];
};

struct memdump_cache {
    int current_index_pointer;
    struct memdup_node boxes[MEMDUMP_COUNT_WINDOW];
    posix__pthread_mutex_t *lock; /* 有利于对齐拷贝 */
    posix__pthread_t thread; /* thread for dump record */
};

struct memdump_record {
    struct memdump_cache *dump;
    struct memdup_node boxes[MEMDUMP_COUNT_WINDOW];
};

int allocate_memory_dump(void **dump_object) {
    struct memdump_cache *object;

    if (!dump_object) {
        return -EINVAL;
    }

    object = (struct memdump_cache *) malloc(sizeof (struct memdump_cache));
    if (!object) {
        return -ENOMEM;
    }
    memset(object, 0, sizeof (struct memdump_cache));

    object->lock = (posix__pthread_mutex_t *) malloc(sizeof (posix__pthread_mutex_t));
    if (!object->lock) {
        free(object);
        return -ENOMEM;
    }
    posix__pthread_mutex_init(object->lock);

    *dump_object = object;
    return 0;
}

void destroy_memory_dump(void *dump_object) {
    if (dump_object) {
        posix__pthread_mutex_release(((struct memdump_cache *) dump_object)->lock);
        free(dump_object);
    }
}

int add_memory_record(void *dump_object, const void *memory_data, int sizecb) {
    struct memdump_cache *dump;
    struct memdup_node *node;
    int retval;
    uint64_t *epoch_time = NULL;

    if (!dump_object || !memory_data || sizecb <= 0) {
        return -EINVAL;
    }

    dump = (struct memdump_cache *) dump_object;

    posix__pthread_mutex_lock(dump->lock);
    do {
        node = &dump->boxes[dump->current_index_pointer];
        epoch_time = (uint64_t *)&node->cache[0];
        
        if (node->current_cp_offset + sizecb > MEMDUMP_NODE_SIZE) {
            retval = -1;
            break;
        }

        if (0 == *epoch_time) {
            *epoch_time = posix__clock_epoch();
        }

        memcpy(&node->cache[node->current_cp_offset], memory_data, sizecb);
        node->current_cp_offset += sizecb;
        retval = 0;
    } while (0);
    posix__pthread_mutex_unlock(dump->lock);

    return retval;
}

void move_memory_record_tonext(void *dump_object) {
    struct memdump_cache *dump;

    if (!dump_object) {
        return;
    }

    dump = (struct memdump_cache *) dump_object;

    posix__pthread_mutex_lock(dump->lock);
    dump->current_index_pointer++;
    if (dump->current_index_pointer == MEMDUMP_COUNT_WINDOW) {
        dump->current_index_pointer = 0;
    }
    
    /* epoch time置0 */
    memset(dump->boxes[dump->current_index_pointer].cache, 0, sizeof(uint64_t));
    /* 拷贝便宜置0 */
    dump->boxes[dump->current_index_pointer].current_cp_offset = 0;
    
    posix__pthread_mutex_unlock(dump->lock);
}

static void *dump_memory_record_proc(void *ptr) {
    struct memdump_record *record;
    int i;
    char file[260];
    posix__systime_t syst;
    int retval;
    uint64_t off = 0;
#if _WIN32
    HANDLE fd;
#else
    int fd;
#endif
    
    record = (struct memdump_record *)ptr;
    if (!record) {
        return NULL;
    }
    
    if (!record->dump) {
        free(record);
        return NULL;
    }
    
    posix__localtime(&syst);
    
#if _WIN32
    posix__sprintf(file, cchof(file), "%s\\%04u%02u%02u_%02u%02u%02u_%04u.mmd", posix__getpedir(),
#else
    posix__sprintf(file, cchof(file), "%s/%04u%02u%02u_%02u%02u%02u_%04u.mmd", posix__getpedir(),
#endif
    syst.year,syst.month,syst.day,syst.hour,syst.minute,syst.second,syst.low / 10000);
    
    do {
        retval = posix__file_create_always(file, &fd);
        if (retval < 0){
            break;
        }

        for (i = 0; i < MEMDUMP_COUNT_WINDOW; i++) {
            posix__write_file((int)fd, (const char *)record->boxes[i].cache, MEMDUMP_NODE_SIZE);
            off += MEMDUMP_NODE_SIZE;
            posix__seek_file_offset((int)fd, off);
        }
        
        posix__close((int)fd);
    }while (0);
    
    posix__pthread_mutex_lock(record->dump->lock);
    record->dump->thread.pid_ = 0;
    posix__pthread_mutex_unlock(record->dump->lock);
    
    free(record);
    return NULL;
}

int dump_memory_record(const void *dump_object) {
    struct memdump_cache *dump;
    struct memdump_record *record;
    int retval;

    if (!dump_object) {
        return -EINVAL;
    }

    dump = (struct memdump_cache *) dump_object;

    /* 复制一份当前的内存记录， 转换线程并记录, 该线程为短线程 */
    posix__pthread_mutex_lock(dump->lock);
    do {
        if (0 != dump->thread.pid_) {
            retval = -EINPROGRESS;
            break;
        }

        record = (struct memdump_record *) malloc(sizeof (struct memdump_record));
        if (!record) {
            retval = -EINVAL;
            break;
        }

        memcpy(record->boxes, dump->boxes, sizeof (dump->boxes));
        record->dump = dump;
        retval = posix__pthread_create(&dump->thread, &dump_memory_record_proc, record);
    } while (0);
    posix__pthread_mutex_unlock(dump->lock);

    return retval;
}