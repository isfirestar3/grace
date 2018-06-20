

#ifndef PROTO_KEEP_ALIVE_H
#define PROTO_KEEP_ALIVE_H

#include "proto_definion.h"

namespace nsp {
	namespace proto {
		typedef struct proto_keep_alive:public nsp::proto::proto_interface{

			nsp::proto::proto_head head_;
			char address_[16];
			nsp::proto::proto_crt_t<uint16_t>  port_;

			unsigned char * serialize(unsigned char *byte_stream) const override
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				memcpy(pos, address_, sizeof(address_));
				pos += sizeof(address_);
				pos = port_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb) override
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				memcpy(address_, pos, sizeof(address_));
				cb -= sizeof(address_);
				pos = port_.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return sizeof(address_) + port_.length() + head_.length();
			}

			void calc_size()
			{
				head_.size_ = length();
			}

		}proto_keep_alive_t;

	}
}

#endif // PROTO_KEEP_ALIVE_H
