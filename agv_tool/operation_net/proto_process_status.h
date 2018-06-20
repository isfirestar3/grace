#pragma once

#include "serialize.hpp"

namespace nsp {
	namespace proto {

		typedef struct process_relation : public nsp::proto::proto_interface{
			process_relation(){};
			~process_relation(){};

			nsp::proto::proto_crt_t<int>  process_id_;
			nsp::proto::proto_string_t<char> process_name_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = process_id_.serialize(pos);
				pos = process_name_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = process_id_.build(pos, cb);
				pos = process_name_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return process_id_.length() + process_name_.length();
			}
		}process_relation_t;

		typedef struct process_list : public nsp::proto::proto_interface {

			process_list() {}
			~process_list() {}

			nsp::proto::proto_vector_t<process_relation> process_list_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = process_list_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = process_list_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return process_list_.length();
			}
		}process_list_t;

		typedef struct process_status {
			process_status() {};
			~process_status() {};

			nsp::proto::proto_crt_t<int>  process_id_;
			nsp::proto::proto_crt_t<int>  process_status_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = process_id_.serialize(pos);
				pos = process_status_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = process_id_.build(pos, cb);
				pos = process_status_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return process_id_.length() + process_status_.length();
			}
		}process_status_t;

		typedef struct process_status_list {
			process_status_list() {}
			~process_status_list() {}

			nsp::proto::proto_vector_t<process_status> process_list_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = process_list_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = process_list_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return process_list_.length();
			}

		} process_status_list_t;
	}
}