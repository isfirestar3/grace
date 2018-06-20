

#ifndef PROTO_COMMON_REPORT
#define PROTO_COMMON_REPORT

#include<memory.h>
#include <stdlib.h>
#include "proto_definion.h"

namespace nsp {
	namespace proto {
		typedef struct proto_common_report :public nsp::proto::proto_head
		{
			proto_common_report() {};
			nsp::proto::proto_head head;
			nsp::proto::proto_crt_t<uint64_t> task_id_;
			nsp::proto::proto_crt_t<int> var_type_;
			nsp::proto::proto_crt_t<int> status_;
			nsp::proto::proto_string_t<char> usrdat_;


			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head.serialize(pos);
				pos = task_id_.serialize(pos);
				pos = var_type_.serialize(pos);
				pos = status_.serialize(pos);
				pos = usrdat_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = head.build(pos, cb);
				pos = task_id_.build(pos, cb);
				pos = var_type_.build(pos, cb);
				pos = status_.build(pos, cb);
				pos = usrdat_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return sizeof(int) * 2 + usrdat_.length() + head.length() + sizeof(task_id_);
			}
            void calc_size()
            {
                head.size_ = length();
            }

		}proto_common_report_t;
	}
}

#endif