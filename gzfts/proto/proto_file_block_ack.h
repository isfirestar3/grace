

#pragma once 
#include "serialize.hpp"
#include "proto_definion.h"


namespace nsp {
	namespace proto {
		typedef struct proto_file_block_ack :public nsp::proto::proto_interface
		{
			proto_file_block_ack(int type, int id) :head_(type, id) {}
			proto_file_block_ack(){};

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint64_t> offset_;
			nsp::proto::proto_crt_t<uint32_t> len_;

			const int length() const
			{
				return head_.length() + offset_.length() + len_.length();
			}

			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = head_.serialize(bytes);
				pos = offset_.serialize(pos);
				pos = len_.serialize(pos);
				return pos;
			}

			virtual const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = head_.build(bytes, cb);
				pos = offset_.build(pos, cb);
				pos = len_.build(pos, cb);
				return pos;
			}

		}proto_file_block_ack_t;
	}
}