

#ifndef proto_common_ack_h
#define proto_common_ack_h

#include "proto_definion.h"

namespace nsp {
	namespace proto{

		typedef struct __proto_common_ack {
			
			nsp::proto::proto_crt_t<int> var_id_;
			nsp::proto::proto_crt_t<int> var_type_;
			nsp::proto::proto_crt_t<int> offset_;
			nsp::proto::proto_string_t<char> value_;

			unsigned char * serialize(unsigned char *byte_stream) const 
			{
				unsigned char *pos = byte_stream;
				pos = var_id_.serialize(pos);
				pos = var_type_.serialize(pos);
				pos = offset_.serialize(pos);
				pos = value_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream,int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = var_id_.build(pos, cb);
				pos = var_type_.build(pos,cb);
				pos = offset_.build(pos, cb);
				pos = value_.build( pos, cb );
				return pos;
			}

			const int length() const 
			{
				return var_type_.length() + offset_.length() + value_.length() + var_id_.length();
			}
		}proto_common_ack;


		typedef struct proto_vec_commom_ack:public nsp::proto::proto_interface
		{

			proto_vec_commom_ack(uint32_t type, uint32_t id) :head(type, id) {}
			proto_vec_commom_ack() {}

			nsp::proto::proto_head head;
			nsp::proto::proto_vector_t<proto_common_ack> vec_common_ack;

			unsigned char * serialize(unsigned char *byte_stream) const override
			{
				unsigned char *pos = byte_stream;
				pos = head.serialize(pos);
				pos = vec_common_ack.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb) override
			{
				unsigned const char *pos = byte_stream;
				pos = head.build(pos, cb);
				pos = vec_common_ack.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return vec_common_ack.length()+head.length();
			}

			void calc_size()
			{
				head.size_ = length();
			}
		}proto_vec_commom_ack_t;
                
         
        typedef struct __proto_common_compare_write:public nsp::proto::proto_interface
        {
			__proto_common_compare_write(uint32_t type, uint32_t id) :head(type, id) {}
			__proto_common_compare_write() {}

			nsp::proto::proto_head head;
			nsp::proto::proto_crt_t<int> nsp__controlor_type_t_;
             nsp::proto::proto_crt_t<int> var_id_;
			nsp::proto::proto_vector_t<proto_common_ack> vct_condition_;
			nsp::proto::proto_vector_t<proto_common_ack> vct_write_;

			unsigned char * serialize(unsigned char *byte_stream)const
			{
				unsigned char *pos = byte_stream;
				pos = head.serialize(pos);
				pos = nsp__controlor_type_t_.serialize(pos);
				pos = var_id_.serialize(pos);
				pos = vct_condition_.serialize(pos);
				pos = vct_write_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head.build(pos, cb);
				pos = nsp__controlor_type_t_.build(pos, cb);
				pos = var_id_.build(pos, cb);
				pos = vct_condition_.build(pos, cb);
				pos = vct_write_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head.length() + nsp__controlor_type_t_.length() + var_id_.length() + vct_condition_.length() + vct_write_.length();
			}

			void calc_size()
			{
				head.size_ = length();
			}
     
        }proto_common_compare_write_t;
	}
}

#endif