#include "fts_push_task_group.h"
#include "gzfts_api.h"
#include <random>
#include "fts_handler.h"

fts_push_task_group::fts_push_task_group():timeout_waiter_(1), push_waiter_(1){
	initlization();
}

fts_push_task_group::~fts_push_task_group(){
	exist_thread_ = true;
	timeout_waiter_.sig();

	if (task_thread_){
		if (task_thread_->joinable()){
			task_thread_->join();
		}
		delete task_thread_;
		task_thread_ = nullptr;
	}
	timeout_waiter_.reset();
	push_waiter_.reset();
}

void fts_push_task_group::initlization(){
	if (task_thread_){
		return;
	}
	try{
		task_thread_ = new std::thread(std::bind(&fts_push_task_group::task_run, this));
	}
	catch (...){
		loerror("operation_net") << "failed to create push task thread";
		return;
	}
}

//添加任务组
uint64_t fts_push_task_group::add_push_task(const char* target_ip, const uint16_t port, const std::vector<push_file_info>& vct_file, const uint64_t task_group){
	task_item_info t_info;
	if (task_group == 0) {
		t_info.task_id = get_random_id();
	}
	else {
		t_info.task_id = task_group;
	}
	t_info.ip = target_ip;
	t_info.port = port;
	t_info.vct_file = vct_file;

	{
		std::lock_guard<decltype(r_mutex_)> lock(r_mutex_);
		deque_task_.push_back(t_info);
		timeout_waiter_.sig();
	}

	return t_info.task_id;
}

//删除失败的任务
void fts_push_task_group::remove_push_task(const uint64_t task_id) {
	exist_curr_task_ = true;
	std::lock_guard<decltype(r_mutex_)> lock(r_mutex_);
	std::deque<task_item_info>::iterator iter = deque_task_.begin();
	while (iter != deque_task_.end()) {
		if (iter->task_id == task_id) {
			iter->vct_file.clear();
			iter = deque_task_.erase(iter);
			continue;
		}
		iter++;
	}
	//置为所有的push计数为0
	if (current_push_number_ > 0) {
		current_push_number_ = 0;
		push_waiter_.sig();
	}
}

//减少一个正在push操作队列
void fts_push_task_group::reduce_push_number(){
	if (current_push_number_ > 0){
		current_push_number_--;
		push_waiter_.sig();
	}
}

uint64_t fts_push_task_group::get_random_id(){
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	uint64_t result = (timeinfo->tm_year + 1900) * pow(10 , 15) +(timeinfo->tm_mon + 1) * pow(10 , 13) + timeinfo->tm_mday * pow(10 , 11) +
		timeinfo->tm_hour * pow(10 , 9) + timeinfo->tm_min * pow(10 , 7) + timeinfo->tm_sec * pow(10 , 5); 
	uint32_t post_fix = allocate_random() & 0xFFFFF;//只取0~0xfffff之间的数
	result += post_fix;
	return result;
}

uint32_t fts_push_task_group::allocate_random(){
	std::random_device rd;
	return rd();
}

void fts_push_task_group::task_run(){

	while (true){
		if (WAIT_OBJECT_0 == timeout_waiter_.wait(-1));
		if (exist_thread_)return;	

		while(true)
		{
			task_item_info item_info;
			{
				exist_curr_task_ = false;
				std::lock_guard<decltype(r_mutex_)> lock(r_mutex_);
				if (deque_task_.size() == 0)break;
				item_info = deque_task_.front();
			}

			for (auto& iter = item_info.vct_file.begin(); iter != item_info.vct_file.end();)
			{
				//进入下一次循环,当前最允许最多有window size个file在同时push
				if (current_push_number_ >= total_window_size){
					//无限等待下次发送机会
					if (WAIT_OBJECT_0 == push_waiter_.wait(-1)) {
						if (exist_curr_task_) break;
					}
				}
				//当前有一个文件正在push操作
				current_push_number_++;
				loinfo("operation_net") << "push remote file:" << iter->remote_file;
				
				if (fts_push(item_info.ip.c_str(), item_info.port, iter->local_file.c_str(), iter->remote_file.c_str(), 1) < 0){
					loerror("operation_net") << "failed to push file to remote,the file is " << iter->local_file;
					current_push_number_--;
				}
				//任务组队列中中删除这个文件
				iter = item_info.vct_file.erase(iter);
			}

			//任务组队列删除这个任务
			if (item_info.vct_file.size() == 0)
			{
				std::lock_guard<decltype(r_mutex_)> lock(r_mutex_);
				deque_task_.pop_front();
			}

		}
	}
}