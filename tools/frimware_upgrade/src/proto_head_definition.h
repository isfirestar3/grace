#ifndef PROTO_HEAD_UDP_DEFINITION_20170717
#define PROTO_HEAD_UDP_DEFINITION_20170717

#ifdef _MSC_VER // for MSVC
#define forceinline __forceinline
#elif defined __GNUC__ // for gcc on Linux/Apple OS X
#define forceinline __inline__ __attribute__((always_inline))
#else
#define forceinline
#endif

#include <memory>

#ifdef __cplusplus
#include "serialize.hpp"
#endif

namespace nsp{
	namespace proto{
		namespace udp{

			//单字节对齐
			#pragma pack(push, 1)
			//协议头
			struct udp_head : public nsp::proto::proto_interface{
				//序列号
				unsigned short int sequence_id_ = 0x0000;
				//版本号
				unsigned char version_ = 0x00;
				//预留
				unsigned char reserver_first_ = 0x00;
				//命令码
				unsigned char cmd_ = 0x06;
				//预留
				unsigned char reserver_second_[3];
				//主操作码
				unsigned char operate_ = 0x00;
				//副操作码
				unsigned char sub_operate_ = 0x00;
				//操作参数
				unsigned short operate_parament_ = 0x0000;

				udp_head(unsigned short int sequence = 0,unsigned char version =0x00,unsigned char cmd=0x00,
					unsigned char op = 0x00,unsigned char sub_op = 0x00,unsigned short operate_parament = 0){
					sequence_id_ = sequence;
					version_ = version;
					cmd_ = cmd;
					operate_ = op;
					sub_operate_ = sub_op;
					operate_parament_ = operate_parament;
					memset(reserver_second_, 0, sizeof(reserver_second_));
				}

				udp_head(const udp_head&lref){
					if (&lref != this){
						sequence_id_ = lref.sequence_id_;
						version_ = lref.version_;
						reserver_first_ = lref.reserver_first_;
						cmd_ = lref.cmd_;
						memcpy_s(reserver_second_, sizeof(reserver_second_), lref.reserver_second_, sizeof(lref.reserver_second_));
						operate_ = lref.operate_;
						sub_operate_ = lref.sub_operate_;
						operate_parament_ = lref.operate_parament_;
					}
				}

				const int length()const
				{
					return sizeof(sequence_id_) + sizeof(version_) + sizeof(reserver_first_) + sizeof(cmd_) + sizeof(reserver_second_) + sizeof(operate_) +
						sizeof(sub_operate_) + sizeof(operate_parament_) ;
				}

				unsigned char *serialize(unsigned char*bytes)const
				{
					if (!bytes){ return nullptr; }
					memcpy_s(bytes, sizeof(sequence_id_), &sequence_id_, sizeof(sequence_id_));
					bytes += sizeof(sequence_id_);
					memcpy_s(bytes, sizeof(version_), &version_, sizeof(version_));
					bytes += sizeof(version_);
					memcpy_s(bytes, sizeof(reserver_first_), &reserver_first_, sizeof(reserver_first_));
					bytes += sizeof(reserver_first_);
					memcpy_s(bytes, sizeof(cmd_), &cmd_, sizeof(cmd_));
					bytes += sizeof(cmd_);
					memcpy_s(bytes, sizeof(reserver_second_), reserver_second_, sizeof(reserver_second_));
					bytes += sizeof(reserver_second_);
					memcpy_s(bytes, sizeof(operate_), &operate_, sizeof(operate_));
					bytes += sizeof(operate_);
					memcpy_s(bytes, sizeof(sub_operate_), &sub_operate_, sizeof(sub_operate_));
					bytes += sizeof(sub_operate_);
					memcpy_s(bytes, sizeof(operate_parament_), &operate_parament_, sizeof(operate_parament_));
					bytes += sizeof(operate_parament_);
					return bytes;
				}

				const unsigned char *build(const unsigned char *bytes, int &cb)
				{
					return nullptr;
				}
			};
			#pragma pack(pop)


			//单字节对齐
			#pragma pack(push, 1)
			//udp ack 协议头
			struct udp_ack_head : public nsp::proto::proto_interface{
				//序列号
				unsigned short int sequence_id_ = 0x0000;
				//版本号
				unsigned char version_ = 0x00;
				//预留
				unsigned char reserver_first_ = 0x00;
				//命令码
				unsigned char cmd_ = 0x00;
				//执行码
				unsigned char ack_ = 0x00;
				//预留
				unsigned short reserver_second_ = 0x0000;
				//主操作码
				unsigned char operate_ = 0x00;
				//副操作码
				unsigned char sub_operate_ = 0x00;
				//操作参数
				unsigned short operate_parament_ = 0x0000;

