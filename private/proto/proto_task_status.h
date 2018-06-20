



#pragma once

#include "serialize.hpp"
#include "proto_definion.h"

namespace nsp {
	namespace proto {

		typedef struct proto_task_status:public nsp::proto::proto_interface
		{

			proto_task_status(uint32_t type, uint32_t id) : head_(type, id) {}
			proto_task_status() {}
			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint64_t> task_id_;

			
			unsigned char *serialize(unsigned char *bytes) const override
			{
				unsigned char *pos = bytes;
				pos = head_.serialize(pos);
				pos = task_id_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb) override
			{
				const unsigned char *pos = bytes;
				pos = head_.build(pos, cb);
				pos = task_id_.build(pos, cb);
				return pos;
			}

			const int length() const override
			{ 
				return sizeof(uint64_t) + head_.length();
			}

			void calc_size()
			{
				head_.size_ = length();
			}
		}proto_task_status_t;

		typedef proto_task_status_t proto_operator_task_action_t;
		typedef proto_task_status_t proto_allocate_operation_task_ack_t;
		typedef proto_task_status_t proto_control_navigation_task;
	}
}