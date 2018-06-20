

#ifndef PROTO_NAVIGATION_TASK_TRAJ_ACK
#define PROTO_NAVIGATION_TASK_TRAJ_ACK

#include "proto_definion.h"
namespace nsp {
	namespace proto {


		typedef struct _navi_task_trail {
			nsp::proto::proto_crt_t<int>			edge_id_;
			nsp::proto::proto_crt_t<int>			wop_id_;

			unsigned char * serialize(unsigned char* pkt_bytes) const
			{
				unsigned char * pos = pkt_bytes;
				pos = edge_id_.serialize(pos);
				pos = wop_id_.serialize(pos);
				return pos;
			}

			const unsigned char* build(const unsigned char*pkt_bytes, int &cb)
			{
				const unsigned char * pos = pkt_bytes;
				pos = edge_id_.build(pos, cb);
				pos = wop_id_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return sizeof(int) * 2;
			}

		}navi_task_trail_t;


		typedef struct query_navigation_task_traj_ack:public nsp::proto::proto_interface{

			query_navigation_task_traj_ack() {}
			nsp::proto::proto_head head_;
            nsp::proto::proto_crt_t<uint64_t> task_id_;
			nsp::proto::proto_vector_t<navi_task_trail_t> trajs_;

			unsigned char * serialize(unsigned char* pkt_bytes)const override
			{
				unsigned char * pos = pkt_bytes;
				pos = head_.serialize(pos);
                pos = head_.serialize(pos);
				pos = trajs_.serialize(pos);
				return pos;
			}

			const unsigned char* build(const unsigned char*pkt_bytes, int &cb) override
			{
				const unsigned char * pos = pkt_bytes;
				pos = head_.build(pos, cb);
                pos = task_id_.build(pos, cb);
				pos = trajs_.build(pos, cb);
				return pos;
			}

			const int length()const override
			{
                return task_id_.length()  + trajs_.length() + head_.length();
			}

		}proto_navigation_task_traj_ack_t;
	}
}

#endif