				udp_ack_head(unsigned short int sequence = 0, unsigned char version = 0x00, unsigned char cmd = 0x00,
					unsigned char op = 0x00, unsigned char sub_op = 0x00, unsigned short operate_parament = 0){
					sequence_id_ = sequence;
					version_ = version;
					cmd_ = cmd;
					operate_ = op;
					sub_operate_ = sub_op;
					operate_parament_ = operate_parament;
				}

				udp_ack_head(const udp_ack_head&lref){
					if (&lref != this){
						sequence_id_ = lref.sequence_id_;
						version_ = lref.version_;
						reserver_first_ = lref.reserver_first_;
						cmd_ = lref.cmd_;
						reserver_second_ = lref.reserver_second_;
						operate_ = lref.operate_;
						sub_operate_ = lref.sub_operate_;
						operate_parament_ = lref.operate_parament_;
					}
				}

				const int length()const
				{
					return sizeof(sequence_id_) + sizeof(version_) + sizeof(reserver_first_) + sizeof(cmd_) + sizeof(ack_) +
						sizeof(reserver_second_) + sizeof(operate_) + sizeof(sub_operate_) + sizeof(operate_parament_);
				}

				unsigned char *serialize(unsigned char*bytes)const
				{
					if (!bytes){ return nullptr; }
					memcpy_s(bytes, sizeof(sequence_id_), &sequence_id_, sizeof(sequence_id_));
					bytes += sizeof(sequence_id_);
					memcpy_s(bytes, sizeof(version_), &version_, sizeof(version_));
					bytes += sizeof(version_);
					memcpy_s(bytes, sizeof(reserver_first_), &reserver_first_, sizeof(reserver_first_));
					bytes += sizeof(reserver_first_);
					memcpy_s(bytes, sizeof(cmd_), &cmd_, sizeof(cmd_));
					bytes += sizeof(cmd_);
					memcpy_s(bytes, sizeof(ack_), &ack_, sizeof(ack_));
					bytes += sizeof(ack_);
					memcpy_s(bytes, sizeof(reserver_second_), &reserver_second_, sizeof(reserver_second_));
					bytes += sizeof(reserver_second_);
					memcpy_s(bytes, sizeof(operate_), &operate_, sizeof(operate_));
					bytes += sizeof(operate_);
					memcpy_s(bytes, sizeof(sub_operate_), &sub_operate_, sizeof(sub_operate_));
					bytes += sizeof(sub_operate_);
					memcpy_s(bytes, sizeof(operate_parament_), &operate_parament_, sizeof(operate_parament_));
					bytes += sizeof(operate_parament_);
					return bytes;
				}

				const unsigned char *build(const unsigned char *byte_stream, int &cb)
				{
					if (cb < length() || !byte_stream) return nullptr;
					memcpy_s(&sequence_id_, sizeof(sequence_id_), byte_stream, sizeof(sequence_id_));
					byte_stream += sizeof(sequence_id_);
					cb -= sizeof(sequence_id_);
					memcpy_s(&version_, sizeof(version_), byte_stream, sizeof(version_));
					byte_stream += sizeof(version_);
					cb -= sizeof(version_);
					memcpy_s(&reserver_first_, sizeof(reserver_first_), byte_stream, sizeof(reserver_first_));
					byte_stream += sizeof(reserver_first_);
					cb -= sizeof(reserver_first_);
					memcpy_s(&cmd_, sizeof(cmd_), byte_stream, sizeof(cmd_));
					byte_stream += sizeof(cmd_);
					cb -= sizeof(cmd_);
					memcpy_s(&ack_, sizeof(ack_), byte_stream, sizeof(ack_));
					byte_stream += sizeof(ack_);
					cb -= sizeof(ack_);
					memcpy_s(&reserver_second_, sizeof(reserver_second_), byte_stream, sizeof(reserver_second_));
					byte_stream += sizeof(reserver_second_);
					cb -= sizeof(reserver_second_);
					memcpy_s(&operate_, sizeof(operate_), byte_stream, sizeof(operate_));
					byte_stream += sizeof(operate_);
					cb -= sizeof(operate_);
					memcpy_s(&sub_operate_, sizeof(sub_operate_), byte_stream, sizeof(sub_operate_));
					byte_stream += sizeof(sub_operate_);
					cb -= sizeof(sub_operate_);
					memcpy_s(&operate_parament_, sizeof(operate_parament_), byte_stream, sizeof(operate_parament_));
					byte_stream += sizeof(operate_parament_);
					cb -= sizeof(operate_parament_);
					return byte_stream;
				}
			};
			#pragma pack(pop)

		}
	}
}

#endif