#pragma once

#include "proto_definion.h"
#include "proto_agv_msg.h"
#include "serialize.hpp"

namespace agv{
	namespace proto{
#pragma pack(push, 1)
		typedef struct proto_cpu_info : public nsp::proto::proto_interface {
			proto_cpu_info(){}
			~proto_cpu_info() {}

			nsp::proto::proto_string_t<char>  name;
			nsp::proto::proto_crt_t<uint64_t>  hz;	//MHz
			
			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = bytes;
				pos = name.serialize(pos);
				pos = hz.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = bytes;
				pos = name.build(pos, cb);
				pos = hz.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return name.length() + hz.length();
			}
		} proto_cpu_info_t;
		
		typedef struct proto_process_run_info : public nsp::proto::proto_interface {
			proto_process_run_info(){}
			~proto_process_run_info() {}

			nsp::proto::proto_string_t<char>  name;
			nsp::proto::proto_crt_t<uint32_t>  pid;
			nsp::proto::proto_string_t<char>  run_time;	//5-03:08:29
			nsp::proto::proto_crt_t<uint64_t>  vir_mm;		//KB
			nsp::proto::proto_crt_t<uint64_t>  rss;			//KB
			nsp::proto::proto_crt_t<uint64_t>  average_cpu;
			nsp::proto::proto_crt_t<uint64_t>  average_mem;
			
			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = bytes;
				pos = name.serialize(pos);
				pos = pid.serialize(pos);
				pos = run_time.serialize(pos);
				pos = vir_mm.serialize(pos);
				pos = rss.serialize(pos);
				pos = average_cpu.serialize(pos);
				pos = average_mem.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = bytes;
				pos = name.build(pos, cb);
				pos = pid.build(pos, cb);
				pos = run_time.build(pos, cb);
				pos = vir_mm.build(pos, cb);
				pos = rss.build(pos, cb);
				pos = average_cpu.build(pos, cb);
				pos = average_mem.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return name.length() + pid.length() + run_time.length() + vir_mm.length() + rss.length() + average_cpu.length() + average_mem.length();
			}
		} proto_process_run_info_t; //动态 
		
		typedef struct proto_process_info : public nsp::proto::proto_interface{
			proto_process_info(){};
			~proto_process_info(){};

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
		} proto_process_info_t; // 静态配置 
		
		typedef struct proto_sysinfo_fixed : public nsp::proto::proto_interface {
			proto_sysinfo_fixed(uint32_t type) :head(type) {}
			proto_sysinfo_fixed(){}
			~proto_sysinfo_fixed() {}

			nsp::proto::proto_head head;
			nsp::proto::proto_crt_t<uint64_t>  total_mem;	//Byte
			nsp::proto::proto_crt_t<uint32_t>  cpu_num;
			nsp::proto::proto_vector_t<proto_cpu_info_t>  cpu_list;
			nsp::proto::proto_crt_t<uint64_t>  disk_total_size;	//KB
			nsp::proto::proto_string_t<char>  uname;
			nsp::proto::proto_string_t<char>  soft_version;
			nsp::proto::proto_string_t<char>  config_version;
			nsp::proto::proto_vector_t<proto_process_info_t>  pocess_info;
			nsp::proto::proto_crt_t<int> status;  //锁状态 
			nsp::proto::proto_string_t<char>  ntp_server;

			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = bytes;
				pos = head.serialize(pos);
				pos = total_mem.serialize(pos);
				pos = cpu_num.serialize(pos);
				pos = cpu_list.serialize(pos);
				pos = disk_total_size.serialize(pos);
				pos = uname.serialize(pos);
				pos = soft_version.serialize(pos);
				pos = config_version.serialize(pos);
				pos = pocess_info.serialize(pos);
				pos = status.serialize(pos);
				pos = ntp_server.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = bytes;
				pos = head.build(pos, cb);
				pos = total_mem.build(pos, cb);
				pos = cpu_num.build(pos, cb);
				pos = cpu_list.build(pos, cb);
				pos = disk_total_size.build(pos, cb);
				pos = uname.build(pos, cb);
				pos = soft_version.build(pos, cb);
				pos = config_version.build(pos, cb);
				pos = pocess_info.build(pos, cb);
				pos = status.build(pos, cb);
				pos = ntp_server.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head.length() + total_mem.length() + cpu_num.length() + cpu_list.length() + disk_total_size.length() \
						+ uname.length() + soft_version.length() + config_version.length() + pocess_info.length() + status.length() \
						+ ntp_server.length();
			}
		} proto_sysinfo_fixed_t;
		
		typedef struct proto_sysinfo_changed : public nsp::proto::proto_interface {
			proto_sysinfo_changed(){}
			~proto_sysinfo_changed() {}

			nsp::proto::proto_crt_t<uint32_t>  cpu_percentage;
			nsp::proto::proto_crt_t<uint64_t>  free_mem;	//Byte
			nsp::proto::proto_crt_t<uint64_t>  total_swap;	//Byte
			nsp::proto::proto_crt_t<uint64_t>  free_swap;	//Byte
			nsp::proto::proto_crt_t<uint64_t>  disk_used_size;	//KB
			nsp::proto::proto_crt_t<uint64_t>  uptime;
			nsp::proto::proto_crt_t<uint64_t>  host_time;
			nsp::proto::proto_crt_t<uint64_t>  net_io_rec;		//bytes
			nsp::proto::proto_crt_t<uint64_t>  net_io_tra;		//bytes
			
			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = bytes;
				pos = cpu_percentage.serialize(pos);
				pos = free_mem.serialize(pos);
				pos = total_swap.serialize(pos);
				pos = free_swap.serialize(pos);
				pos = disk_used_size.serialize(pos);
				pos = uptime.serialize(pos);
				pos = host_time.serialize(pos);
				pos = net_io_rec.serialize(pos);
				pos = net_io_tra.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = bytes;
				pos = cpu_percentage.build(pos, cb);
				pos = free_mem.build(pos, cb);
				pos = total_swap.build(pos, cb);
				pos = free_swap.build(pos, cb);
				pos = disk_used_size.build(pos, cb);
				pos = uptime.build(pos, cb);
				pos = host_time.build(pos, cb);
				pos = net_io_rec.build(pos, cb);
				pos = net_io_tra.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return cpu_percentage.length() + free_mem.length() + total_swap.length() + free_swap.length() + \
						disk_used_size.length() + uptime.length() + host_time.length() + net_io_rec.length() + net_io_tra.length();
			}
		} proto_sysinfo_changed_t;
		
		typedef struct proto_keepalive_ack : public nsp::proto::proto_interface {
			proto_keepalive_ack(uint32_t type) :head(type) {}
			proto_keepalive_ack(){}
			~proto_keepalive_ack() {}

			nsp::proto::proto_head  head;
			proto_sysinfo_changed_t  sysinfo;
			nsp::proto::proto_vector_t<proto_process_run_info_t>  process_list;
			nsp::proto::proto_string_t<char>  reserve;
			
			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = bytes;
				pos = head.serialize(pos);
				pos = sysinfo.serialize(pos);
				pos = process_list.serialize(pos);
				pos = reserve.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = bytes;
				pos = head.build(pos, cb);
				pos = sysinfo.build(pos, cb);
				pos = process_list.build(pos, cb);
				pos = reserve.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return head.length() + sysinfo.length() + reserve.length() + reserve.length();
			}
		} proto_keepalive_ack_t;
#pragma pack(pop)
	}
}
