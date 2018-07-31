#include "agv_driver.h"
#include "TrafficManage.h"
#include <sstream>
#include "os_util.hpp"
#include "log.h"
#include "math.h"
#include <functional>

AgvDriver::AgvDriver(int agv_id)
	: agv_id_(agv_id), agv_max_segment_distance_(5), agv_release_distance_(0.8),
	is_register_(false), upl_valid_(false), wop_index_(0), need_to_released_segment_count_(0), first_task_(true), have_task_(false)
{
	not_release_segment_error_count_ = 0;
	last_current_upl_index_in_not_relaase_segment = -1;
	blocked_ = false;
	blocked_first_print_ = false;
	traffic_manage_ = NULL;
}

AgvDriver::~AgvDriver()
{
	loinfo("agvdriver") << "AgvDriver" << agv_id_ << " destroyed.";
}

void AgvDriver::SetMaxSegmentDistance(double max_segment_distance)
{
	agv_max_segment_distance_ = max_segment_distance;
}

void AgvDriver::SetReleaseDistance(double release_distance)
{
	agv_release_distance_ = release_distance;
}

void AgvDriver::SetTrafficManage(TrafficManage* traffic_manage)
{
	traffic_manage_ = traffic_manage;
}

void AgvDriver::DoAllocatedAnddRelease()
{
	if (false == have_task_) {
		return;
	}
	this->AllocateDealCycle();
	this->ReleaseDealCycle();
}

bool AgvDriver::RegisterUPL(upl_t current_upl)
{
	std::lock_guard<decltype(agv_driver_mutex_)> guard(agv_driver_mutex_);

	if (true == is_register_) {
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Register Upl, Segment " << current_upl.edge_id_ << ", the Agv is already registered...";
		return true;
	}

	std::vector<int> m_wop_list;

	PathSearch_LayoutData *layout_data = traffic_manage_->GetLayoutData();
	for (unsigned int i = 0; i < layout_data->edgeList.size(); ++i) {
		PathSearch_EDGEINFO m_edge = layout_data->edgeList[i];
		if (m_edge.id == current_upl.edge_id_) {
			m_wop_list = m_edge.wop_list;
			break;
		}
	}

	if (m_wop_list.size() == 0) {
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Register Upl, Segment " << current_upl.edge_id_ << " is not exist or wop list is empty...";
		return false;
	}

	bool register_ok = true;
	int m_segment_id = current_upl.edge_id_;
	std::vector<int> allocated_segments;
	for (unsigned int i = 0; i < m_wop_list.size(); ++i) {
		int m_wop_id = m_wop_list[i];
		if (false == traffic_manage_->AllocateSegment(agv_id_, m_segment_id, m_wop_id, true)) {
			loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Register Upl, Segment " << m_segment_id << " wop " << m_wop_id << " is allocated failed...";
			register_ok = false;
			break;
		}
		else {
			allocated_segments.push_back(m_wop_id);
			loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Register Upl, Segment " << m_segment_id << " wop " << m_wop_id << " is allocated success...";
			register_ok = true;
		}
	}

	if (true == register_ok) {
		for (unsigned int i = 0; i < allocated_segments.size(); ++i) {
			trail_t m_task_path;
			m_task_path.edge_id_ = m_segment_id;
			m_task_path.wop_id_ = allocated_segments[i];
			register_allocated_task_path_.push_back(m_task_path);
		}

		is_register_ = true;

		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Register Upl Success, Segment " << m_segment_id;

		return true;
	}
	else {
		for (unsigned int i = 0; i < allocated_segments.size(); ++i) {
			traffic_manage_->ReleaseSegment(agv_id_, m_segment_id, allocated_segments[i]);
			
			loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Register Upl failed, Release Segment " << m_segment_id << " wop " << allocated_segments[i];
		}

		traffic_manage_->ClearNotAllocateSegment(agv_id_);

		is_register_ = false;

		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Register Upl failed, Segment " << m_segment_id;

		return false;
	}
	return false;
}

void AgvDriver::UnRegister()
{
	std::lock_guard<decltype(agv_driver_mutex_)> guard(agv_driver_mutex_);

	if (false == is_register_) {
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " UnRegister Upl, the Agv is not registered...";
		return;
	}

	traffic_manage_->ReleaseSegmentAndNode(agv_id_);
	loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " UnRegister Upl, Release all segments and nodes...";

	traffic_manage_->ClearNotAllocateSegment(agv_id_);
	loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " UnRegister Upl, Let traffic manage not allocate segment...";

	is_register_ = false;
	register_allocated_task_path_.clear();

	have_task_ = false;
	complete_task_path_.clear();
	need_allocate_task_path_.clear();
	allocated_but_not_download_task_path_.clear();
	download_and_allocated_task_path_.clear();
	released_task_path_.clear();

	need_to_released_segment_count_ = 0;
	not_release_segment_error_count_ = 0;
	last_current_upl_index_in_not_relaase_segment = -1;

	blocked_ = false;
	loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " UnRegister Upl success";
}

