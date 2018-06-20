


#ifndef PROTO_NAVIGTION_TASK_H
#define PROTO_NAVIGTION_TASK_H


#include "proto_definion.h"
#include "serialize.hpp"
#include <vector>

namespace nsp {
	namespace proto {

		typedef struct _upl_t {
			
			nsp::proto::proto_crt_t<int>		edge_id_;
			nsp::proto::proto_crt_t<double>	    percentage_;
			nsp::proto::proto_crt_t<double>		angle_;


			unsigned char * serialize(unsigned char* pkt_bytes)const
			{
				unsigned char * pos = pkt_bytes;
				pos = edge_id_.serialize(pos);
				pos = percentage_.serialize(pos);
				pos = angle_.serialize(pos);
				return pos;
			}

			const unsigned char* build(const unsigned char*pkt_bytes, int &cb)
			{
				const unsigned char * pos = pkt_bytes;
				pos = edge_id_.build(pos, cb);
				pos = percentage_.build(pos, cb);
				pos = angle_.build(pos, cb);
				return pos;
			}

			const int length()const
			{
				return sizeof(int) +2*sizeof(double);
			}

		}upl_t;



		typedef struct _POSITION {
			nsp::proto::proto_crt_t<double> x_;
			nsp::proto::proto_crt_t<double> y_;
			nsp::proto::proto_crt_t<double> w_;
			unsigned char * serialize(unsigned char* pkt_bytes)const 
			{
				unsigned char * pos = pkt_bytes;
				pos = x_.serialize(pos);
				pos = y_.serialize(pos);
				pos = w_.serialize(pos);
				return pos;
			}

			const unsigned char* build(const unsigned char*pkt_bytes, int &cb)
			{
				const unsigned char * pos = pkt_bytes;
				pos = x_.build(pos, cb);
				pos = y_.build(pos, cb);
				pos = w_.build(pos, cb);
				return pos;
			}

			int length() const
			{
				return sizeof(double) * 3;
			}

		}position_t;

		typedef struct _trail {
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

		}trail_t;


		typedef struct proto_navigation_task :public nsp::proto::proto_interface
		{
			proto_navigation_task(uint32_t type,uint32_t id) :head(type,id) {}

			nsp::proto::proto_head					head;
			nsp::proto::proto_crt_t<uint64_t>		set_task_id_;
			upl_t									dest_upl_;
			position_t								dest_pos_;
			nsp::proto::proto_crt_t<int>			is_traj_whole_;	/* 是否完整的导航轨迹(是否允许导航任务结束) */
			nsp::proto::proto_vector_t<trail_t>		vec_trail_t;

			unsigned char * serialize(unsigned char* pkt_bytes)const  override
			{
				unsigned char * pos = pkt_bytes;
				pos = head.serialize(pos);
				pos = set_task_id_.serialize(pos);
				pos = dest_upl_.serialize(pos);
				pos = dest_pos_.serialize(pos);
				pos = is_traj_whole_.serialize(pos);
				pos = vec_trail_t.serialize(pos);
				return pos;
			}

			const unsigned char* build(const unsigned char*pkt_bytes, int &cb) override
			{
				const unsigned char * pos = pkt_bytes;
				pos = head.build(pos, cb);
				pos = set_task_id_.build(pos, cb);
				pos = dest_upl_.build(pos, cb);
				pos = dest_pos_.build(pos, cb);
				pos = is_traj_whole_.build(pos, cb);
				pos = vec_trail_t.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return set_task_id_.length() + dest_upl_.length() + dest_pos_.length() + vec_trail_t.length() + is_traj_whole_.length() + head.length();
			}

			void calc_size()
			{
				head.size_ = length();
			}

		}proto_navigation_task_t;

	}
}





#endif