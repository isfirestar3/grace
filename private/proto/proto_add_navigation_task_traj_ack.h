#ifndef PROTO_ADD_NAVIGATION_TASK_TRAJ_ACK
#define PROTO_ADD_NAVIGATION_TASK_TRAJ_ACK

#include "proto_definion.h"


namespace nsp {
	namespace proto {
		typedef struct _task_trail
		{
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

		}navi_task_add_trail_t;


		typedef struct query_add_navigation_task_traj_ack : public nsp::proto::proto_interface
		{
			query_add_navigation_task_traj_ack(uint32_t type, uint32_t id):head(type,id) {}

			nsp::proto::proto_head head;
			nsp::proto::proto_crt_t<uint64_t> task_id_;
            nsp::proto::proto_crt_t<int> index_;
			nsp::proto::proto_crt_t<int>	is_traj_whole_;	/* 是否完整的导航轨迹(是否允许导航任务结束) */
			nsp::proto::proto_vector_t<navi_task_add_trail_t> cnt_trajs_;

			unsigned char * serialize(unsigned char* pkt_bytes) const override
			{
				unsigned char * pos = pkt_bytes;
				pos = head.serialize(pos);
				pos = task_id_.serialize(pos);
                pos = index_.serialize(pos);
				pos = is_traj_whole_.serialize(pos);
				pos = cnt_trajs_.serialize(pos);
				return pos;
			}

			const unsigned char* build(const unsigned char*pkt_bytes, int &cb) override
			{
				const unsigned char * pos = pkt_bytes;
				pos = head.build(pos,cb);
				pos = task_id_.build(pos, cb);
                pos = index_.build(pos, cb);
				pos = is_traj_whole_.build(pos, cb);
				pos = cnt_trajs_.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
                return  head.length() + task_id_.length() + index_.length() + is_traj_whole_.length() + cnt_trajs_.length();
			}

			void calc_size()
			{
				head.size_ = length();
			}

		}proto_add_navigation_task_traj_ack_t;
	}
}

#endif