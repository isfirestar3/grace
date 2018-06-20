#include "fts_pull_task_group.h"
#include "log.h"
#include <io.h>
#include "rw_xml_file.h"
#include "os_util.hpp"

fts_pull_task_group::fts_pull_task_group() {
	initlization();
}

fts_pull_task_group::~fts_pull_task_group() {
	uint();
}

void fts_pull_task_group::initlization() {
	//fts_parameter tmp;
	//memset(tmp.file_search_root, 0, sizeof(tmp.file_search_root));
	//tmp.timeout_cancel_transfer = 10000;
	//fts_change_configure(&tmp);

	////读取xml文件配置信息
	//std::string local_xml_file = nsp::os::get_module_directory<char>() + "/AGVSetting.xml";
	//std::string  port;
	//if (_access(local_xml_file.c_str(), 0) != -1) {
	//	rw_xml_file rw;
	//	rw.read_xml_element(local_xml_file, "loacl_fts", "ipv4", fts_ipv4_);
	//	rw.read_xml_element(local_xml_file, "loacl_fts", "port", port);
	//}
	//fts_port_ = atoi(port.c_str());
	//if (fts_create_receiver(fts_ipv4_.c_str(), fts_port_) < 0) {
	//	loerror("operation_net") << "failed to create fts server to receive frimware bin file.";
	//}
	if (task_thread_) {
		return;
	}
	try {
		task_thread_ = new std::thread(std::bind(&fts_pull_task_group::task_run, this));
	}
	catch (...) {
		loerror("operation_net") << "failed to create push task thread";
		return;
	}
}

void fts_pull_task_group::uint() {
	//fts_destory_receiver(fts_ipv4_.c_str(), fts_port_);
	exist_thread_ = true;
	timeout_wait_.sig();

	if (task_thread_) {
		if (task_thread_->joinable()) {
			task_thread_->join();
		}
		delete task_thread_;
		task_thread_ = nullptr;
	}
	timeout_wait_.reset();
	pull_waiter_.reset();
}

void fts_pull_task_group::add_pull_task(const char* target_ip, const uint16_t port, const std::string& remote_file, const std::string& local_file)
{
	pull_task_item f_task;
	f_task.target_ipv4_ = target_ip;
	f_task.target_port_ = port;
	f_task.remote_path_ = remote_file;
	f_task.local_path_ = local_file;
	{
		std::lock_guard<decltype(r_mutex_)> lock(r_mutex_);
		pull_deque_.push_back(f_task);
	}
	timeout_wait_.sig();
}

void fts_pull_task_group::task_run() {
	while (true) {
		if (WAIT_OBJECT_0 == timeout_wait_.wait(-1));
		if (exist_thread_)return;

		while (true)
		{
			pull_task_item f_task;
			{
				std::lock_guard<decltype(r_mutex_)> lock(r_mutex_);
				if (pull_deque_.size() == 0) break;
				f_task = pull_deque_.front();
				pull_deque_.pop_front();
			}

			if (fts_pull(f_task.target_ipv4_.c_str(), f_task.target_port_, f_task.local_path_.c_str(), f_task.remote_path_.c_str(), 0) < 0) {
				loerror("operation_net") << "failed to pull m core file from agv_shell server,the target server is  " << f_task.target_ipv4_ << " ,the core file is " << f_task.remote_path_;
			}
			//等待50毫秒
			pull_waiter_.wait(50);
		}
	}
}