void AgvDriver::UpdateUPL(upl_t current_upl, unsigned int wop_index)
{
	{
		std::lock_guard<decltype(agv_upl_mutex_)> guard(agv_upl_mutex_);
		current_upl_ = current_upl;

		if (false == upl_valid_) {
			wop_index_ = wop_index;
			upl_valid_ = true;
		}
	}

	std::lock_guard<decltype(agv_driver_mutex_)> guard(agv_driver_mutex_);

	if (false == have_task_) {
		return;
	}

	if (complete_task_path_.size() > wop_index) {
		if (current_upl.edge_id_ == complete_task_path_[wop_index].edge_id_) {
			if (wop_index > wop_index_) {
				need_to_released_segment_count_ += (wop_index - wop_index_);
				loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " is already get over " << (wop_index - wop_index_) << ((wop_index - wop_index_) ? " segment" : " segments")
					<< ", wop_index:" << wop_index << " current wop_index:" << wop_index_
					<< ", need to release " << need_to_released_segment_count_ << " segments"
					<< ", current segment " << current_upl_.edge_id_ << " angle " << (current_upl_.angle_ * 180.0 / 3.1415926) << " percent " << current_upl_.percentage_ << " index " << wop_index;
			}
			else if (wop_index < wop_index_) {
				wop_index_ = 0;
				return;
			}
			wop_index_ = wop_index;
		}
		else {
			loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " udpate upl error: " << current_upl.edge_id_ << " != " << complete_task_path_[wop_index].edge_id_ << ", wop_index:" << wop_index;
			//wop_index_ = 0;
		}
	}
}

bool AgvDriver::StartTaskPath(std::vector<trail_t> complete_task_path)
{
	std::lock_guard<decltype(agv_driver_mutex_)> guard(agv_driver_mutex_);

	if (complete_task_path.size() == 0) {
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Start task path failed...";
		return false;
	}

	if (false == is_register_ || true == have_task_) {
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Start task path failed, is register:" << is_register_ << ", already have task:" << have_task_;
		return false;
	}

	if (complete_task_path_.size() > 0) {
		if (complete_task_path_[complete_task_path_.size() - 1].edge_id_ == complete_task_path[0].edge_id_) {
			if (complete_task_path_[complete_task_path_.size() - 1].wop_id_ != complete_task_path[0].wop_id_) {
				//need_to_released_segment_count_ += 1;
			}
		}
		else {
			loerror("agvdriver") << "AgvDriver Agv " << agv_id_ << " Start task path, Path not connect...";
		}
	}

	complete_task_path_ = complete_task_path;
	direction_chanage_node_list_ = traffic_manage_->CheckDirectionChangeNode(complete_task_path);
	for (std::vector<int>::iterator iter_c = direction_chanage_node_list_.begin(); iter_c != direction_chanage_node_list_.end(); iter_c++)
	{
		loinfo("agvdriver") << "AgvDriver Agv" << agv_id_ << " =============>direction_chanage_node_list_:" << *iter_c;
	}
	
	need_allocate_task_path_ = complete_task_path;
	allocated_but_not_download_task_path_.clear();

	wop_index_ = 0;

	if (register_allocated_task_path_.size() > 0) {
		trail_t current_task_path = complete_task_path[0];
		if (false == traffic_manage_->ReleaseSegmentAndNodeButNotCurrentUpl(agv_id_, current_task_path.edge_id_, current_task_path.wop_id_, complete_task_path_, direction_chanage_node_list_, first_task_)) {
			loerror("agvdriver") << "StartTaskPath ReleaseSegmentAndNodeButNotCurrentUpl error...";
		}
		first_task_ = false;
		register_allocated_task_path_.clear();

		lotrace("agvdriver") << "AgvDriver Agv " << agv_id_ << " Start task path, Only allocated segment " << current_task_path.edge_id_ << " wop " << current_task_path.wop_id_;

		download_and_allocated_task_path_.clear();
		need_to_released_segment_count_ = 0;
	}

	have_task_ = true;

	std::stringstream ss;
	for (unsigned int i = 0; i < complete_task_path.size(); ++i) {
		trail_t m_task_path = complete_task_path[i];
		ss << " " << m_task_path.edge_id_ << " " << m_task_path.wop_id_ << ",";
	}

	std::stringstream ssl;
	for (unsigned int i = 0; i < direction_chanage_node_list_.size(); ++i) {
		ssl << " " << direction_chanage_node_list_[i] << ",";
	}

	lotrace("agvdriver") << "AgvDriver Agv " << agv_id_ << " Start task path success, Path list:" << ss.str();
	lotrace("agvdriver") << "AgvDriver Agv " << agv_id_ << " Start task path success, Direction Change Node List:" << ssl.str();

	return true;
}

