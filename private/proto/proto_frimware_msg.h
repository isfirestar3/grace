#pragma once

#include <memory>
#include "serialize.hpp"
#include "agv_shell_proto.hpp"

namespace nsp{
	namespace proto{
		typedef struct proto_frimware_info_request : public nsp::proto::proto_interface
		{
			proto_frimware_info_request(int type) : head_(type) {}
			proto_frimware_info_request(){}
			~proto_frimware_info_request(){}

			agv_shell_proto_head head_;
			nsp::proto::proto_crt_t<int> frimware_type_;
			nsp::proto::proto_string_t<char> target_endpoint_;

			const int length() const
			{
				return head_.length() + frimware_type_.length() + target_endpoint_.length();
			}

			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = head_.serialize(bytes);
				pos = frimware_type_.serialize(pos);
				pos = target_endpoint_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = head_.build(bytes, cb);
				pos = frimware_type_.build(pos, cb);
				pos = target_endpoint_.build(pos, cb);
				return pos;
			}
		}proto_frimware_info_request_t;

		typedef struct proto_frimware_info_response : public nsp::proto::proto_interface
		{
			proto_frimware_info_response(int type) : head_(type) {}
			proto_frimware_info_response(){}
			~proto_frimware_info_response(){}

			nsp::proto::agv_shell_proto_head head_;
			nsp::proto::proto_crt_t<int> frimware_status;
			nsp::proto::proto_crt_t<int> frimware_type_;
			nsp::proto::proto_crt_t<int> vcu_type_;
			nsp::proto::proto_string_t<char> frimware_msg_;

			const int length() const
			{
				return head_.length() + frimware_status.length() + frimware_type_.length() + vcu_type_.length() + frimware_msg_.length();
			}

			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = head_.serialize(bytes);
				pos = frimware_status.serialize(pos);
				pos = frimware_type_.serialize(pos);
				pos = vcu_type_.serialize(pos);
				pos = frimware_msg_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = head_.build(bytes, cb);
				pos = frimware_status.build(pos, cb);
				pos = frimware_type_.build(pos, cb);
				pos = vcu_type_.build(pos, cb);
				pos = frimware_msg_.build(pos, cb);
				return pos;
			}
		}proto_frimware_info_response_t;

		typedef struct proto_frimware_update : public nsp::proto::proto_interface
		{
			proto_frimware_update(int type) : head_(type){}
			proto_frimware_update(){}
			~proto_frimware_update(){}

			nsp::proto::agv_shell_proto_head head_;
			nsp::proto::proto_crt_t<int> frimware_type_;
			nsp::proto::proto_string_t<char> frimware_path_;
			nsp::proto::proto_string_t<char> target_endpoint_;

			const int length() const
			{
				return head_.length() + frimware_type_.length() + frimware_path_.length() + target_endpoint_.length();
			}

			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = head_.serialize(bytes);
				pos = frimware_type_.serialize(pos);
				pos = frimware_path_.serialize(pos);
				pos = target_endpoint_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = head_.build(bytes, cb);
				pos = frimware_type_.build(pos, cb);
				pos = frimware_path_.build(pos, cb);
				pos = target_endpoint_.build(pos, cb);
				return pos;
			}
		}proto_frimware_update_t;

		typedef struct proto_frimware_update_response : public nsp::proto::proto_interface
		{
			proto_frimware_update_response(int type) : head_(type){}
			proto_frimware_update_response(){}
			~proto_frimware_update_response(){}

			nsp::proto::agv_shell_proto_head head_;
			nsp::proto::proto_crt_t<int> frimware_type_;
			nsp::proto::proto_crt_t<int> frimware_step;
			nsp::proto::proto_crt_t<int> frimware_status;

			const int length() const
			{
				return head_.length() + frimware_type_.length() + frimware_step.length() + frimware_status.length();
			}

			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = head_.serialize(bytes);
				pos = frimware_type_.serialize(pos);
				pos = frimware_step.serialize(pos);
				pos = frimware_status.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = head_.build(bytes, cb);
				pos = frimware_type_.build(pos, cb);
				pos = frimware_step.build(pos, cb);
				pos = frimware_status.build(pos, cb);
				return pos;
			}
		}proto_frimware_update_response_t;

		typedef struct proto_frimware_download_request : public nsp::proto::proto_interface
		{
			proto_frimware_download_request(int type) : head_(type){}
			proto_frimware_download_request(){}
			~proto_frimware_download_request(){}

			nsp::proto::agv_shell_proto_head head_;
			nsp::proto::proto_crt_t<int> frimware_type_;
			nsp::proto::proto_crt_t<int> frimware_length_;
			nsp::proto::proto_string_t<char> target_endpoint_;

			const int length() const
			{
				return head_.length() + frimware_type_.length() + frimware_length_.length() + target_endpoint_.length();
			}

			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = head_.serialize(bytes);
				pos = frimware_type_.serialize(pos);
				pos = frimware_length_.serialize(pos);
				pos = target_endpoint_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = head_.build(bytes, cb);
				pos = frimware_type_.build(pos, cb);
				pos = frimware_length_.build(pos, cb);
				pos = target_endpoint_.build(pos, cb);
				return pos;
			}
		}proto_frimware_download_request_t;

		typedef struct proto_frimware_download_reponse : public nsp::proto::proto_interface
		{
			proto_frimware_download_reponse(int type) : head_(type){}
			proto_frimware_download_reponse(){}
			~proto_frimware_download_reponse(){}

			nsp::proto::agv_shell_proto_head head_;
			nsp::proto::proto_string_t<char> frimware_path_;

			const int length() const
			{
				return head_.length() + frimware_path_.length();
			}

			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = head_.serialize(bytes);
				pos = frimware_path_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = head_.build(bytes, cb);
				pos = frimware_path_.build(pos, cb);
				return pos;
			}
		}proto_frimware_download_reponse_t;

		typedef struct proto_msg_str : public nsp::proto::proto_interface {

			proto_msg_str() {};
			proto_msg_str(int type) : head_(type) {}
			~proto_msg_str() {}

			nsp::proto::agv_shell_proto_head head_;
			nsp::proto::proto_crt_t<int> msg_int_;
			nsp::proto::proto_string_t<char>  msg_str_;

			unsigned char * serialize(unsigned char *byte_stream) const
			{
				unsigned char *pos = byte_stream;
				pos = head_.serialize(pos);
				pos = msg_int_.serialize(pos);
				pos = msg_str_.serialize(pos);
				return pos;
			}

			const unsigned char * build(const unsigned char *byte_stream, int &cb)
			{
				unsigned const char *pos = byte_stream;
				pos = head_.build(pos, cb);
				pos = msg_int_.build(pos, cb);
				pos = msg_str_.build(pos, cb);
				return pos;
			}

			const int length() const
			{
				return msg_int_.length() + msg_str_.length() + head_.length();
			}

		}proto_msg_str_t;

		typedef struct proto_frimware_custom_update : public nsp::proto::proto_interface
		{
			proto_frimware_custom_update(int type) : head_(type) {}
			proto_frimware_custom_update() {}
			~proto_frimware_custom_update() {}

			nsp::proto::agv_shell_proto_head head_;
			nsp::proto::proto_crt_t<int> node_id;
			nsp::proto::proto_crt_t<int> serial_type;
			nsp::proto::proto_string_t<char> frimware_path_;
			nsp::proto::proto_string_t<char> target_endpoint_;

			const int length() const
			{
				return head_.length() + node_id.length() + serial_type.length() + frimware_path_.length() + target_endpoint_.length();
			}

			unsigned char *serialize(unsigned char *bytes) const
			{
				unsigned char *pos = head_.serialize(bytes);
				pos = node_id.serialize(pos);
				pos = serial_type.serialize(pos);
				pos = frimware_path_.serialize(pos);
				pos = target_endpoint_.serialize(pos);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = head_.build(bytes, cb);
				pos = node_id.build(pos, cb);
				pos = serial_type.build(pos, cb);
				pos = frimware_path_.build(pos, cb);
				pos = target_endpoint_.build(pos, cb);
				return pos;
			}
		}proto_frimware_custom_update_t;

		template<class T, uint32_t _Type>
		class package : public agv_shell_proto_head {
			T body_;
		public:
			package(const T &body_ref) : body_(body_ref), agv_shell_proto_head(_Type) {
			}
			package() : agv_shell_proto_head(_Type) {
			}
			~package() {}
			virtual const int length() const override {
				return agv_shell_proto_head::length() + body_.length();
			}
			virtual unsigned char *serialize(unsigned char *bytes) const override {
				unsigned char *pos = agv_shell_proto_head::serialize(bytes);
				return body_.serialize(pos);
			}
			virtual const unsigned char *build(const unsigned char *bytes, int &cb) override {
				return nullptr;
			}
			int from_body_stream(const unsigned char *bytes, const int length) {
				int cb = length;
				return ((nullptr == body_.build(bytes, cb)) ? (-1) : (0));
			}

			T *operator->() { return &body_; }
			T &operator*() { return body_; }
		};
	}
}
