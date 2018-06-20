

#pragma once 

#include "proto_definion.h"
#include "serialize.hpp"

namespace nsp {
	namespace proto {
		typedef struct proto_operator_task_action_ack:public nsp::proto::proto_interface{

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint64_t> task_id_;
			nsp::proto::proto_crt_t<int32_t> optcode_;

			const unsigned char *build(const unsigned char *bytes, int &cb) override
			{
				const unsigned char* pos = bytes;
				pos = head_.build(pos, cb);
				pos = task_id_.build(pos, cb);
				pos = optcode_.build(pos, cb);
				return pos;
			}

			unsigned char *serialize(unsigned char *bytes) const override
			{
				unsigned char* pos = bytes;
				pos = head_.serialize(pos);
				pos = task_id_.serialize(pos);
				pos = optcode_.serialize(pos);
				return pos;
			}

		    const int length()const  override
			{
				return  head_.length() +sizeof(uint64_t) +sizeof(int32_t);
			}
		}proto_operator_task_action_ack_t;
	}
}