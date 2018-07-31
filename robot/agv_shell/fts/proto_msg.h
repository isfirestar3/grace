#ifndef __AGV_SHELL_FILE_PROTO_MSG_H__
#define __AGV_SHELL_FILE_PROTO_MSG_H__

#include "proto_definion.h"

namespace file {
	namespace proto {
		//ÎÄ¼þ×´Ì¬
		enum file_status{
			cancle = -4,
			uninit = -3,
			read_error = -2,
			write_error = -1,
			normal = 0,
			complete = 1
		};
		#pragma pack(push, 1)
		
		typedef struct _proto_file_delete : public nsp::proto::proto_interface
		{
			_proto_file_delete(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_file_delete() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_string_t<char> file_path_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = file_path_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = file_path_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + file_path_.length();
			}
		} proto_file_delete;
		

		typedef struct proto_file_info : public nsp::proto::proto_interface {
			proto_file_info() {}
			~proto_file_info(){}

			nsp::proto::proto_string_t<char> file_name_;
			nsp::proto::proto_crt_t<int> file_type;
			nsp::proto::proto_crt_t<int> file_attributes_;
			nsp::proto::proto_crt_t<uint64_t> file_size_;
			nsp::proto::proto_crt_t<uint64_t> file_crc32_;
			nsp::proto::proto_crt_t<uint64_t> file_create_time_;
			nsp::proto::proto_crt_t<uint64_t> file_modify_time_;
			nsp::proto::proto_crt_t<uint64_t> file_access_time_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = file_name_.serialize(pos);
				pos = file_type.serialize(pos);
				pos = file_attributes_.serialize(pos);
				pos = file_size_.serialize(pos);
				pos = file_crc32_.serialize(pos);
				pos = file_create_time_.serialize(pos);
				pos = file_modify_time_.serialize(pos);
				pos = file_access_time_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = file_name_.build(pos, cb);
				pos = file_type.build(pos, cb);
				pos = file_attributes_.build(pos, cb);
				pos = file_size_.build(pos, cb);
				pos = file_crc32_.build(pos, cb);
				pos = file_create_time_.build(pos, cb);
				pos = file_modify_time_.build(pos, cb);
				pos = file_access_time_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return file_name_.length() + file_type.length() + file_attributes_.length() + file_size_.length() + file_crc32_.length() +
					file_create_time_.length() + file_modify_time_.length() + file_access_time_.length();
			}
		} proto_file_info_t;
		

		typedef struct proto_file_info_list : public nsp::proto::proto_interface
		{
			proto_file_info_list() {}
			proto_file_info_list(uint32_t type, uint32_t id) :head_(type, id) {}
			~proto_file_info_list() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_vector_t<proto_file_info> file_list_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = file_list_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = file_list_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return file_list_.length() + head_.length();
			}
		}proto_file_info_list_t;

		
		typedef struct _proto_request_file_head : public nsp::proto::proto_interface
		{
			_proto_request_file_head(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_request_file_head() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_string_t<char> file_path_;
			nsp::proto::proto_crt_t<uint64_t> file_id_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = file_path_.serialize(pos);
				pos = file_id_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = file_path_.build(pos, cb);
				pos = file_id_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + file_path_.length() + file_id_.length();
			}
		} proto_request_file_head_t;
		

		typedef struct _proto_file_head_info : public nsp::proto::proto_interface
		{
			_proto_file_head_info(uint32_t type) :head_(type) {}
			_proto_file_head_info(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_file_head_info() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint32_t> file_type_;
            nsp::proto::proto_string_t<char> file_name_;
			nsp::proto::proto_crt_t<uint64_t> file_id_;
			nsp::proto::proto_crt_t<uint64_t> total_size_;
			nsp::proto::proto_crt_t<uint64_t> file_create_time_;
			nsp::proto::proto_crt_t<uint64_t> file_modify_time_;
			nsp::proto::proto_crt_t<uint64_t> file_access_time_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = file_type_.serialize(pos);
                pos = file_name_.serialize(pos);
				pos = file_id_.serialize(pos);
				pos = total_size_.serialize(pos);
				pos = file_create_time_.serialize(pos);
				pos = file_modify_time_.serialize(pos);
				pos = file_access_time_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = file_type_.build(pos, cb);
                pos = file_name_.build(pos, cb);
				pos = file_id_.build(pos, cb);
				pos = total_size_.build(pos, cb);
				pos = file_create_time_.build(pos, cb);
				pos = file_modify_time_.build(pos, cb);
				pos = file_access_time_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + file_type_.length() + file_id_.length() + file_name_.length() + total_size_.length() + file_create_time_.length() + 
					file_modify_time_.length() + file_access_time_.length();
			}
		} proto_file_head_info_t;
		

		typedef struct _proto_request_file_data : public nsp::proto::proto_interface
		{
			_proto_request_file_data(uint32_t type) :head_(type) {}
			_proto_request_file_data(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_request_file_data() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint32_t> block_num_;
			nsp::proto::proto_crt_t<uint32_t> file_length_;
			nsp::proto::proto_crt_t<uint64_t> file_id_;
			nsp::proto::proto_crt_t<uint64_t> file_offset_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = block_num_.serialize(pos);
				pos = file_length_.serialize(pos);
				pos = file_id_.serialize(pos);
				pos = file_offset_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = block_num_.build(pos, cb);
				pos = file_length_.build(pos, cb);
				pos = file_id_.build(pos, cb);
				pos = file_offset_.build(pos, cb);
				
				return pos;
			}

			const int length() const
			{
				return head_.length() + block_num_.length() + file_id_.length() + file_offset_.length() + file_length_.length();
			}
		} proto_request_file_data_t;
		

		typedef struct _proto_file_data : public nsp::proto::proto_interface
		{
			_proto_file_data(uint32_t type) :head_(type) {}
			_proto_file_data(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_file_data() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint32_t> block_num_;
			nsp::proto::proto_crt_t<uint64_t> file_id_;
			nsp::proto::proto_crt_t<uint64_t> file_offset_;
			nsp::proto::proto_string_t<char> file_data_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = block_num_.serialize(pos);
				pos = file_id_.serialize(pos);
				pos = file_offset_.serialize(pos);
				pos = file_data_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = block_num_.build(pos, cb);
				pos = file_id_.build(pos, cb);
				pos = file_offset_.build(pos, cb);
				pos = file_data_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + block_num_.length() + file_id_.length() + file_offset_.length() + file_data_.length();
			}
		} proto_file_data_t;
		
		
		typedef struct _proto_file_status : public nsp::proto::proto_interface
		{
			_proto_file_status(uint32_t type) :head_(type) {}
			_proto_file_status(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_file_status() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int> error_code_;
			nsp::proto::proto_crt_t<uint32_t> block_num_;
			nsp::proto::proto_crt_t<uint64_t> file_id_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = error_code_.serialize(pos);
				pos = block_num_.serialize(pos);
				pos = file_id_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = error_code_.build(pos, cb);
				pos = block_num_.build(pos, cb);
				pos = file_id_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + block_num_.length() + file_id_.length() + error_code_.length();
			}
		} proto_file_status_t;
		
		#pragma pack(pop)
	}
}

#endif //__AGV_SHELL_FILE_PROTO_MSG_H__
