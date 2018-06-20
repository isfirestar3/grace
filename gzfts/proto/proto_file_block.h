

#pragma once 
#include "proto_definion.h"
#include "serialize.hpp"

namespace nsp{
	namespace  proto{
		typedef struct proto_file_block:public nsp::proto::proto_interface
		{
			proto_file_block(int type, int id) :head_(type, id) {}
			proto_file_block() {}
			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint64_t> offset_;
			nsp::proto::proto_string_t<char>  block_data_;

			const int length() const
			{
				return head_.length()  + offset_.length() + block_data_.length();//+ path_.length()
			}

			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = head_.serialize(bytes);
				pos = offset_.serialize(pos);
				pos = block_data_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = head_.build(bytes, cb);
				pos = offset_.build(pos, cb);
				pos = block_data_.build(pos, cb);
				return pos;
			}

		}proto_file_block_t;
	}
}