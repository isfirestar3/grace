#ifndef GZFTS_API_H_20170804
#define GZFTS_API_H_20170804

#ifdef _USRDLL
#define METH dllexport
#else
#define METH dllimport
#endif

#include <stdint.h>

#ifndef FTS_EXP

//传输状态
enum transmit_status{
	transmission_timeout = -100,	//传输超时
	transmission_interruption = -1,	//传输中断
	transmission_complete = 0,		//传输完成
	transmiting,					//传输进行
	transmission_wait				//等待传输
};

#define FILE_BLOCK_SIZE				0x00010000	//64kb
#define TRANSFER_WINDOW_SIZE		1			//默认窗口大小
#define TIMEOUT_TRANSFER			5000		//默认链接超时时间5000毫秒


#pragma pack(push ,1)
#if _WIN32
#define FTS_EXP(type)  extern "C" _declspec(METH) type __stdcall
struct fts_parameter {
	uint32_t			block_size_pre_transfer = FILE_BLOCK_SIZE;
	uint32_t			timeout_cancel_transfer = TIMEOUT_TRANSFER;
	uint32_t			maximum_iops_pre_link;
	uint32_t			maximum_iops_global;
	uint32_t			memory_cache_size_pre_link;
	uint32_t			memory_cache_size_global;
	char				file_search_root[300];
	uint32_t 			window_size = TRANSFER_WINDOW_SIZE;
	void *				user_data;
	int					user_data_size;

	int(__stdcall *fts_callback)(const char *ipaddr, uint16_t port,
		const char *path_local, const char *path_remote,
		int status, int error, int step,
		void *user_data, int user_data_size);
};

#else
#define FTS_EXP(type)  extern "C" type
struct fts_parameter {
	uint32_t			block_size_pre_transfer;
	uint32_t			timeout_cancel_transfer;
	uint32_t			maximum_iops_pre_link;
	uint32_t			maximum_iops_global;
	uint32_t			memory_cache_size_pre_link;
	uint32_t			memory_cache_size_global;
	char				file_search_root[300];
	uint32_t 			window_size;
	void *				user_data;
	int					user_data_size;
	int(*fts_callback)(const char *ipaddr, uint16_t port,
		const char *path_local, const char *path_remote,
		int status, int error, int step,
		void *user_data, int user_data_size);
};
#endif
#pragma pack(pop)


#endif

FTS_EXP(int) fts_create_receiver(const char *ipaddr_local, uint16_t port_local, const bool is_long_lnk = false);
FTS_EXP(void) fts_destory_receiver(const char *ipaddr_local, uint16_t port_local, const bool is_long_lnk = false);
FTS_EXP(int) fts_change_configure(const struct fts_parameter *param);
FTS_EXP(int) fts_listdir(const char *ipaddr_to, uint16_t port_to, const char*input_catalog, char **dir, int *dircbs);
FTS_EXP(void) fts_freedir(char *dir);

FTS_EXP(int) fts_pull(const char *ipaddr_from, uint16_t port_from, const char *path_local, const char *path_remote, int overwrite);
FTS_EXP(int) fts_push(const char *ipaddr_to, uint16_t port_to, const char *path_local, const char *path_remote, int overwrite);
FTS_EXP(void) fts_cancel(const char *ipaddr, uint16_t port, const bool is_long_lnk = false);

//同步接口
FTS_EXP(int) fts_delete_file(const char* ipaddr_to, uint16_t port_to, const char*input_files);
FTS_EXP(int) fts_catalog_info(const char* ipaddr_to, uint16_t port_to, const char*input_catalog, char **output_info, int *output_cb);

//支持长链接接口
FTS_EXP(int) fts_pull_long(const char *ipaddr_from, uint16_t port_from, const char *path_local, const char *path_remote, int overwrite);

#endif