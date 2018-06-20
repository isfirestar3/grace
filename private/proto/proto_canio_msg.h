



#ifndef PROTO_CANIO_MSG_H
#define PROTO_CANIO_MSG_H

#include "proto_definion.h"

namespace nsp {
	namespace proto {
		typedef struct proto_canio_msg :public nsp::proto::proto_interface
		{
			proto_canio_msg(uint32_t type, uint32_t id) :head_(type, id) {}
			proto_canio_msg() {}
		
			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint32_t>  id_;
			nsp::proto::proto_crt_t<uint32_t>  var_type_;
			nsp::proto::proto_crt_t<uint32_t>  command_;
			nsp::proto::proto_crt_t<uint32_t>  middle_;
			nsp::proto::proto_crt_t<uint32_t>  response_;

			unsigned char * serialize(unsigned char *byte_stream) const override
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = id_.serialize(pos);
				pos = var_type_.serialize(pos);
				pos = command_.serialize(pos);
				pos = middle_.serialize(pos);
				pos = response_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb) override
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = id_.build(pos, cb);
				pos = var_type_.build(pos, cb);
				pos = command_.build(pos, cb);
				pos = middle_.build(pos, cb);
				pos = response_.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return sizeof(uint32_t) * 5 + head_.length();
			}

			void calc_size()
			{
				head_.size_ = length();
			}

		}proto_canio_msg_t;

	}
}




#endif
