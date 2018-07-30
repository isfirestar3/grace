#include "pstorage.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "logger.h"
#include "posix_ifos.h"
#include "posix_thread.h"

#if !defined PAGE_SIZE
#define PAGE_SIZE	(4096)
#endif

struct period_storage_data {
	void *mptr;
	uint32_t mlen;
	posix__pthread_mutex_t	mutex;
};

static struct period_storage_data mapped_data = {.mptr = NULL, .mlen = 0 };

int run__load_mapping(uint32_t cb) {
    char path[255];
    uint32_t len;
    struct stat statbuff;
    const char *pedir;
    int retval;
    int fd;
	char *init_mapped_buufer;
	int wrcb;
	int wroffset;
	void *addr;
	
	if (mapped_data.mptr || mapped_data.mlen > 0 ) {
		return -1;
	}

    do {
        fd = -1;
        retval = -1;
		init_mapped_buufer = NULL;

        pedir = posix__getpedir();
        sprintf(path, "%s/tom", pedir);

        fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd < 0) {
            log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "failed to open associated file %s for total odo meter mapping.error=%d", path, errno);
            break;
        }

        if (stat(path, &statbuff) < 0) {
            break;
        }
		
		// 调整长度为页对齐
		if (cb % PAGE_SIZE != 0){
			len = ((cb / PAGE_SIZE) + 1) * PAGE_SIZE;
		}else {
			len = cb;
		}

        // 如果文件不足以存放数据(新建文件)， 则拉伸到可用长度
        if (statbuff.st_size < len) {
			init_mapped_buufer = (char *)malloc(len);
			if (!init_mapped_buufer) {
				break;
			}
			*(double *)init_mapped_buufer = 0.0;
			wroffset = 0;
			wrcb = len;
			while (wrcb > 0){
				retval = write(fd, init_mapped_buufer + wroffset, wrcb);
				if (retval < 0 ) {
					if (errno != EINTR ) {
						break;
					}
					continue;
				}else if ( 0 == retval ) {
					break;
				}
				wrcb -= retval;
				wroffset += retval;
			}
        }

        addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (MAP_FAILED == addr) {
            break;
        }

        close(fd);
        fd = -1;
        retval = 0;
		mapped_data.mptr = addr;
		mapped_data.mlen = len;
		posix__pthread_mutex_init(&mapped_data.mutex);
    } while (0);

    if (fd >= 0) {
        close(fd);
    }
	
	if (init_mapped_buufer) {
		free(init_mapped_buufer);
	}
	
    return retval;
}

void run__release_mapping() {
	if (mapped_data.mptr && mapped_data.mlen > 0 ) {
		munmap(mapped_data.mptr, sizeof (mapped_data.mlen));
		posix__pthread_mutex_uninit(&mapped_data.mutex);
	}
}

int run__write_mapping(uint32_t offset, uint32_t len, const void *data) {
	if (!mapped_data.mptr || (mapped_data.mlen <= 0) || (offset + len > mapped_data.mlen) || !data) {
		return -EINVAL;
	}
	
	posix__pthread_mutex_lock(&mapped_data.mutex);
	memcpy(mapped_data.mptr, data, len);
	posix__pthread_mutex_unlock(&mapped_data.mutex);
	
	return 0;
}

int run__read_mapping(uint32_t offset, uint32_t len, void *data) {
	if (!mapped_data.mptr || (mapped_data.mlen <= 0) || (offset + len > mapped_data.mlen) || !data) {
		return -EINVAL;
	}
	
	posix__pthread_mutex_lock(&mapped_data.mutex);
	memcpy(data, mapped_data.mptr, len);
	posix__pthread_mutex_unlock(&mapped_data.mutex);
	
	return 0;
}