bool AgvDriver::UpdateTaskPath(std::vector<trail_t> complete_task_path)
{
	std::lock_guard<decltype(agv_driver_mutex_)> guard(agv_driver_mutex_);
	if (false == is_register_ || false == have_task_) {
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Update task path failed, is register:" << is_register_ << ", already have task:" << have_task_;
		return false;
	}

	//if (complete_task_path_.size() > complete_task_path.size()) {
	//	loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Update task path failed, complete_task_path_.size()=" << complete_task_path_.size() << " , complete_task_path.size()" << complete_task_path.size();
	//	return false;
	//}

	//unsigned int m_index = -1;
	//for (unsigned int i = 0; i < complete_task_path_.size(); ++i) {
	//	trail_t m_current = complete_task_path_[i];
	//	trail_t m_update = complete_task_path[i];
	//	if (m_current.edge_id_ == m_update.edge_id_ && m_current.wop_id_ == m_update.wop_id_) {
	//		continue;
	//	}
	//	m_index = i;
	//	break;
	//}

	unsigned int m_index = -1;
	for (unsigned int i = 0; i < complete_task_path_.size(); ++i) {
		trail_t m_current = complete_task_path_[i];
		if (m_current.edge_id_ == current_upl_.edge_id_ && i == wop_index_) {

			m_index = i;
			break;
		}
	}

	if (-1 == m_index) {
		lotrace("agvdriver") << "AgvDriver Agv " << agv_id_ << " Update task path failed, current Upl(" << current_upl_.edge_id_ << "," << wop_index_ << ")";
		return false;
	}

	std::stringstream ss;
	for (unsigned int i = 0; i < complete_task_path.size(); ++i) {
		trail_t m_task_path = complete_task_path[i];
		ss << " " << m_task_path.edge_id_ << " " << m_task_path.wop_id_ << ",";
	}
	lotrace("agvdriver") << "AgvDriver Agv " << agv_id_ << " Update task path success, Path list:" << ss.str();

	complete_task_path_ = complete_task_path;

	need_allocate_task_path_.clear();
	for (unsigned int i = m_index+1; i < complete_task_path.size(); ++i) {
		need_allocate_task_path_.push_back(complete_task_path[i]);
	}

	std::stringstream ssl;
	for (unsigned int i = 0; i < need_allocate_task_path_.size(); ++i) {
		trail_t m_task_path = need_allocate_task_path_[i];
		ssl << " " << m_task_path.edge_id_ << " " << m_task_path.wop_id_ << ",";
	}
	lotrace("agvdriver") << "AgvDriver Agv " << agv_id_ << " Update task path success, Need Allocate Path List:" << ssl.str();

	traffic_manage_->ClearNotAllocateSegment(agv_id_);
	return true;
}

void AgvDriver::FinishTaskPath()
{
	std::lock_guard<decltype(agv_driver_mutex_)> guard(agv_driver_mutex_);
	if (false == is_register_ || false == have_task_) {
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Finish task path failed, is register:" << is_register_ << ", already have task:" << have_task_;
		return;
	}

	{
		std::lock_guard<decltype(agv_upl_mutex_)> guard(agv_upl_mutex_);
		need_to_released_segment_count_ -= released_task_path_.size();

		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Finish task path, released " << released_task_path_.size() << " segments, Need to Release " << need_to_released_segment_count_ << " segments";
	}
	released_task_path_.clear();

	if (download_and_allocated_task_path_.size() > 0) {
		trail_t current_task_path = download_and_allocated_task_path_[download_and_allocated_task_path_.size()-1];
		if (false == traffic_manage_->ReleaseSegmentAndNodeButNotCurrentUpl(agv_id_, current_task_path.edge_id_, current_task_path.wop_id_, complete_task_path_, direction_chanage_node_list_, false)) {
			lotrace("agvdriver") << "AgvDriver Agv " << agv_id_ << " Finish Path ReleaseSegmentAndNodeButNotCurrentUpl error...";
		}
		lotrace("agvdriver") << "AgvDriver Agv " << agv_id_ << " Finish task path, Only allocated segment " << current_task_path.edge_id_ << " wop " << current_task_path.wop_id_;

		download_and_allocated_task_path_.clear();
		register_allocated_task_path_.push_back(current_task_path);
	}
	else {
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Finish task path error, download_and_allocated_task_path_.size() = 0";
	}

	have_task_ = false;

	loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Finish task path success";
}

bool AgvDriver::CancelTaskPath()
{
	std::lock_guard<decltype(agv_driver_mutex_)> guard(agv_driver_mutex_);
	if (false == is_register_ || false == have_task_) {
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Cancel task path failed, is register:" << is_register_ << ", already have task:" << have_task_;
		return false;
	}

	{
		std::lock_guard<decltype(agv_upl_mutex_)> guard(agv_upl_mutex_);
		need_to_released_segment_count_ -= released_task_path_.size();

		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Cancel task path, released " << released_task_path_.size() << " segments, Need to Release " << need_to_released_segment_count_ << " segments";
	}
	released_task_path_.clear();

	if (download_and_allocated_task_path_.size() > 0) {
		trail_t current_task_path = download_and_allocated_task_path_[0];

		download_and_allocated_task_path_.clear();
		register_allocated_task_path_.push_back(current_task_path);
	}
	else {
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Cancel task path no path to release...";
	}

	traffic_manage_->ClearNotAllocateSegment(agv_id_);

	have_task_ = false;
	return true;
}

