#pragma once

#include <string>
#include "asio_manage.h"
#include <map>
#include <deque>

struct file_callabck_data{
	std::string ipaddr;
	uint16_t port;
	std::string path_local;
	std::string path_remote;
	int status = -1;
	int error = 0;
	int step = 0;
	uint64_t total_size = 0;
};

namespace nsp{
	namespace file{
		struct file_head{
			std::string path_remote;	//目标主机文件指定路径
			uint64_t  total_size;		//文件总大小
			int  is_overwrite;			//是否强制覆盖
			uint64_t file_create_time_;	//文件创建时间
			uint64_t file_modify_time_;	//最近修改时间
			uint64_t file_access_time_;	//最近访问时间
		};

		struct file_block{
			uint64_t offset;
			std::string stream;
			uint64_t file_id = 0;

			file_block& operator=(const file_block& lref){
				if (this == &lref)return *this;
				offset = lref.offset;
				stream = lref.stream;
				file_id = lref.file_id;
				return *this;
			}
		};

		struct file_info : asio_data{
			uint64_t file_size_ = 0;		//总大小
			uint32_t file_block_num_ = 0;	//块总数目
			uint64_t file_create_time_ = 0;	//文件创建时间
			uint64_t file_modify_time_ = 0;	//最近修改时间
			uint64_t file_access_time_ = 0;	//最近访问时间
			uint32_t file_attri_;			//文件属性
			uint64_t file_crc32_ = 0;		//文件crc32码
			std::string file_name_;			//文件名称
			uint64_t file_id_ = 0;				//文件ID
		};

		struct list_dir_info : asio_data{
			std::string list_data_;
		};

		enum  current_identify
		{
			pull_identify, //pull身份
			push_identify  //push身份
		};

		enum pkt_identify
		{
			receiver_pkt,	//发给receiver对象的包
			sender_pkt		//发给sender对象的包
		};
	}
}
