#pragma once

#include "agv_shell_proto.hpp"
#include "serialize.hpp"

namespace nsp{
	namespace proto{
#pragma pack(push, 1)
		typedef struct proto_pre_login_agv : public nsp::proto::proto_interface {
			proto_pre_login_agv(uint32_t type) :head_(type) {}
			proto_pre_login_agv(){}
			~proto_pre_login_agv() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_string_t<char> key_;

			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = bytes;
				pos = head_.serialize(pos);
				pos = key_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = bytes;
				pos = head_.build(pos, cb);
				pos = key_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + key_.length();
			}
		} proto_pre_login_agv_t;
		
		typedef struct proto_login_agv : public nsp::proto::proto_interface {
			proto_login_agv(uint32_t type) :head_(type) {}
			proto_login_agv(){}
			~proto_login_agv() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int> acct_; // role
			nsp::proto::proto_crt_t<int> access_;
			nsp::proto::proto_string_t<char> original_buffer_;
			nsp::proto::proto_string_t<char> encrypted_md5_;

			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = bytes;
				pos = head_.serialize(pos);
				pos = acct_.serialize(pos);
				pos = access_.serialize(pos);
				pos = original_buffer_.serialize(pos);
				pos = encrypted_md5_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = bytes;
				pos = head_.build(pos, cb);
				pos = acct_.build(pos, cb);
				pos = access_.build(pos, cb);
				pos = original_buffer_.build(pos, cb);
				pos = encrypted_md5_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + access_.length() + acct_.length() + original_buffer_.length() + encrypted_md5_.length();
			}
		} proto_login_agv_t;

		typedef struct proto_msg_int :public nsp::proto::proto_interface {

			proto_msg_int(){}
			proto_msg_int(int type) : head_(type){}
			~proto_msg_int(){}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int> msg_int_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = msg_int_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = msg_int_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return msg_int_.length() + head_.length();
			}
		}proto_msg_int_t;

		typedef struct proto_msg : public nsp::proto::proto_interface{

			proto_msg(){}
			proto_msg(int type) : head_(type){}
			~proto_msg(){}

			nsp::proto::proto_head head_;
			nsp::proto::proto_string_t<char> msg_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = msg_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = msg_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return msg_.length() + head_.length();
			}
		}proto_msg_t;

		typedef struct proto_process : public nsp::proto::proto_interface{
			proto_process(){};
			~proto_process(){};

			nsp::proto::proto_crt_t<int>  process_id_;
			nsp::proto::proto_string_t<char> process_name_;
			nsp::proto::proto_string_t<char> process_path_;
			nsp::proto::proto_string_t<char> process_cmd_;
			nsp::proto::proto_crt_t<int> process_delay_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = process_id_.serialize(pos);
				pos = process_name_.serialize(pos);
				pos = process_path_.serialize(pos);
				pos = process_cmd_.serialize(pos);
				pos = process_delay_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = process_id_.build(pos, cb);
				pos = process_name_.build(pos, cb);
				pos = process_path_.build(pos, cb);
				pos = process_cmd_.build(pos, cb);
				pos = process_delay_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return process_id_.length() + process_name_.length() + process_path_.length() + process_cmd_.length() + process_delay_.length();
			}
		}proto_process_t;

		typedef struct proto_process_list_reponse : public nsp::proto::proto_interface {

			proto_process_list_reponse() {}
			proto_process_list_reponse(int type) : head_(type) {}
			~proto_process_list_reponse() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int> pkt_id_;
			nsp::proto::proto_vector_t<proto_process> process_list_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = pkt_id_.serialize(pos);
				pos = process_list_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = pkt_id_.build(pos, cb);
				pos = process_list_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return process_list_.length() + head_.length() + pkt_id_.length();
			}

		} proto_process_list_reponse_t;

		typedef struct proto_set_keepalive_status : public nsp::proto::proto_interface {

			proto_set_keepalive_status() {};
			proto_set_keepalive_status(int type) : head_(type) {}
			~proto_set_keepalive_status() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int> keepalive_status_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = keepalive_status_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = keepalive_status_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return keepalive_status_.length() + head_.length();
			}

		} proto_set_keepalive_status_t;

		typedef struct proto_keepalive_status_reponse : public nsp::proto::proto_interface {

			proto_keepalive_status_reponse() {};
			proto_keepalive_status_reponse(int type) : head_(type) {}
			~proto_keepalive_status_reponse() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int> status_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = status_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = status_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return status_.length() + head_.length();
			}

		} proto_keepalive_status_reponse_t;

		typedef struct proto_command_process : public nsp::proto::proto_interface{
			proto_command_process(){}
			proto_command_process(int type) : head_(type) {}
			~proto_command_process(){}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int> command_;
			nsp::proto::proto_crt_t<int> process_id_all_;
			nsp::proto::proto_vector_t<nsp::proto::proto_string_t<char> > list_param_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = command_.serialize(pos);
				pos = process_id_all_.serialize(pos);
				pos = list_param_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = command_.build(pos, cb);
				pos = process_id_all_.build(pos, cb);
				pos = list_param_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return command_.length() + head_.length() + list_param_.length() + process_id_all_.length();
			}
		}proto_command_process_t;

		typedef struct proto_common_stream : public nsp::proto::proto_interface {
			proto_common_stream() {}
			proto_common_stream(int type) : head_(type) {}
			~proto_common_stream() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int> pkt_id;
			nsp::proto::proto_string_t<char> common_stream;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = pkt_id.serialize(pos);
				pos = common_stream.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = pkt_id.build(pos, cb);
				pos = common_stream.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return pkt_id.length() + head_.length() + common_stream.length();
			}
		}proto_common_stream_t;
		
		typedef struct proto_local_info : public nsp::proto::proto_interface {
			proto_local_info() {}
			proto_local_info(int type) : head_(type) {}
			~proto_local_info() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int> agv_port_;
			nsp::proto::proto_crt_t<int> fts_port_;
			nsp::proto::proto_string_t<char> mac_addr_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = agv_port_.serialize(pos);
				pos = fts_port_.serialize(pos);
				pos = mac_addr_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = agv_port_.build(pos, cb);
				pos = fts_port_.build(pos, cb);
				pos = mac_addr_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + agv_port_.length() + fts_port_.length() + mac_addr_.length();
			}
		}proto_local_info_t;

		typedef struct proto_mac_info : public nsp::proto::proto_interface {
			proto_mac_info() {}
			~proto_mac_info() {}

			nsp::proto::proto_string_t<char> ipv4_;
			nsp::proto::proto_crt_t<int> agv_port_;
			nsp::proto::proto_crt_t<int> fts_port_;
			nsp::proto::proto_string_t<char> mac_addr_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = ipv4_.serialize(pos);
				pos = agv_port_.serialize(pos);
				pos = fts_port_.serialize(pos);
				pos = mac_addr_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = ipv4_.build(pos, cb);
				pos = agv_port_.build(pos, cb);
				pos = fts_port_.build(pos, cb);
				pos = mac_addr_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return ipv4_.length() + agv_port_.length() + fts_port_.length() + mac_addr_.length();
			}
		}proto_mac_info_t;

		typedef struct proto_mac_vec : public nsp::proto::proto_interface{
			proto_mac_vec() {}
			proto_mac_vec(int type) : head_(type) {}
			~proto_mac_vec() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int>  pkt_id;
			nsp::proto::proto_vector_t<proto_mac_info> vct_mac_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = pkt_id.serialize(pos);
				pos = vct_mac_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = pkt_id.build(pos, cb);
				pos = vct_mac_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + pkt_id.length() + vct_mac_.length();
			}
		}proto_mac_vec_t;

		typedef struct proto_log_type : public nsp::proto::proto_interface{
			proto_log_type() {}
			proto_log_type(int type) : head_(type) {}
			~proto_log_type() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_string_t<char> log_type_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = log_type_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = log_type_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + log_type_.length();
			}
		}proto_log_type_t;

		typedef struct proto_log_type_vct : public nsp::proto::proto_interface{
			proto_log_type_vct() {}
			proto_log_type_vct(int type) : head_(type) {}
			~proto_log_type_vct() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_vector_t<proto_log_type> vct_log_type_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = vct_log_type_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = vct_log_type_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + vct_log_type_.length();
			}
		}proto_log_type_vct_t;

		typedef struct proto_log_condition : public nsp::proto::proto_interface{
			proto_log_condition() {}
			proto_log_condition(int type) : head_(type) {}
			~proto_log_condition() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<unsigned int> task_id = 0;
			nsp::proto::proto_string_t<char> start_time;
			nsp::proto::proto_string_t<char> end_time;
			nsp::proto::proto_vector_t<proto_log_type> vct_log_type_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = task_id.serialize(pos);
				pos = start_time.serialize(pos);
				pos = end_time.serialize(pos);
				pos = vct_log_type_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = task_id.build(pos, cb);
				pos = start_time.build(pos, cb);
				pos = end_time.build(pos,cb);
				pos = vct_log_type_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + task_id.length() + start_time.length() + end_time.length() + vct_log_type_.length();
			}
		}proto_log_condition_t;

		typedef struct proto_logs_file_path : public nsp::proto::proto_interface{
			proto_logs_file_path() {}
			proto_logs_file_path(int type) : head_(type) {}
			~proto_logs_file_path() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<unsigned int> task_id = 0;
			nsp::proto::proto_vector_t<nsp::proto::proto_string_t<char> > vct_log_file_name_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = task_id.serialize(pos);
				pos = vct_log_file_name_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = task_id.build(pos, cb);
				pos = vct_log_file_name_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + task_id.length() + vct_log_file_name_.length();
			}
		}proto_logs_file_path_t;
		
		typedef struct proto_logs_cancel : public nsp::proto::proto_interface{
			proto_logs_cancel() {}
			proto_logs_cancel(int type) : head_(type) {}
			~proto_logs_cancel() {}

			nsp::proto::proto_head head_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length();
			}
		}proto_logs_cancel_t;

		typedef struct proto_msg_int_sync : public nsp::proto::proto_interface{
			proto_msg_int_sync() {}
			proto_msg_int_sync(int type) : head_(type) {}
			~proto_msg_int_sync() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_crt_t<int> pkt_id;
			nsp::proto::proto_crt_t<int> msg_int;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = pkt_id.serialize(pos);
				pos = msg_int.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = pkt_id.build(pos, cb);
				pos = msg_int.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + pkt_id.length() + msg_int.length();
			}
		}proto_msg_int_sync_t;


		typedef struct proto_shell_version :public nsp::proto::proto_interface
		{
			proto_shell_version() {}
			proto_shell_version(int type) : head_(type) {}
			~proto_shell_version() {}

			nsp::proto::proto_head head_;
			nsp::proto::proto_string_t<char>version_;
			

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = version_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = version_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head_.length() + version_.length();
			}
		}proto_shell_version_t;
#pragma pack(pop)
	}
}