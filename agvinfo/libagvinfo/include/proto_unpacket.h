#pragma  once

#include "proto_definion.h"
namespace nsp
{
	namespace proto
	{
		class unpackt{
		private:
			const unsigned char* buffer_ = nullptr;
			const unsigned char* pos_ = nullptr;
			nsp::proto::proto_head head_;
			int cb_;

			int build_head(){
				if (cb_ < head_.length()){
					return -1;
				}
				pos_ = head_.build(pos_, cb_);
				if (!pos_){
					return -1;
				}
				return 0;
			}

		public:
			unpackt(const std::string& pkt){
				buffer_ = (unsigned char*)pkt.c_str();
				pos_ = buffer_;
				cb_ = pkt.length();
				build_head();
			}
			~unpackt(){};

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

			uint16_t ack_type(){ return head_.type_; };

			uint32_t pkt_id(){ return head_.id_; };
		};
	}
}