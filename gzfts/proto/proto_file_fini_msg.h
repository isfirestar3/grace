


#pragma once 

#include "serialize.hpp"
#include  "proto_definion.h"



namespace nsp{
	namespace proto{
		typedef struct proto_file_fini_msg:public nsp::proto::proto_interface
		{
			proto_file_fini_msg(int type) :head_(type) {}
			proto_file_fini_msg() {}
			nsp::proto::proto_head head_;
			const int length()const
			{
				return sizeof(head_);
			}

			unsigned char * serialize(unsigned char * bytes)const 
			{
				unsigned char *pos = head_.serialize(bytes);
				return pos;
			}

			const unsigned char *build(const unsigned char *bytes, int &cb)
			{
				const unsigned char *pos = head_.build(bytes, cb);
				return pos;
			}

		}proto_file_fini_msg_t;
	}
}
