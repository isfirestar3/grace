


#ifndef PROTO_NAVIGATION_TASK_STATUS_ACK
#define PROTO_NAVIGATION_TASK_STATUS_ACK
#include "proto_definion.h"
#include "vartypes.h"

namespace nsp{
	namespace proto{

		typedef struct _navi_task_staus_upl_t {

			nsp::proto::proto_crt_t<int>		edge_id_;
			nsp::proto::proto_crt_t<double>	    percentage_;
			nsp::proto::proto_crt_t<int>		wop_id_;


			unsigned char * serialize(unsigned char* pkt_bytes) const
			{
				unsigned char * pos = pkt_bytes;
				pos = edge_id_.serialize(pos);
				pos = percentage_.serialize(pos);
				pos = wop_id_.serialize(pos);
				return pos;
			}

			const unsigned char* build(const unsigned char*pkt_bytes, int &cb)
			{
				const unsigned char * pos = pkt_bytes;
				pos = edge_id_.build(pos, cb);
				pos = percentage_.build(pos, cb);
				pos = wop_id_.build(pos, cb);
				return pos;
			}

			const int length() const 
			{
				return 2 * sizeof(int) + sizeof(double);
			}

		}navi_task_staus_upl_t;
		
		typedef struct _navi_task_staus_position {
			nsp::proto::proto_crt_t<double> x_;
			nsp::proto::proto_crt_t<double> y_;
			nsp::proto::proto_crt_t<double> w_;
			unsigned char * serialize(unsigned char* pkt_bytes) const
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

			const int length() const
			{
				return sizeof(double) * 3;
			}

		}navi_task_status_position_t;

		

		typedef struct proto_query_navigation_task_status_ack :public nsp::proto::proto_interface{

			proto_query_navigation_task_status_ack() {}

			nsp::proto::proto_head head;
			nsp::proto::proto_crt_t<int>       traj_index_current_;
			nsp::proto::proto_crt_t<int>       track_eno_;
			nsp::proto::proto_crt_t<prc_t>	   track_prc_;
			navi_task_status_position_t		   pos_;
			nsp::proto::proto_crt_t<uint64_t>  pos_time_stamp_;
			nsp::proto::proto_crt_t<double>	   pos_confidence_;
			navi_task_staus_upl_t			   upl_;
			nsp::proto::proto_crt_t<int> 	   vehcile_control_mode_;

			unsigned char * serialize(unsigned char* pkt_bytes)const  override
			{
				unsigned char * pos = pkt_bytes;
				pos = head.serialize(pos);
				pos = traj_index_current_.serialize(pos);
				pos = track_eno_.serialize(pos);
				pos = track_prc_.serialize(pos);
				pos = pos_.serialize(pos);
				pos = pos_time_stamp_.serialize(pos);
				pos = pos_confidence_.serialize(pos);
				pos = upl_.serialize(pos);
				pos = vehcile_control_mode_.serialize(pos);
				return pos;
			}

			const unsigned char* build(const unsigned char*pkt_bytes, int &cb) override
			{
				const unsigned char * pos = pkt_bytes;
				pos = head.build(pos, cb);
				pos = traj_index_current_.build(pos, cb);
				pos = track_eno_.build(pos, cb);
				pos = track_prc_.build(pos, cb);
				pos = pos_.build(pos, cb);
				pos = pos_time_stamp_.build(pos, cb);
				pos = pos_confidence_.build(pos, cb);
				pos = upl_.build(pos, cb);
				pos = vehcile_control_mode_.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return sizeof(int) * 3 + track_prc_.length() + pos_.length() + sizeof(uint64_t) + sizeof(double) + upl_.length() + head.length();
			}


		}proto_query_navigation_task_status_ack_t;

	}
}






#endif