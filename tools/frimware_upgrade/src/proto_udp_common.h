#ifndef PROTO_UDP_COMMON_H_20170717
#define PROTO_UDP_COMMON_H_20170717

#include "proto_head_definition.h"

namespace nsp{
	namespace proto{
		namespace udp{
			//单字节对齐
		#pragma pack(push, 1)
			typedef struct __common_data : public nsp::proto::proto_interface{
				nsp::proto::udp::udp_head head_;
				short int data_len_ = 0;//双字节存储数据长度
				std::string data_;//数据块

				__common_data(unsigned char op, unsigned short int sequence_id) :head_(sequence_id, 0x00, 0x06, op, 0x00, 0){}
				__common_data(unsigned char op, unsigned char sub_op, unsigned short int sequence_id) :head_(sequence_id, 0x00, 0x06, op, sub_op, 0){}
				__common_data(){}

				const int length()const
				{
					return head_.length() + sizeof(data_len_) + data_len_*sizeof(unsigned char);
				}

				unsigned char *serialize(unsigned char*bytes)const
				{
					if (!bytes)return nullptr;
					bytes = head_.serialize(bytes);
					memcpy_s(bytes, sizeof(data_len_), &data_len_, sizeof(data_len_));
					bytes += sizeof(data_len_);
					for (size_t i = 0; i < data_.size(); i++){
						memcpy_s(bytes, sizeof(unsigned char), &data_.at(i), sizeof(unsigned char));
						bytes += sizeof(unsigned char);
					}
					return bytes;
				}

				const unsigned char *build(const unsigned char *bytes, int &cb)
				{
					if (!bytes || cb < length())return nullptr;
					const unsigned char *pos = bytes;
					//pos = head_.build(pos, cb);
					memcpy_s(&data_len_, sizeof(data_len_), pos, sizeof(data_len_));
					pos += sizeof(data_len_);
					cb -= sizeof(data_len_);
					data_.assign((char*)pos, cb);
					return pos;
				}

				void set_sub_operate(uint8_t value){
					head_.sub_operate_ = value;
				}

			}common_data;
		#pragma pack(pop)

#pragma pack(push, 1)
			typedef struct __can_data : public nsp::proto::proto_interface{
				uint8_t can_cmd;
				uint8_t	can_serial_index_0;
				uint8_t can_serial_index_1;
				uint8_t can_node_id;
				uint32_t data_length;
				std::string data_;//数据块

				__can_data(uint8_t cmd, uint16_t serial_index, uint8_t node_id )
				{
					can_cmd = cmd;
					can_serial_index_1 = serial_index & 0x00ff;
					can_serial_index_0 = serial_index >>8 & 0x00ff;
					can_node_id = node_id;
					data_length = 0;
				}

				__can_data()
				{
					can_cmd = 0;
					can_serial_index_0 = 0;
					can_serial_index_1 = 0;
					can_node_id = 0;
					data_length = 0;
				}

				const int length() const
				{
					return sizeof(can_cmd)+sizeof(can_serial_index_0)+sizeof(can_serial_index_1)+sizeof(can_node_id)+sizeof(data_length)+data_length*sizeof(uint8_t);
				}

				unsigned char *serialize(unsigned char*bytes)const
				{
					*bytes++ = can_cmd;
					*bytes++ = can_serial_index_0;
					*bytes++ = can_serial_index_1;
					*bytes++ = can_node_id;
					memcpy_s(bytes, sizeof(uint32_t), &data_length, sizeof(uint32_t));
					bytes += sizeof(uint32_t);
					for (size_t i = 0; i < data_.size(); i++){
						memcpy_s(bytes, sizeof(unsigned char), &data_.at(i), sizeof(unsigned char));
						bytes += sizeof(unsigned char);
					}
					return bytes;
				}

				const unsigned char *build(const unsigned char *bytes, int &cb)
				{
					if (!bytes || cb < length())return nullptr;

					can_cmd = *bytes++;
					can_serial_index_0 = *bytes++;
					can_serial_index_1 = *bytes++;
					can_node_id = *bytes++;
					memcpy_s(&data_length,sizeof(uint32_t),bytes,sizeof(uint32_t));
					bytes += sizeof(uint32_t);
					data_.assign((char*)bytes, cb);
					return bytes;
				}

			}can_data_t;
#pragma pack(pop)

			class unpackage{
			private:
				const unsigned char* buffer_ = nullptr;
				const unsigned char* pos_ = nullptr;
				nsp::proto::udp::udp_ack_head pkt_head_;
				int cb_;

				int build_head(){
					if (cb_ < pkt_head_.length()){
						return -1;
					}
					pos_ = pkt_head_.build(pos_, cb_);
					if (!pos_){
						return -1;
					}
					return 0;
				}

			public:
				unpackage(const std::string& pkt){
					buffer_ = (unsigned char*)pkt.c_str();
					pos_ = buffer_;
					cb_ = pkt.length();
					build_head();
				}
				~unpackage(){};

				template<class T>
				std::shared_ptr<T> shared_for() {
					try {
						std::shared_ptr<T> s_ptr = std::make_shared<T>();
						if (s_ptr->build(pos_, cb_) < 0) {
							return std::shared_ptr<T>(nullptr);
						}
						return s_ptr;
					}
					catch (std::bad_alloc &) {
						return std::shared_ptr<T>(nullptr);
					}
				}

				uint8_t ack_type(){
					return pkt_head_.ack_;
				}

				uint16_t pkt_id(){
					return pkt_head_.sequence_id_;
				}

				uint8_t main_operate(){
					return pkt_head_.operate_;
				}

				uint8_t sub_operate(){
					return pkt_head_.sub_operate_;
				}
			};


#pragma pack(push, 1)
			typedef struct __recv_data{
				short int data_len_ = 0;//双字节存储数据长度
				std::string data_;//数据块

				const int length()const
				{
					return  sizeof(data_len_) + data_len_*sizeof(data_);
				}

				unsigned char *serialize(unsigned char*bytes)const
				{
					if (!bytes)return nullptr;
					memcpy_s(bytes, sizeof(data_len_), &data_len_, sizeof(data_len_));
					bytes += sizeof(data_len_);
					for (size_t i = 0; i < data_len_; i++){
						if (data_.at(i)){
							memcpy_s(bytes, sizeof(unsigned char), &data_.at(i), 1);
							bytes += sizeof(unsigned char);
						}
					}
					return bytes;
				}

				const unsigned char *build(const unsigned char *bytes, int &cb)
				{
					if (!bytes || cb < length())return nullptr;
					memcpy_s(&data_len_, sizeof(data_len_), bytes, sizeof(data_len_));
					bytes += sizeof(data_len_);
					cb -= sizeof(data_len_);
					data_.assign((char*)bytes, cb);
					bytes += sizeof(char)*data_.size();
					cb -= data_.size();
					return bytes;
				}

			}recv_data;
#pragma pack(pop)

		}
	}
}

#endif