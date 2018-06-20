
# pragma once
#include "proto_definion.h"


namespace nsp {
	namespace proto {

		typedef struct
		{
			nsp::proto::proto_crt_t<int> id_;
			nsp::proto::proto_crt_t<uint32_t> type_;

			unsigned char *serialize(unsigned char * byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = id_.serialize(pos);
				pos = type_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb)
			{
				const unsigned char *pos = byte_stream;
				pos = id_.build(pos, cb);
				pos = type_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return sizeof(int) + sizeof(type_);
			}

		}var_report_item_t;


		typedef struct proto_var_report_items :public nsp::proto::proto_interface
		{
			proto_var_report_items() {}
			nsp::proto::proto_head head;
			nsp::proto::proto_vector_t<var_report_item_t> items;

			unsigned char *serialize(unsigned char * byte_stream) const override
			{
				unsigned char *pos = byte_stream;
				pos = head.serialize(pos);
				pos = items.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb) override
			{
				const unsigned char *pos = byte_stream;
				pos = head.build(pos, cb);
				pos = items.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return  items.length() +head.length();
			}
		}proto_var_report_items_t;

		struct proto_get_mtver_ack :public nsp::proto::proto_interface
		{
			nsp::proto::proto_head head;
			proto_crt_t<uint16_t> major;
			proto_crt_t<uint16_t> sub;
			proto_crt_t<uint16_t> extended;
			proto_string_t<char> uname;
			proto_string_t <char> vcu;

			unsigned char *serialize(unsigned char * byte_stream) const override
			{
				unsigned char *pos = byte_stream;
				pos = head.serialize(pos);
				pos = major.serialize(pos);
				pos = sub.serialize(pos);
				pos = extended.serialize(pos);
				pos = uname.serialize(pos);
				pos = vcu.serialize(pos);
				return pos;
			}

			const unsigned char *build(const  unsigned char * byte_stream, int &cb) override
			{
				const unsigned char *pos = byte_stream;
				pos = head.build(pos, cb);
				pos = major.build(pos, cb);
				pos = sub.build(pos, cb);
				pos = extended.build(pos, cb);
				pos = uname.build(pos, cb);
				pos = vcu.build(pos, cb);
				return pos;
			}

			const int length() const override
			{
				return  head.length() + major.length() + sub.length() + extended.length() + uname.length() + vcu.length();
			}
		};

	}
}