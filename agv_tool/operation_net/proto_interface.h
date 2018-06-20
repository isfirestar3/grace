#pragma once

#include "serialize.hpp"

namespace nsp {
	namespace api {

		typedef struct proto_file_info{
			nsp::proto::proto_string_t<char> file_name;
			nsp::proto::proto_crt_t<int> file_type;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = file_name.serialize(pos);
				pos = file_type.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = file_name.build(pos, cb);
				pos = file_type.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return file_name.length() + file_type.length();
			}
		}proto_file_info_t;

		typedef struct proto_file_list {
			nsp::proto::proto_vector_t<proto_file_info> file_list;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = file_list.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = file_list.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return file_list.length();
			}
		}proto_file_list_t;

		typedef struct proto_msg{
			nsp::proto::proto_string_t<char> msg;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = msg.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = msg.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return msg.length();
			}
		}proto_msg_t;

		typedef struct proto_msg_int {
			nsp::proto::proto_crt_t<int> msg_id;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = msg_id.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = msg_id.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return msg_id.length();
			}
		}proto_msg_int_t;

		typedef struct proto_msg_list {
			nsp::proto::proto_vector_t<proto_msg> msg_list;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = msg_list.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = msg_list.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return msg_list.length();
			}
		}proto_msg_list_t;

		typedef struct proto_push_list
		{
			proto_msg_list   local_file_list;
			nsp::proto::proto_string_t<char>   remote_relative_path;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = local_file_list.serialize(pos);
				pos = remote_relative_path.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = local_file_list.build(pos, cb);
				pos = remote_relative_path.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return local_file_list.length() + remote_relative_path.length();
			}
		}proto_push_list_t;

		typedef struct proto_linux_info{

			nsp::proto::proto_string_t<char> robot_time_;
			nsp::proto::proto_crt_t<int> vcu_enable_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = robot_time_.serialize(pos);
				pos = vcu_enable_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = robot_time_.build(pos, cb);
				pos = vcu_enable_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return robot_time_.length() + vcu_enable_.length();
			}
		}proto_linux_info_t;

		typedef struct proto_interface_stream {
			proto_interface_stream() {}
			~proto_interface_stream() {}

			nsp::proto::proto_crt_t<int> pkt_id;
			nsp::proto::proto_string_t<char> common_stream;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = pkt_id.serialize(pos);
				pos = common_stream.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = pkt_id.build(pos, cb);
				pos = common_stream.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return pkt_id.length() + common_stream.length();
			}
		}proto_common_stream_t;

		typedef struct proto_dhcp_info {
			proto_dhcp_info() {}
			~proto_dhcp_info() {}

			nsp::proto::proto_string_t<char> ipv4_;
			nsp::proto::proto_crt_t<int> agv_port_;
			nsp::proto::proto_crt_t<int> fts_port_;
			nsp::proto::proto_string_t<char> mac_addr_;
			nsp::proto::proto_crt_t<int> vhid_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = ipv4_.serialize(pos);
				pos = agv_port_.serialize(pos);
				pos = fts_port_.serialize(pos);
				pos = mac_addr_.serialize(pos);
				pos = vhid_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = ipv4_.build(pos, cb);
				pos = agv_port_.build(pos, cb);
				pos = fts_port_.build(pos, cb);
				pos = mac_addr_.build(pos, cb);
				pos = vhid_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return ipv4_.length() + agv_port_.length() + fts_port_.length() + mac_addr_.length() + vhid_.length();
			}
		}proto_dhcp_info_t;

		typedef struct proto_agvinfo_vec {
			proto_agvinfo_vec() {}
			~proto_agvinfo_vec() {}

			nsp::proto::proto_vector_t<proto_dhcp_info> vct_mac_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = vct_mac_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = vct_mac_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return vct_mac_.length();
			}
		}proto_agvinfo_vec_t;

		typedef struct proto_backup_item {
			proto_backup_item() {}
			~proto_backup_item() {}

			nsp::proto::proto_crt_t<int> robot_id_;
			nsp::proto::proto_crt_t<int> car_id_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = robot_id_.serialize(pos);
				pos = car_id_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = robot_id_.build(pos, cb);
				pos = car_id_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return robot_id_.length() + car_id_.length();
			}
		}proto_backup_item_t;

		typedef struct proto_backups_info {
			proto_backups_info() {}
			~proto_backups_info() {}

			nsp::proto::proto_vector_t<proto_backup_item> vct_robot_item_;
			nsp::proto::proto_string_t<char> local_path_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = vct_robot_item_.serialize(pos);
				pos = local_path_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = vct_robot_item_.build(pos, cb);
				pos = local_path_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return vct_robot_item_.length() + local_path_.length();
			}
		}proto_backups_info_t;
	}
}
