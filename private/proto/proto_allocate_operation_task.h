

#pragma once 

#include "serialize.hpp"
#include "proto_definion.h"


namespace nsp {
	namespace proto {
		typedef struct proto_allocate_operation_task:public nsp::proto::proto_interface
		{
			proto_allocate_operation_task(uint32_t type, uint32_t id) : head_(type, id) {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint64_t> task_id_;
			nsp::proto::proto_crt_t<int32_t> code_;
			nsp::proto::proto_crt_t<uint64_t> params0_;
			nsp::proto::proto_crt_t<uint64_t> params1_;
			nsp::proto::proto_crt_t<uint64_t> params2_;
			nsp::proto::proto_crt_t<uint64_t> params3_;
			nsp::proto::proto_crt_t<uint64_t> params4_;
			nsp::proto::proto_crt_t<uint64_t> params5_;
			nsp::proto::proto_crt_t<uint64_t> params6_;
			nsp::proto::proto_crt_t<uint64_t> params7_;
			nsp::proto::proto_crt_t<uint64_t> params8_;
			nsp::proto::proto_crt_t<uint64_t> params9_;

			unsigned char *serialize(unsigned char *bytes) const override
			{
				unsigned char *pos = bytes;
				pos = head_.serialize(pos);
				pos = task_id_.serialize(pos);
                pos = code_.serialize(pos);
				pos = params0_.serialize(pos);
				pos = params1_.serialize(pos);
				pos = params2_.serialize(pos);
				pos = params3_.serialize(pos);
				pos = params4_.serialize(pos);
				pos = params5_.serialize(pos);
				pos = params6_.serialize(pos);
				pos = params7_.serialize(pos);
				pos = params8_.serialize(pos);
				pos = params9_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb) override
			{
				const unsigned char *pos = bytes;
				pos = head_.build(pos, cb);
				pos = task_id_.build(pos, cb);
				pos = code_.build(pos, cb);
				pos = params0_.build(pos, cb);
				pos = params1_.build(pos, cb);
				pos = params2_.build(pos, cb);
				pos = params3_.build(pos, cb);
				pos = params4_.build(pos, cb);
				pos = params5_.build(pos, cb);
				pos = params6_.build(pos, cb);
				pos = params7_.build(pos, cb);
				pos = params8_.build(pos, cb);
				pos = params9_.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return task_id_.length() + sizeof(int32_t) + head_.length() + sizeof(uint64_t)* 10;
			}

			void calc_size()
			{
				head_.size_ = length();
			}


		}proto_allocate_operation_task_t;
	}
}