


#ifndef PROTO_LOGIN_H
#define PROTO_LOGIN_H

#include "proto_definion.h"
#include "serialize.hpp"
#include "proto_typedef.h"


namespace nsp {
	namespace proto {
		typedef struct proto_login_robot : public nsp::proto::proto_interface
		{
			proto_login_robot(uint32_t type, uint32_t id) :head(type, id) {}
			nsp::proto::proto_head head;
			nsp::proto::proto_crt_t<nsp__controlor_type_t>  cct_;
			nsp::proto::proto_crt_t<uint32_t>  acquire_access_;
			nsp::proto::proto_string_t<char>   original_buffer_;
			nsp::proto::proto_string_t<char>   encrypted_buffer_;

			unsigned char *serialize(unsigned char *bytes) const override
			{
				unsigned char *pos = bytes;
				pos = head.serialize(pos);
				pos = cct_.serialize(pos);
				pos = acquire_access_.serialize(pos);
				pos = original_buffer_.serialize(pos);
				pos = encrypted_buffer_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb) override
			{
				const unsigned char *pos = bytes;
				pos = head.build(pos,cb);
				pos = cct_.build(pos, cb);
				pos = acquire_access_.build(pos, cb);
				pos = original_buffer_.build(pos, cb);
				pos = encrypted_buffer_.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return sizeof(uint32_t) + sizeof(uint32_t) + original_buffer_.length() + encrypted_buffer_.length()+head.length();
			}

			void calc_size()
			{
				head.size_ = length();
			}

		}proto_login_robot;
	}
}

	
#endif