std::vector<trail_t> AgvDriver::GetAllocatedTaskPath(bool &is_finish, int& idx)
{
	std::lock_guard<decltype(agv_driver_mutex_)> guard(agv_driver_mutex_);

    idx = download_and_allocated_task_path_.size() + released_task_path_.size();


	std::vector<trail_t> allocated_task_path = allocated_but_not_download_task_path_;
	
	for (unsigned int i = 0; i < allocated_but_not_download_task_path_.size(); ++i) {
		if (download_and_allocated_task_path_.size() == 0) {
			download_and_allocated_task_path_.push_back(allocated_but_not_download_task_path_[i]);
		}
		else {
			trail_t m_path = download_and_allocated_task_path_[download_and_allocated_task_path_.size() - 1];
			trail_t m_other_path = allocated_but_not_download_task_path_[i];
			if (m_path.edge_id_ != m_other_path.edge_id_ || m_path.wop_id_ != m_other_path.wop_id_) {
				download_and_allocated_task_path_.push_back(allocated_but_not_download_task_path_[i]);
			}
		}
	}

	is_finish = (0 == need_allocate_task_path_.size());

	allocated_but_not_download_task_path_.clear();

	if (allocated_task_path.size() > 0) {
		std::stringstream ss;
		for (unsigned int i = 0; i < allocated_task_path.size(); ++i) {
			trail_t m_task_path = allocated_task_path[i];
			ss << " " << m_task_path.edge_id_ << " " << m_task_path.wop_id_ << ",";
		}
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Get allocated task path, Path list:" << ss.str() << " is finished:" << is_finish;
	} 

	return allocated_task_path;
}

bool AgvDriver::GetBolocked(int &blocked_agv_id)
{
	std::lock_guard<decltype(agv_driver_mutex_)> guard(agv_driver_mutex_);
	blocked_agv_id = blocked_agv_;
	return blocked_;
}

void AgvDriver::AllocateDealCycle()
{
    bool has_path = false;
    {
        std::lock_guard<decltype(agv_driver_mutex_)> guard(agv_driver_mutex_);

        double allocated_segment_distance = this->CalculateAllocatedSegmentDistance();
        while (allocated_segment_distance < agv_max_segment_distance_ || allocated_but_not_download_task_path_.size() + download_and_allocated_task_path_.size() <= 2) {
            if (need_allocate_task_path_.size() > 0) {
                //std::cout << "allocated_segment_distance: " << allocated_segment_distance << std::endl;
                trail_t m_task_path = need_allocate_task_path_[0];
				if (true == traffic_manage_->AllocateSegment(agv_id_, m_task_path.edge_id_, m_task_path.wop_id_, 1, complete_task_path_, direction_chanage_node_list_)) {

                    allocated_but_not_download_task_path_.push_back(m_task_path);
                    need_allocate_task_path_.erase(need_allocate_task_path_.begin());

                    loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " allocated segment " << m_task_path.edge_id_ << " wop " << m_task_path.wop_id_ << " success...";
                }
                else {
                    break;
                }
            }
            else {
                break;
            }
            allocated_segment_distance = this->CalculateAllocatedSegmentDistance();
        }
		
		//static bool distance_too_long_display = false;
		//if (allocated_segment_distance < agv_max_segment_distance_ || allocated_but_not_download_task_path_.size() + download_and_allocated_task_path_.size() <= 2) {

		//	distance_too_long_display = false;
		//}
		//else {
	
		//	if (false == distance_too_long_display) {
		//	
		//		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " allocated_segment_distance : " << allocated_segment_distance
		//			<< " allocated_but_not_download_task_path_.size() = " << allocated_but_not_download_task_path_.size()
		//			<< " download_and_allocated_task_path_.size() = " << download_and_allocated_task_path_.size();
		//		distance_too_long_display = true;
		//	}

		//}

        if (!allocated_but_not_download_task_path_.empty())
        {
            has_path = true;
        }
    }


    if (__path_available_fn )
    {
        __path_available_fn(has_path);
    }
}

void AgvDriver::ReleaseDealCycle()
{
	std::lock_guard<decltype(agv_driver_mutex_)> guard(agv_driver_mutex_);

	upl_t m_current_upl;
	int wop_index; {
		std::lock_guard<decltype(agv_upl_mutex_)> guard(agv_upl_mutex_);
		m_current_upl = current_upl_;
		wop_index = wop_index_;
	}

	this->ReleaseNodeDealCycle(m_current_upl);
	this->ReleaseSegmentDealCycle(m_current_upl, wop_index);
	
	if (true == this->CheckAgvBlocked(m_current_upl, wop_index, blocked_)) {

		this->CheckAgvBlockedByWhichAgv(m_current_upl, wop_index, blocked_agv_);

		if (true == blocked_ && false == blocked_first_print_) {
			loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Blocked in " << m_current_upl.edge_id_ << " " << m_current_upl.percentage_ << " blocked agv: " << blocked_agv_;
			blocked_first_print_ = true;
		}
		else if (false == blocked_ && true == blocked_first_print_) {
			loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Blocked Finished " << m_current_upl.edge_id_ << " " << m_current_upl.percentage_;
			blocked_first_print_ = false;
		}
	}

}

