#ifndef PROTO_MSG_H
#define PROTO_MSG_H

#include "proto_definion.h"
#include "serialize.hpp"

namespace nsp {
	namespace proto {
		typedef struct _proto_msg : public proto_interface
		{
			_proto_msg(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_msg() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_string_t<char> ls_dir_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = ls_dir_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = ls_dir_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + ls_dir_.length();
			}
		}proto_msg;


		typedef struct _proto_write_head : public proto_interface
		{
			_proto_write_head(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_write_head() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_string_t<char> file_name_;
			nsp::proto::proto_crt_t<uint64_t> total_size_;
			nsp::proto::proto_crt_t<uint64_t> file_create_time_;
			nsp::proto::proto_crt_t<uint64_t> file_modify_time_;
			nsp::proto::proto_crt_t<uint64_t> file_access_time_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = file_name_.serialize(pos);
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
				pos = file_name_.build(pos, cb);
				pos = total_size_.build(pos, cb);
				pos = file_create_time_.build(pos, cb);
				pos = file_modify_time_.build(pos, cb);
				pos = file_access_time_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + file_name_.length() + total_size_.length() + file_create_time_.length() + file_modify_time_.length() + file_access_time_.length();
			}
		}proto_write_head;


		typedef struct _proto_request_file : public proto_interface
		{
			_proto_request_file(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_request_file() {}

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
				return head_.length() + file_path_.length() ;
			}
		}proto_request_file;

		typedef struct _proto_request_file_data : public proto_interface
		{
			_proto_request_file_data(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_request_file_data() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint64_t> file_offset_;
			nsp::proto::proto_crt_t<uint32_t> file_length_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = file_offset_.serialize(pos);
				pos = file_length_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = file_offset_.build(pos, cb);
				pos = file_length_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length()  + file_offset_.length() + file_length_.length();//+ file_name_.length()
			}
		}proto_request_file_data;


		typedef struct _proto_file_data : public proto_interface
		{
			_proto_file_data(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_file_data() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint64_t> file_offset_;
			nsp::proto::proto_string_t<char> file_data_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = file_offset_.serialize(pos);
				pos = file_data_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = file_offset_.build(pos, cb);
				pos = file_data_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + file_offset_.length() + file_data_.length();// + file_path_.length()
			}
		}proto_file_data;

		typedef struct _proto_file_delete : public proto_interface
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
		}proto_file_delete;

		typedef struct _proto_identify : public proto_interface
		{
			_proto_identify(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_identify() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int> identify_;
			nsp::proto::proto_crt_t<int> error_code_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = identify_.serialize(pos);
				pos = error_code_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = identify_.build(pos, cb);
				pos = error_code_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + identify_.length() + error_code_.length();
			}
		}proto_identify;

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
		}proto_file_info_t;

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

		/***********************************************以下为长连接数据包***********************************************************/
		typedef struct _proto_request_file_long_lnk : public proto_interface
		{
			_proto_request_file_long_lnk(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_request_file_long_lnk() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_string_t<char> file_path_;
			nsp::proto::proto_crt_t<int> long_lnk_;
			nsp::proto::proto_crt_t<uint64_t> pull_id_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = file_path_.serialize(pos);
				pos = long_lnk_.serialize(pos);
				pos = pull_id_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = file_path_.build(pos, cb);
				pos = long_lnk_.build(pos, cb);
				pos = pull_id_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + file_path_.length() + long_lnk_.length() + pull_id_.length();
			}
		}proto_request_file_long_lnk_t;

		typedef struct _proto_write_head_long_lnk : public proto_interface
		{
			_proto_write_head_long_lnk(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_write_head_long_lnk() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint64_t> file_id_;
			nsp::proto::proto_string_t<char> file_name_;
			nsp::proto::proto_crt_t<uint64_t> total_size_;
			nsp::proto::proto_crt_t<uint64_t> file_create_time_;
			nsp::proto::proto_crt_t<uint64_t> file_modify_time_;
			nsp::proto::proto_crt_t<uint64_t> file_access_time_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = file_id_.serialize(pos);
				pos = file_name_.serialize(pos);
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
				pos = file_id_.build(pos, cb);
				pos = file_name_.build(pos, cb);
				pos = total_size_.build(pos, cb);
				pos = file_create_time_.build(pos, cb);
				pos = file_modify_time_.build(pos, cb);
				pos = file_access_time_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + file_id_.length() + file_name_.length() + total_size_.length() + file_create_time_.length() + 
					file_modify_time_.length() + file_access_time_.length();
			}
		}proto_write_head_long_lnk_t;

		typedef struct _proto_long_lnk_request_file_data : public proto_interface
		{
			_proto_long_lnk_request_file_data(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_long_lnk_request_file_data() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint64_t> file_id_;
			nsp::proto::proto_crt_t<uint64_t> file_offset_;
			nsp::proto::proto_crt_t<uint32_t> file_length_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = file_id_.serialize(pos);
				pos = file_offset_.serialize(pos);
				pos = file_length_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = file_id_.build(pos, cb);
				pos = file_offset_.build(pos, cb);
				pos = file_length_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + file_id_.length() + file_offset_.length() + file_length_.length();//+ file_name_.length()
			}
		}proto_long_lnk_request_file_data_t;

		typedef struct _proto_long_lnk_identify : public proto_interface
		{
			_proto_long_lnk_identify(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_long_lnk_identify() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint64_t> file_id;
			nsp::proto::proto_crt_t<int> identify_;
			nsp::proto::proto_crt_t<int> pkt_identify_;
			nsp::proto::proto_crt_t<int> error_code_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = file_id.serialize(pos);
				pos = identify_.serialize(pos);
				pos = pkt_identify_.serialize(pos);
				pos = error_code_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = file_id.build(pos, cb);
				pos = identify_.build(pos, cb);
				pos = pkt_identify_.build(pos, cb);
				pos = error_code_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + file_id.length() + identify_.length() + pkt_identify_.length() + error_code_.length();
			}
		}proto_long_lnk_identify_t;

		typedef struct _proto_file_long_lnk_data : public proto_interface
		{
			_proto_file_long_lnk_data(uint32_t type, uint32_t id) :head_(type, id) {}
			_proto_file_long_lnk_data() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint64_t> file_id_;
			nsp::proto::proto_crt_t<uint64_t> file_offset_;
			nsp::proto::proto_string_t<char> file_data_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = file_id_.serialize(pos);
				pos = file_offset_.serialize(pos);
				pos = file_data_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = file_id_.build(pos, cb);
				pos = file_offset_.build(pos, cb);
				pos = file_data_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + file_id_.length() + file_offset_.length() + file_data_.length();
			}
		}proto_file_long_lnk_data_t;

	}
}

#endif