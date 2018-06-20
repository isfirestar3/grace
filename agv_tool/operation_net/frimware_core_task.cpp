#include "frimware_core_task.h"
#include "gzfts_api.h"
#include "log.h"

#define push_window_size 1

frimware_core_task::frimware_core_task() {
	initlization();
}

frimware_core_task::~frimware_core_task() {
	uinit();
}

void frimware_core_task::initlization() {
	if (!task_thread_) {
		try {
			task_thread_ = new std::thread(std::bind(&frimware_core_task::task_run, this));
		}
		catch (...)
		{
			loerror("operation_net") << "failed to create send frimware core file task thread.";
		}
	}
}

void frimware_core_task::uinit() {
	is_exists_ = true;
	timeout_wait_.sig();
	if (task_thread_)
	{
		if (task_thread_->joinable()) {
			task_thread_->join();
		}
		delete task_thread_;
		task_thread_ = nullptr;
	}
	timeout_wait_.reset();
}

void frimware_core_task::add_push_core_task(const std::string& ip, const uint16_t port, const std::string & local_file, const std::string& remote_file)
{
	frimware_task t_info;
	t_info.ip_ = ip;
	t_info.port_ = port;
	t_info.local_file_ = local_file;
	t_info.remote_file_ = remote_file;
	{
		std::lock_guard<decltype(r_mutex_)> lock(r_mutex_);
		core_deque_.push_back(t_info);
	}
	timeout_wait_.sig();
}

void frimware_core_task::reduce_push_count()
{
	loinfo("operation_net") << "current push count is " << current_push_number_ << " ,then reduce one.";
	if (current_push_number_ > 0) {
		current_push_number_--;
		push_waiter_.sig();
	}
	else if (current_push_number_ < 0){
		current_push_number_ = 0;
		push_waiter_.sig();
	}
}

void frimware_core_task::task_run() {
	while (true) {
		if (WAIT_OBJECT_0 == timeout_wait_.wait(-1));
		if (is_exists_) {
			lowarn("operation_net") << "the normal thread of push file task class existed.";
			return;
		}

		while (true)
		{
			frimware_task f_task;
			{
				std::lock_guard<decltype(r_mutex_)> lock(r_mutex_);
				if (core_deque_.size() == 0) break;
				f_task = core_deque_.front();
				core_deque_.pop_front();
			}

			loinfo("operation_net") << "current push number value is " << current_push_number_;
			//查看当前发送窗口是否大于1，如果大于1，则无限等待，直至获取到发送窗口为止
			if (current_push_number_ >= push_window_size) {
				//无限等待下次发送机会
				if (WAIT_OBJECT_0 == push_waiter_.wait(-1));
			}
			current_push_number_++;

			loinfo("operation_net") << "start push file: " << f_task.local_file_ << " to target: " << f_task.ip_;
			if (fts_push(f_task.ip_.c_str(), f_task.port_, f_task.local_file_.c_str(), f_task.remote_file_.c_str(), 1) < 0) {
				current_push_number_--;
				loerror("operation_net") << "failed to push file to agv_shell server,the target server is  " << f_task.ip_ << " ,the file is "<< f_task.local_file_;
			}
		}
	}

}