double AgvDriver::CalculateAllocatedSegmentDistance()
{
	std::vector<trail_t> calculate_distance_task_path;

	for (unsigned int i = 0; i < download_and_allocated_task_path_.size(); ++i) {
		calculate_distance_task_path.push_back(download_and_allocated_task_path_[i]);
	}

	for (unsigned int i = 0; i < allocated_but_not_download_task_path_.size(); ++i) {
		calculate_distance_task_path.push_back(allocated_but_not_download_task_path_[i]);
	}

	for (unsigned int i = 0; i < need_allocate_task_path_.size(); ++i) {
		if (0 == i) {
			calculate_distance_task_path.push_back(need_allocate_task_path_[i]);
		}
	}

	double m_max_distance = 999999.9999;
	double m_allocated_segment_distance = 0.0;

	if (calculate_distance_task_path.size() == 0) {
		
	}
	else if (calculate_distance_task_path.size() == 1) {
		trail_t m_task_path = calculate_distance_task_path[0];

		double m_segment_length = -1;
		int m_start_node_id = -1;
		int m_end_node_id = -1;
		bool wop_forward = true;
		double wop_angle = -1;

		if (false == this->SearchSegment(m_task_path, m_start_node_id, m_end_node_id, m_segment_length, wop_forward, wop_angle)) {
			return m_max_distance;
		}

		double m_percent = current_upl_.percentage_;
		if (false == wop_forward) {
			int m_id = m_start_node_id;
			m_start_node_id = m_end_node_id;
			m_end_node_id = m_id;
			m_percent = 1 - m_percent;
		}

		m_allocated_segment_distance += (m_segment_length * (1 - m_percent));
	}
	else {
		bool first_caculate = true;
		for (unsigned int i = 0; i < calculate_distance_task_path.size() - 1; ++i) {

			double m_current_task_path_length = 0.0;

			// first path
			trail_t m_task_path = calculate_distance_task_path[i];

			double m_segment_length = -1;
			int m_start_node_id = -1;
			int m_end_node_id = -1;
			bool wop_forward = true;
			double wop_angle = -1;

			if (false == this->SearchSegment(m_task_path, m_start_node_id, m_end_node_id, m_segment_length, wop_forward, wop_angle)) {
				return m_max_distance;
			}

			if (false == wop_forward) {
				int m_id = m_start_node_id;
				m_start_node_id = m_end_node_id;
				m_end_node_id = m_id;
			}

			// second path
			trail_t m_next_task_path = calculate_distance_task_path[i+1];

			double m_next_segment_length = -1;
			int m_next_start_node_id = -1;
			int m_next_end_node_id = -1;
			bool next_wop_forward = true;
			double next_wop_angle = -1;

			if (false == this->SearchSegment(m_next_task_path, m_next_start_node_id, m_next_end_node_id, m_next_segment_length, next_wop_forward, next_wop_angle)) {
				return m_max_distance;
			}

			if (false == next_wop_forward) {
				int m_id = m_next_start_node_id;
				m_next_start_node_id = m_next_end_node_id;
				m_next_end_node_id = m_id;
			}

			//
			if (m_task_path.edge_id_ == m_next_task_path.edge_id_) {
				if (m_task_path.wop_id_ == m_next_task_path.wop_id_) {
					m_current_task_path_length = 0;
				}
				else {
					if (fabs(wop_angle - next_wop_angle) < 30) {
						m_current_task_path_length = 0;
					}
					else {
						m_current_task_path_length = m_segment_length;
					}
				}
			}
			else {
				m_current_task_path_length = m_segment_length;
			}

			if (true == first_caculate) {
				first_caculate = false;
				double m_percent = current_upl_.percentage_;
				if (false == wop_forward) {
					m_percent = 1 - m_percent;
				}

				m_allocated_segment_distance += (m_current_task_path_length * (1 - m_percent));
			}
			else {
				m_allocated_segment_distance += m_current_task_path_length;
			}

			if (i + 1 == calculate_distance_task_path.size() - 1) {
				m_allocated_segment_distance += m_next_segment_length;
			}

		}
	}

	return m_allocated_segment_distance;
}

bool AgvDriver::SearchSegment(trail_t task_path, int &start_node_id, int &end_node_id, double &segment_length, bool &wop_forward, double &wop_angle)
{
	bool find_segment = false;
	PathSearch_LayoutData *layout_data = traffic_manage_->GetLayoutData();
	for (unsigned int i = 0; i < layout_data->edgeList.size(); ++i) {
		PathSearch_EDGEINFO &m_edge = layout_data->edgeList[i];
		if (m_edge.id == task_path.edge_id_) {
			segment_length = m_edge.length / 100.0;
			start_node_id = m_edge.start_node_id;
			end_node_id = m_edge.end_node_id;
			find_segment = true;
			break;
		}
	}

	if (false == find_segment) {
		return false;
	}

	bool find_wop = false;
	for (unsigned int i = 0; i < layout_data->wopList.size(); ++i) {
		PathSearch_Wop &m_wop = layout_data->wopList[i];
		if (m_wop.wop_id == task_path.wop_id_) {
			wop_forward = (m_wop.direction == PathSearch_FORWARD);
			wop_angle = m_wop.angle;
			find_wop = true;
			break;
		}
	}

	if (false == find_wop) {
		return false;
	}
	return true;
}

