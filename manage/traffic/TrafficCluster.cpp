#include "TrafficCluster.h"
#include "log.h"

TrafficCluster::TrafficCluster(int cluster_id, std::vector<int> nodes)
	: cluster_id_(cluster_id), nodes_(nodes), allocate_agv_(-1)
{
	
}

TrafficCluster::~TrafficCluster()
{

}

bool TrafficCluster::CheckNodeInCluster(int node_id)
{
	bool have_node = false;

	for (unsigned int i = 0; i < nodes_.size(); ++i) {
		int m_node = nodes_[i];
		if (m_node == node_id) {
			have_node = true;
		}
	}

	if (true == have_node) {
		return true;
	}
	else {
		return false;
	}
	return false;
}

bool TrafficCluster::CheckNodesInCluster(std::vector<int> node_list, int &node_id)
{
	for (unsigned int i = 0; i < node_list.size(); ++i) {
		if (true == this->CheckNodeInCluster(node_list[i])) {
			node_id = node_list[i];
			return true;
		}
	}
	return false;
}

bool TrafficCluster::CheckSegmentInCluster(int start_id, int end_id)
{
	bool have_start_node = false;
	bool have_end_node = false;

	for (unsigned int i = 0; i < nodes_.size(); ++i) {
		int m_node = nodes_[i];
		if (m_node == start_id) {
			have_start_node = true;
		}

		if (m_node == end_id) {
			have_end_node = true;
		}
	}

	if (true == have_start_node || true == have_end_node) {
		return true;
	}
	else {
		return false;
	}
	return false;
}

void TrafficCluster::AddSegment(int start_id, int end_id)
{
	bool have_same = false;
	for (std::vector<std::pair<int, int> >::iterator iter = segments_.begin(); iter != segments_.end(); ++iter) {
		std::pair<int, int> m_segment = *iter;
		if (start_id == m_segment.first && end_id == m_segment.second) {
			have_same = true;
		}
	}
	if (false == have_same) {
		segments_.push_back(std::pair<int, int>(start_id, end_id));
		loinfo("traffic") << "TrafficCluster Cluster " << cluster_id_ << " Add Segment " << start_id << "->" << end_id << " ...";
	}
	else {
		loinfo("traffic") << "TrafficCluster Cluster " << cluster_id_ << " Add Segment " << start_id << "->" << end_id << " Repeated, Not need to add...";
	}
}

bool TrafficCluster::CheckSegmentOccupiedInCluster(int start_id, int end_id)
{
	for (unsigned int i = 0; i < segments_.size(); ++i) {
		std::pair<int, int> m_segment = segments_[i];
		if (start_id == m_segment.first && end_id == m_segment.second) {
			return true;
		}
	}
	return false;
}

void TrafficCluster::RemoveSegment(int start_id, int end_id)
{
	for (std::vector<std::pair<int, int> >::iterator iter = segments_.begin(); iter != segments_.end(); ++iter) {
		std::pair<int, int> m_segment = *iter;
		if (start_id == m_segment.first && end_id == m_segment.second) {
			segments_.erase(iter);
			loinfo("traffic") << "TrafficCluster Cluster " << cluster_id_ << " Remove Segment " << start_id << "->" << end_id;
			break;
		}
	}
}

bool TrafficCluster::CheckSegmentEmpty()
{
	if (0 == segments_.size()) {
		return true;
	}
	return false;
}

int TrafficCluster::GetAllocateAgv()
{
	return allocate_agv_;
}

bool TrafficCluster::IsAllocated(int agv_id)
{
	if (-1 == allocate_agv_) {

		return false;
	}
	else {
		if (allocate_agv_ != agv_id) {
			return true;
		}
		else {
			return false;
		}
	}
}

bool TrafficCluster::AllocateCluster(int agv_id)
{
	std::lock_guard<decltype(mutex_allocated_agv_)> guard(mutex_allocated_agv_);
	if (true == this->IsAllocated(agv_id)) {
		return false;
	}
	else {
		allocate_agv_ = agv_id;
		loinfo("traffic") << "TrafficCluster Cluster " << cluster_id_ << " is allocated to agv " << agv_id;
		return true;
	}
}

bool TrafficCluster::ReleaseCluster(int agv_id)
{
	std::lock_guard<decltype(mutex_allocated_agv_)> guard(mutex_allocated_agv_);
	if (allocate_agv_ == agv_id) {
		loinfo("traffic") << "TrafficCluster agv " << agv_id << " release Cluster " << cluster_id_;
		segments_.clear();
		allocate_agv_ = -1;
		return true;
	}
	else {
		//loinfo("traffic") << "TrafficCluster agv " << agv_id << " is not allocated Cluster " << cluster_id_;
		return false;
	}
}