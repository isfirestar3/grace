

#ifndef PROTO_COMMON_BY_ID
#define PROTO_COMMON_BY_ID

#include<memory.h>
#include <stdlib.h>
#include "proto_definion.h"
#include "proto_ack_typedef.h"

namespace nsp {
	namespace proto {
		typedef struct __common_id_t
		{
			nsp::proto::proto_crt_t<int> var_id_;
			nsp::proto::proto_crt_t<int> offset_;
			nsp::proto::proto_crt_t<int> length_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = var_id_.serialize(pos);
				pos = offset_.serialize(pos);
				pos = length_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = var_id_.build(pos, cb);
				pos = offset_.build(pos, cb);
				pos = length_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return sizeof(int) * 3;
			}

		}common_id_t;

		typedef struct __proto_vec_comm_id
		{

			nsp::proto::proto_head head;
			nsp::proto::proto_vector_t<common_id_t> vec_common_par;

			unsigned char *serialize(unsigned char * byte_stream)
			{
				unsigned char *pos = byte_stream;
				pos = vec_common_par.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = vec_common_par.build(pos, cb);
				return pos;
			}

			int length()
			{
				return vec_common_par.length();
			}
		}proto_vec_comm_id;
	}
}

#endif