void AgvDriver::ReleaseNodeDealCycle(upl_t m_current_upl)
{
	unsigned int need_released_segment_count = need_to_released_segment_count_;

	if (download_and_allocated_task_path_.size() == 0) {
		return;
	}

	trail_t m_pre_release_task_path = download_and_allocated_task_path_[0];

	double m_segment_length = -1;
	int m_start_node_id = -1;
	int m_end_node_id = -1;
	bool wop_forward = true;
	double wop_angle = -1;

	if (false == this->SearchSegment(m_pre_release_task_path, m_start_node_id, m_end_node_id, m_segment_length, wop_forward, wop_angle)) {
		return;
	}

	double m_percent = current_upl_.percentage_;
	if (false == wop_forward) {
		int m_id = m_start_node_id;
		m_start_node_id = m_end_node_id;
		m_end_node_id = m_id;
		m_percent = 1 - m_percent;
	}

	if (released_task_path_.size() == need_released_segment_count) {
		// Normal Release Logistic
		if (m_segment_length * m_percent > agv_release_distance_) {
			if (true == traffic_manage_->ReleaseNode(agv_id_, m_start_node_id)) {
				loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " release node " << m_start_node_id
					<< " upl segment " << m_current_upl.edge_id_ << " angle " << (m_current_upl.angle_ * 180.0 / 3.1415926) << " percent " << m_current_upl.percentage_;
			}
		}
	}
	else {
		// wireless not good, segment jump
		if (true == traffic_manage_->ReleaseNode(agv_id_, m_start_node_id)) {
			loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " release node " << m_start_node_id
				<< " upl segment " << m_current_upl.edge_id_ << " angle " << (m_current_upl.angle_ * 180.0 / 3.1415926) << " percent " << m_current_upl.percentage_;
		}
	}
}

