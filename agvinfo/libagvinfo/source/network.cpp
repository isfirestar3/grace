#include "network.h"
#include "log.h"

network::network()
{
	if (!thread_)
	{
		try{
			thread_ = new std::thread(std::bind(&network::check_time,this));
		}
		catch (...){
			nsperror << "create thread error";
		}
	}
}

network::~network()
{
	thread_exit_ = true;
	condition_.notify_one();
	timeout_waiter_.sig();
	if (thread_){
		if (thread_->joinable()){
			thread_->join();
		}
		delete thread_;
		thread_ = nullptr;
	}
}

int network::write( uint32_t pid, std::shared_ptr<agv_motion::motion_ack> &motion,const std::function<int()> &sender )
{
	if (!sender){
		return -1;
	}
	
	if (push(pid, motion)){
		return -2;
	}

	if (sender()){
		remove(pid);
		return -3;
	}

	condition_.notify_one();

	return 0;
}

int network::push(uint32_t pid, std::shared_ptr<agv_motion::motion_ack> & motion )
{
	{
		std::lock_guard<decltype(mutex_)> lock(mutex_);
		if (map_callback_.find(pid) != map_callback_.end()){
			return -1;
		}

		map_callback_[pid] = motion;
	}

	return 0;
}

void network::remove(uint32_t pid)
{
	auto iter = map_callback_.end();
	std::lock_guard<decltype(mutex_)> lock(mutex_);
	iter = map_callback_.find(pid);
	if (iter != map_callback_.end()){
		map_callback_.erase(iter);
	}
}

void network::exec(uint32_t pid,void *data,int length )
{
	auto iter = map_callback_.end();
	std::shared_ptr<agv_motion::motion_ack> motion = nullptr;
	{
		std::lock_guard<decltype(mutex_)> lock(mutex_);
		if ((iter = map_callback_.find(pid)) != map_callback_.end()){
			motion = std::move(iter->second);
		}
	}
	remove(pid);
	if (motion){
		agv_motion::motion_data motion_block(0, (unsigned char *)data, length);
		motion->exec(&motion_block);
	}
}

int network::check_time()
{
	static uint32_t time_wait = 1000;
	static uint32_t time_out = 5000;
	while (timeout_waiter_.wait(time_wait) > 0)
	{
		if (map_callback_.empty()){
			std::unique_lock<decltype(mutex_)> lock(mutex_);
			condition_.wait(lock);
		}

		if (thread_exit_){
			break;
		}

		{
			uint64_t current_tick = nsp::os::gettick();
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			for (auto iter = map_callback_.begin(); iter != map_callback_.end();){
				std::shared_ptr<agv_motion::motion_ack> motion = iter->second;

				if (motion){
					uint64_t tick = motion->tick_;
					if (current_tick > tick){
						if ((current_tick - tick) > time_out){
							notify_timeout(motion);
							iter = map_callback_.erase(iter);
							continue;
						}
					}
				}
				iter++;
			}
		}
	}
	return 0;
}

void network::notify_timeout(std::shared_ptr<agv_motion::motion_ack> &motion)
{
	if (motion){
		agv_motion::motion_data motion_block;
		motion->exec((void *)&motion_block);
	}
}

void network::notify()
{
	if (notify_callback_){
		notify_callback_();
	}
}
