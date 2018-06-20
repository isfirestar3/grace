#ifndef PROTO_COMMON_PLUS
#define PROTO_COMMON_PLUS

#include<memory.h>
#include <stdlib.h>
#include "proto_definion.h"

namespace nsp{
	namespace proto{
		typedef struct __common_t {
			nsp::proto::proto_crt_t<int> var_id_;
			nsp::proto::proto_crt_t<int> var_type_;
			nsp::proto::proto_crt_t<int> offset_;
			nsp::proto::proto_crt_t<int> length_;

			unsigned char *serialize(unsigned char * byte_stream) const 
			{
				unsigned char *pos = byte_stream;
				pos = var_id_.serialize(pos);
				pos = var_type_.serialize(pos);
				pos = offset_.serialize(pos);
				pos = length_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = var_id_.build(pos, cb);
				pos = var_type_.build(pos, cb);
				pos = offset_.build(pos, cb);
				pos = length_.build(pos, cb);
				return pos;
			}

			const int length() const 
 			{
				return sizeof(int) * 4;
			}

		}common_t;

		typedef struct proto_vec_comm:public nsp::proto::proto_interface
		{
			proto_vec_comm(uint32_t type, uint32_t id) :head(type, id) {}
			nsp::proto::proto_head head;
			nsp::proto::proto_vector_t<common_t> vec_common_par;

			unsigned char *serialize(unsigned char *bytes) const override
			{
				unsigned char *pos = bytes;
				pos = head.serialize(pos);
				pos = vec_common_par.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte, int &cb) override
			{
				const unsigned char *pos = byte;
				pos = head.build(pos, cb);
				pos = vec_common_par.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return vec_common_par.length() + head.length();
			}

			void calc_size()
			{
				head.size_ = length();
			}


		}proto_vec_comm_t;
		
		struct proto_periodic_title {
			nsp::proto::proto_crt_t<int> var_id_;
			nsp::proto::proto_crt_t<int> event_cycle_; // in milliseconds
			nsp::proto::proto_crt_t<int> var_type_;
			nsp::proto::proto_crt_t<int> offset_;
			nsp::proto::proto_crt_t<int> length_;

			unsigned char *serialize(unsigned char * byte_stream) const 
			{
				unsigned char *pos = byte_stream;
				pos = var_id_.serialize(pos);
				pos = event_cycle_.serialize(pos);
				pos = var_type_.serialize(pos);
				pos = offset_.serialize(pos);
				pos = length_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = var_id_.build(pos, cb);
				pos = event_cycle_.build(pos, cb);
				pos = var_type_.build(pos, cb);
				pos = offset_.build(pos, cb);
				pos = length_.build(pos, cb);
				return pos;
			}

			const int length() const 
 			{
				return sizeof(int) * 5;
			}
		};

		struct proto_periodic_data {
			nsp::proto::proto_crt_t<int> var_id_;
			nsp::proto::proto_crt_t<int> event_cycle_; // in milliseconds
			nsp::proto::proto_crt_t<int> var_type_;
			nsp::proto::proto_crt_t<int> offset_;
			nsp::proto::proto_string_t<char> data_;

			unsigned char *serialize(unsigned char * byte_stream) const 
			{
				unsigned char *pos = byte_stream;
				pos = var_id_.serialize(pos);
				pos = event_cycle_.serialize(pos);
				pos = var_type_.serialize(pos);
				pos = offset_.serialize(pos);
				pos = data_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = var_id_.build(pos, cb);
				pos = event_cycle_.build(pos, cb);
				pos = var_type_.build(pos, cb);
				pos = offset_.build(pos, cb);
				pos = data_.build(pos, cb);
				return pos;
			}

			const int length() const 
 			{
				return sizeof(int) * 4 + data_.length();
			}
		};
		
		template<class T>
		struct proto_periodic_report : public nsp::proto::proto_interface
		{
			proto_periodic_report(uint32_t type, uint32_t id) :head(type, id) {}
			proto_periodic_report() {}

			nsp::proto::proto_head head;
			nsp::proto::proto_crt_t<uint64_t> timestamp;
			nsp::proto::proto_vector_t<T> parameters;

			unsigned char *serialize(unsigned char *bytes) const override
			{
				unsigned char *pos = bytes;
				pos = head.serialize(pos);
				pos = timestamp.serialize(pos);
				pos = parameters.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte, int &cb) override
			{
				const unsigned char *pos = byte;
				pos = head.build(pos, cb);
				pos = timestamp.build(pos, cb);
				pos = parameters.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return parameters.length() + head.length() + timestamp.length();
			}

			void calc_size()
			{
				head.size_ = length();
			}
		};
	}
}

#endif