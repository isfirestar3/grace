

#ifndef PROTO_PRE_LOGIN_H
#define PROTO_PRE_LOGIN_H

#include "proto_definion.h"

namespace nsp {
	namespace proto {
		typedef struct proto_key:public nsp::proto::proto_interface{

			nsp::proto::proto_head head_;
			nsp::proto::proto_string_t<char>  key_;

			unsigned char * serialize(unsigned char *byte_stream) const override
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = key_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb) override
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = key_.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return key_.length();
			}
		}proto_key_t;

	}
}




#endif