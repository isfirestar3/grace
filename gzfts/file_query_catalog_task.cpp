#include "file_query_catalog_task.h"
#include "file_read_handler.h"
#include "receiver_manager.h"

file_query_catalog_task::file_query_catalog_task()
{
	try
	{
		if (!run_task_)
		{
			run_task_ = new std::thread(std::bind(&file_query_catalog_task::query_task, this));
		}
	}
	catch(...){

	}
}

file_query_catalog_task::~file_query_catalog_task()
{
	exit_task_thread_ = 1;
	task_wait_.sig();

	if (run_task_)
	{
		if (run_task_->joinable())
		{
			run_task_->join();
		}
		delete run_task_;
		run_task_ = nullptr;
	}

	task_wait_.reset();
}

void file_query_catalog_task::add_query_task(const uint32_t link, const uint32_t pkt_id, const std::string& f_path)
{
	catalog_task_info r_info;
	r_info.link = link;
	r_info.pkt_id = pkt_id;
	r_info.file_path = f_path;
	{
		std::lock_guard<decltype(task_mutex_)> lock(task_mutex_);
		deque_task_.push_back(r_info);
	}
	task_wait_.sig();
}

void file_query_catalog_task::query_task()
{
	file_read_headler read_handler;
	while (true){
		if (0 == task_wait_.wait(-1));
		if (exit_task_thread_ == 1) break;

		catalog_task_info r_info;
		while (true)
		{
			{
				std::lock_guard<decltype(task_mutex_)> lock(task_mutex_);
				if (deque_task_.size() == 0) break;
				r_info = deque_task_.front();
				deque_task_.pop_front();
			}
			std::vector<nsp::file::file_info> vct_file_info;
			int result = read_handler.get_fts_current_catalog(r_info.file_path, vct_file_info);
			if (result < 0){
				loerror("fts") << "get an error while get current catalog information:" << r_info.file_path;
			}
			nsp::toolkit::singleton<receiver_manager>::instance()->post_catalog_reponse(r_info.link, r_info.pkt_id, result, vct_file_info);
		}
	}
}