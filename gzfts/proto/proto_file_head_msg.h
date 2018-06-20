
#pragma once 

#include "serialize.hpp"
#include "proto_definion.h"


namespace nsp {
	namespace proto {
		typedef struct proto_file_head_msg:public nsp::proto::proto_interface
		{
			proto_file_head_msg(int type, int id) :head_(type, id) {}
			proto_file_head_msg() {};
			nsp::proto::proto_head head_;
			nsp::proto::proto_string_t<char> path_;
			nsp::proto::proto_crt_t<uint64_t> total_size_;
			nsp::proto::proto_crt_t<uint32_t> total_blocks_;
			nsp::proto::proto_crt_t<int> is_overwrite_;
			nsp::proto::proto_crt_t<uint64_t> file_create_time_;
			nsp::proto::proto_crt_t<uint64_t> file_modify_time_;
			nsp::proto::proto_crt_t<uint64_t> file_access_time_;

			virtual const int length() const
			{
				return head_.length() + path_.length() + total_size_.length() + total_blocks_ .length()+ is_overwrite_.length() + file_create_time_.length() + file_modify_time_.length() + file_access_time_.length();
			}

			virtual unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = head_.serialize(bytes);
				pos = path_.serialize(pos);
				pos = total_size_.serialize(pos);
				pos = total_blocks_.serialize(pos);
				pos = is_overwrite_.serialize(pos);
				pos = file_create_time_.serialize(pos);
				pos = file_modify_time_.serialize(pos);
				pos = file_access_time_.serialize(pos);
				return pos;
			}

			virtual const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = head_.build(bytes, cb);
				pos = path_.build(pos, cb);
				pos = total_size_.build(pos, cb);
				pos = total_blocks_.build(pos, cb);
				pos = is_overwrite_.build(pos, cb);
				pos = file_create_time_.build(pos, cb);
				pos = file_modify_time_.build(pos, cb);
				pos = file_access_time_.build(pos, cb);
				return pos;
			}

		}proto_file_head_msg_t;
	}
}