void AgvDriver::ReleaseSegmentDealCycle(upl_t m_current_upl, int wop_index)
{
	unsigned int need_released_segment_count = need_to_released_segment_count_;

	if (download_and_allocated_task_path_.size() <= 1) {
		return;
	}

	trail_t m_pre_release_task_path = download_and_allocated_task_path_[0];
	double m_pre_segment_length = -1;
	int m_pre_start_node_id = -1;
	int m_pre_end_node_id = -1;
	bool wop_forward = true;
	double wop_angle = -1;

	if (false == this->SearchSegment(m_pre_release_task_path, m_pre_start_node_id, m_pre_end_node_id, m_pre_segment_length, wop_forward, wop_angle)) {
		return;
	}

	if (false == wop_forward) {
		int m_id = m_pre_start_node_id;
		m_pre_start_node_id = m_pre_end_node_id;
		m_pre_end_node_id = m_id;
	}

	trail_t m_next_release_task_path = download_and_allocated_task_path_[1];
	double m_next_segment_length = -1;
	int m_next_start_node_id = -1;
	int m_next_end_node_id = -1;
	double m_next_wop_angle = -1;

	if (false == this->SearchSegment(m_next_release_task_path, m_next_start_node_id, m_next_end_node_id, m_next_segment_length, wop_forward, m_next_wop_angle)) {
		return;
	}

	double m_percent = current_upl_.percentage_;
	if (false == wop_forward) {
		int m_id = m_next_start_node_id;
		m_next_start_node_id = m_next_end_node_id;
		m_next_end_node_id = m_id;
		m_percent = 1 - m_percent;
	}

	bool release_segment_ok = false;

	// Release Segment
	if (released_task_path_.size() + 1 == need_released_segment_count) {
		// Normal Release Logistic
		
		if (m_next_segment_length * m_percent > agv_release_distance_) {

			bool have_allocated_segment = false;

			for (unsigned int m_index = 1; m_index < download_and_allocated_task_path_.size(); ++m_index) {
				trail_t m_trail = download_and_allocated_task_path_[m_index];
				if (m_trail.edge_id_ == m_pre_release_task_path.edge_id_ && m_trail.wop_id_ == m_pre_release_task_path.wop_id_) {
					have_allocated_segment = true;
				}
			}

			for (unsigned int m_index = 0; m_index < allocated_but_not_download_task_path_.size(); ++m_index) {

				trail_t m_trail = allocated_but_not_download_task_path_[m_index];
				if (m_trail.edge_id_ == m_pre_release_task_path.edge_id_ && m_trail.wop_id_ == m_pre_release_task_path.wop_id_) {
					have_allocated_segment = true;
				}
			}

			if (false == have_allocated_segment) {
				if (traffic_manage_->GetAgvAllocateSegmentCount(agv_id_) >= 2) {
					traffic_manage_->ReleaseSegment(agv_id_, m_pre_release_task_path.edge_id_, m_pre_release_task_path.wop_id_);
					traffic_manage_->ReleaseNode(agv_id_, m_pre_end_node_id);
					loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " release traffic segment " << m_pre_release_task_path.edge_id_ << " wop " << m_pre_release_task_path.wop_id_;
				}
				else {
					loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " allocated count < 2 not release traffic segment " << m_pre_release_task_path.edge_id_ << " wop " << m_pre_release_task_path.wop_id_;
				}
			}
			else {
				loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " not release traffic segment " << m_pre_release_task_path.edge_id_ << " wop " << m_pre_release_task_path.wop_id_;
			}

			download_and_allocated_task_path_.erase(download_and_allocated_task_path_.begin());

			released_task_path_.push_back(m_pre_release_task_path);

			loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " release node " << m_pre_end_node_id
				<< " upl segment " << m_current_upl.edge_id_ << " agnle " << (m_current_upl.angle_ * 180.0 / 3.1415926) << " percent " << m_current_upl.percentage_;
			loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " release segment " << m_pre_release_task_path.edge_id_ << " wop " << m_pre_release_task_path.wop_id_;
		
			release_segment_ok = true;
		}
	}
	else if (released_task_path_.size() + 1 < need_released_segment_count) {
		// wireless not good, segment jump

		bool have_allocated_segment = false;

		for (unsigned int m_index = 1; m_index < download_and_allocated_task_path_.size(); ++m_index) {
			trail_t m_trail = download_and_allocated_task_path_[m_index];
			if (m_trail.edge_id_ == m_pre_release_task_path.edge_id_ && m_trail.wop_id_ == m_pre_release_task_path.wop_id_) {
				have_allocated_segment = true;
			}
		}

		for (unsigned int m_index = 0; m_index < allocated_but_not_download_task_path_.size(); ++m_index) {

			trail_t m_trail = allocated_but_not_download_task_path_[m_index];
			if (m_trail.edge_id_ == m_pre_release_task_path.edge_id_ && m_trail.wop_id_ == m_pre_release_task_path.wop_id_) {
				have_allocated_segment = true;
			}
		}

		if (false == have_allocated_segment) {
			if (traffic_manage_->GetAgvAllocateSegmentCount(agv_id_) >= 2) {
				traffic_manage_->ReleaseSegment(agv_id_, m_pre_release_task_path.edge_id_, m_pre_release_task_path.wop_id_);
				traffic_manage_->ReleaseNode(agv_id_, m_pre_end_node_id);
				loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " release traffic segment " << m_pre_release_task_path.edge_id_ << " wop " << m_pre_release_task_path.wop_id_;
			}
			else {
				loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " allocated count < 2 not release traffic segment " << m_pre_release_task_path.edge_id_ << " wop " << m_pre_release_task_path.wop_id_;
			}
		}
		else {
			loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " not release traffic segment " << m_pre_release_task_path.edge_id_ << " wop " << m_pre_release_task_path.wop_id_;
		}

		download_and_allocated_task_path_.erase(download_and_allocated_task_path_.begin());

		released_task_path_.push_back(m_pre_release_task_path);

		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " release node " << m_pre_end_node_id
			<< " upl segment " << m_current_upl.edge_id_ << " agnle " << (m_current_upl.angle_ * 180.0 / 3.1415926) << " percent " << m_current_upl.percentage_;
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " release segment " << m_pre_release_task_path.edge_id_ << " wop " << m_pre_release_task_path.wop_id_;

		release_segment_ok = true;
	}

	if (true == release_segment_ok) {
		int m_current_upl_index_in_not_relaase_segment = this->CheckCurrentUplIndexInNotReleaseSegment(m_current_upl, wop_index);
		
		loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Check Current Upl Index In Not Release Segment :" << m_current_upl_index_in_not_relaase_segment;
		
		if (last_current_upl_index_in_not_relaase_segment < 0) {
			last_current_upl_index_in_not_relaase_segment = m_current_upl_index_in_not_relaase_segment;
		}

		if (m_current_upl_index_in_not_relaase_segment > 0) {

			if (last_current_upl_index_in_not_relaase_segment == m_current_upl_index_in_not_relaase_segment) {

				not_release_segment_error_count_++;
				loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Check Current Upl Index In Not Release Segment > 0, not_release_segment_error_count = " << not_release_segment_error_count_;
			}
			else {
				not_release_segment_error_count_ = 0;
				loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " not repeate, not_release_segment_error_count clear";
			}
		}
		else if (m_current_upl_index_in_not_relaase_segment == 0) {
			not_release_segment_error_count_ = 0;
			loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " m_current_upl_index_in_not_relaase_segment = 0, not_release_segment_error_count clear";
		}

		last_current_upl_index_in_not_relaase_segment = m_current_upl_index_in_not_relaase_segment;

		if (not_release_segment_error_count_ > 10) {
			not_release_segment_error_count_ = 0;
			need_to_released_segment_count_ += 1;
			loinfo("agvdriver") << "AgvDriver Agv " << agv_id_ << " Check Current Upl Index In Not Release Segment Count > 10, need_to_released_segment_count + 1";
		}
	}
}

