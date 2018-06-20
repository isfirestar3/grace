#ifndef PROTO_TASK_OPERA_PARM
#define PROTO_TASK_OPERA_PARM


#include "proto_definion.h"
#include "serialize.hpp"


namespace nsp {
	namespace proto {

		typedef struct proto_task_opera_parm:public nsp::proto::proto_interface
		{
			proto_task_opera_parm(uint32_t type, uint32_t id) :head_(type, id) {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int>  lower_bound_;
			nsp::proto::proto_crt_t<int>  upper_bound_;
			nsp::proto::proto_crt_t<int>  target_status_;
			nsp::proto::proto_crt_t<uint32_t> code_;
			nsp::proto::proto_vector_t<nsp::proto::proto_crt_t<uint64_t>> vec_parm_;
			nsp::proto::proto_crt_t<int> mask_;

			unsigned char *serialize(unsigned char *bytes) const override
			{
				unsigned char *pos = bytes;
				pos = head_.serialize(pos);
				pos = lower_bound_.serialize(pos);
				pos = upper_bound_.serialize(pos);
				pos = target_status_.serialize(pos);
				pos = code_.serialize(pos);
				pos = vec_parm_.serialize(pos);
				pos = mask_.serialize(pos);
				return pos;
			}
			
			const unsigned char* build(const unsigned char* pkt_bytes, int &cb) override
			{
				const unsigned char *pos = pkt_bytes;
				pos = head_.build(pos, cb);
				pos = lower_bound_.build(pos, cb);
				pos = upper_bound_.build(pos, cb);
				pos = target_status_.build(pos, cb);
				pos = code_.build(pos, cb);
				pos = vec_parm_.build(pos, cb);
				pos = mask_.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return 4 * sizeof(int) + vec_parm_.length() + sizeof(uint32_t) + head_.length();
			}

			void calc_size()
			{
				head_.size_ = length();
			}
		}proto_task_opera_parm_t;
	}
}




#endif