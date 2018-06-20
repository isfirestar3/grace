#pragma  once
#include "functional"
#include "os_util.hpp"
#include "memory"

namespace agv_motion
{
	class motion_data
	{
	public:
		motion_data(int err,unsigned char * buffer,int length ) {
			packet_.assign((char *)buffer,length);
			error_ = err;
		};

		motion_data(){};

		~motion_data(){};

	public:
		std::string packet_;
		//unsigned char * block_ = nullptr;
		int error_ = -1;
	};

	class motion_ack
	{
	public:
		motion_ack(){
			refresh();
		};

		motion_ack(const std::function<void(const void*)>&callback){
			callback_function_ = callback;
			refresh();
		}

		~motion_ack() {};
		void refresh() { tick_ = nsp::os::gettick(); };

		void exec(void * data)
		{
			if (callback_function_){
				callback_function_(data);
			}
		}

		uint64_t gettick(){ return tick_; };

	public:
		std::function<void(const void*)> callback_function_;
		uint64_t tick_;
	};
}