int AgvDriver::CheckCurrentUplIndexInNotReleaseSegment(upl_t m_current_upl, int wop_index)
{
	if (complete_task_path_.size() > (unsigned int)wop_index) {
		if (m_current_upl.edge_id_ == complete_task_path_[wop_index].edge_id_) {

			trail_t m_current_wop = complete_task_path_[wop_index];

			unsigned int m_index = 0;
			for (; m_index < download_and_allocated_task_path_.size(); m_index++) {
				trail_t m_not_release_wop = download_and_allocated_task_path_[m_index];
				if (m_current_wop.edge_id_ == m_not_release_wop.edge_id_ && m_current_wop.wop_id_ == m_not_release_wop.wop_id_) {
					break;
				}
			}

			if (m_index >= download_and_allocated_task_path_.size()) {
				return -1;
			}
			return m_index;
		}
		else {
			return -2;
		}
	}
	else {
		return -3;
	}
	return -4;
}

bool AgvDriver::CheckAgvBlocked(upl_t m_current_upl, int wop_index, bool &blocked)
{
	if (need_allocate_task_path_.size() <= 0) {

		blocked = false;
		return true;
	}

	std::vector<trail_t> m_allocated_task_path;
	for (unsigned int i = 0; i < allocated_but_not_download_task_path_.size(); ++i) {
		m_allocated_task_path.push_back(allocated_but_not_download_task_path_[i]);
	}

	for (unsigned int i = 0; i < download_and_allocated_task_path_.size(); ++i) {
		m_allocated_task_path.push_back(download_and_allocated_task_path_[i]);
	}

	if (m_allocated_task_path.size() > 1) {

		blocked = false;
		return true;
	}
	else if (m_allocated_task_path.size() == 1) {

		if (complete_task_path_.size() > (unsigned int)wop_index) {
			if (m_current_upl.edge_id_ == complete_task_path_[wop_index].edge_id_) {

				trail_t m_current_wop = complete_task_path_[wop_index];

				PathSearch_LayoutData *layout_data = traffic_manage_->GetLayoutData();

				double m_segment_length = 0.0;
				for (unsigned int i = 0; i < layout_data->edgeList.size(); ++i) {
					PathSearch_EDGEINFO &m_edge = layout_data->edgeList[i];
					if (m_edge.id == m_current_wop.edge_id_) {
						m_segment_length = m_edge.length / 100.0;
						break;
					}
				}

				bool wop_forward = true;
				bool find_wop = false;
				for (unsigned int i = 0; i < layout_data->wopList.size(); ++i) {
					PathSearch_Wop &m_wop = layout_data->wopList[i];
					if (m_wop.wop_id == m_current_wop.wop_id_) {
						wop_forward = (m_wop.direction == PathSearch_FORWARD);
						find_wop = true;
						break;
					}
				}

				if (true == find_wop) {

					double m_percent = current_upl_.percentage_;
					if (false == wop_forward) {
						m_percent = 1 - m_percent;
					}

					if ( (1 - m_percent) * m_segment_length < 0.1 ) {

						blocked = true;
						return true;
					}
					else {
						blocked = false;
						return true;
					}
				}
			}
		}
	}
	else {

		blocked = true;
		return true;
	}
	return false;
}

bool AgvDriver::CheckAgvBlockedByWhichAgv(upl_t m_current_upl, int wop_index, int &blocked_agv)
{
	bool m_blocked = false;
	if (false == this->CheckAgvBlocked(m_current_upl, wop_index, m_blocked)) {
		
		blocked_agv = -1;
		return false;
	}

	if (false == m_blocked) {

		blocked_agv = -1;
		return false;
	}

	SegmentAllocatedFialedReason m_allocated_failed_reason;
	if (false == traffic_manage_->CheckAgvBlockedReason(agv_id_, m_allocated_failed_reason)) {

		blocked_agv = -1;
		return false;
	}

	if (m_allocated_failed_reason.failed_reason_ == SegmentAllocatedFialedReason::NoFailedReason) {

		blocked_agv = -1;
		return false;
	}
	else if (m_allocated_failed_reason.failed_reason_ == SegmentAllocatedFialedReason::SegmentFailed) {

		if (true == traffic_manage_->CheckSegmentWopAllocatedAgv(m_allocated_failed_reason.segment_id_, m_allocated_failed_reason.wop_id_, blocked_agv)) {

			return true;
		}
	}
	else if (m_allocated_failed_reason.failed_reason_ == SegmentAllocatedFialedReason::OcupiedClusteFailedr) {
		
		if (true == traffic_manage_->CheckOcupiedClusterAllocatedAgv(m_allocated_failed_reason.ocupied_cluster_id_, blocked_agv)) {

			return true;
		}
	}
	else if (m_allocated_failed_reason.failed_reason_ == SegmentAllocatedFialedReason::AckClusterFailed) {
		
		if (true == traffic_manage_->CheckAckClusterAllocatedAgv(m_allocated_failed_reason.ack_cluster_id_, blocked_agv)) {

			return true;
		}
	}

	blocked_agv = -1;
	return false;
}

void AgvDriver::SetAllocatedAvailabeCallback(const std::function<int(int has_path)> &fn)
{
    __path_available_fn = fn;
}
