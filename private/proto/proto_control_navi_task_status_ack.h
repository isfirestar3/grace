

#ifndef PROTO_CONTROL_NAVI_TASK_STATUS_ACK
#define PROTO_CONTROL_NAVI_TASK_STATUS_ACK

#include "proto_definion.h"
#include "serialize.hpp"


namespace nsp {
	namespace proto {
		typedef struct proto_control_navi_task_status_ack:public nsp::proto::proto_interface{
		    
			proto_control_navi_task_status_ack() {}
			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<uint64_t> task_id_;
			nsp::proto::proto_crt_t<int> status_;

			unsigned char* serialize(unsigned char* byte_stream) const override
			{
				unsigned char* pos = byte_stream;
				pos = head_.serialize(pos);
				pos = task_id_.serialize(pos);
				pos = status_.serialize(pos);
				return pos;
			}

			const unsigned char* build(const unsigned char*byte_stream, int &cb) override
			{
				const unsigned char* pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = task_id_.build(pos, cb);
				pos = status_.build(pos, cb);
				return pos;
			}

			const int length()const override
			{
				return head_.length() + task_id_.length() + status_.length();
			}

		}proto_control_navi_task_status_ack_t;
	}
}


#endif