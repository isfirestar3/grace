



#ifndef PROTO_REPORT_STATUS_H
#define PROTO_REPORT_STATUS_H

#include "proto_definion.h"

namespace nsp {
	namespace proto {
		typedef struct proto_report_status :public nsp::proto::proto_interface
		{
			proto_report_status(uint32_t type, uint32_t id) :head_(type, id) {}

			nsp::proto::proto_head head_;
            nsp::proto::proto_crt_t<uint64_t>  task_id_;
			nsp::proto::proto_crt_t<uint32_t>  vartype_;
			nsp::proto::proto_crt_t<uint32_t>  status_;
			nsp::proto::proto_string_t<char>   usr_msg_;

			unsigned char * serialize(unsigned char *byte_stream) const override
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
                pos = task_id_.serialize(pos);
				pos = vartype_.serialize(pos);
				pos = status_.serialize(pos);
				pos = usr_msg_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb) override
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
                pos = task_id_.build(pos, cb);
				pos = vartype_.build(pos, cb);
				pos = status_.build(pos, cb);
				pos = usr_msg_.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
                return task_id_.length() + vartype_.length() + usr_msg_.length() + status_.length() + head_.length();
			}

			void calc_size()
			{
				head_.size_ = length();
			}

		}proto_report_status_t;

	}
}




#endif
