#include "TrafficNode.h"
#include "TrafficSegment.h"
#include "log.h"

TrafficSegment::TrafficSegment(int segment_id, int wop_id, int start_id, int end_id, double length)
	: segment_id_(segment_id), wop_id_(wop_id), start_id_(start_id), end_id_(end_id), length_(length),
	allocate_agv_(-1)
{
	atuo_mutex_nodes_.clear();
	auto_mutex_segments_.clear();

	manual_mutex_nodes_.clear();
	manual_mutex_segments_.clear();
}

TrafficSegment::~TrafficSegment()
{

}

void TrafficSegment::SetTrafficStartNode( TrafficNode *node )
{
	start_node_ = node;
}

void TrafficSegment::SetTrafficEndNode( TrafficNode *node )
{
	end_node_ = node;
}

void TrafficSegment::AddAutoMutexNode( TrafficNode * node )
{
	if (NULL != node) {
		int m_node_id = node->GetNodeId();
		atuo_mutex_nodes_[m_node_id] = node;
	}
}

void TrafficSegment::AddAutoMutexSegment( TrafficSegment* segment )
{
	if (NULL != segment && this != segment) {
		int m_segment_id = segment->GetSegmentId();
		int m_wop_id = segment->GetWopId();
		auto_mutex_segments_[std::pair<int, int>(m_segment_id, m_wop_id)] = segment;
	}
}

void TrafficSegment::AddManualMutexNode( TrafficNode * node )
{
	if (NULL != node) {
		int m_node_id = node->GetNodeId();
		manual_mutex_nodes_[m_node_id] = node;
	}
}

void TrafficSegment::AddManualMutexSegment(TrafficSegment* segment)
{
	if (NULL != segment && this != segment) {
		int m_segment_id = segment->GetSegmentId();
		int m_wop_id = segment->GetWopId();
		manual_mutex_segments_[std::pair<int, int>(m_segment_id, m_wop_id)] = segment;
	}
}

TrafficNode * TrafficSegment::GetTrafficStartNode()
{
	return start_node_;
}

TrafficNode * TrafficSegment::GetTrafficEndNode()
{
	return end_node_;
}

int TrafficSegment::GetSegmentId()
{
	return segment_id_;
}

int TrafficSegment::GetWopId()
{
	return wop_id_;
}

int TrafficSegment::GetStartId()
{
	return start_id_;
}

int TrafficSegment::GetEndId()
{
	return end_id_;
}

double TrafficSegment::GetLength()
{
	return length_;
}


int TrafficSegment::GetAllocateAgv()
{
	return allocate_agv_;
}

bool TrafficSegment::IsAllocated(int agv_id)
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

bool TrafficSegment::CheckCanAllocated(int agv_id, int &allcoated_auto_mutex_segment, int &allocated_auto_mutex_wop, int &allocated_manual_mutex_segment, int &allocated_manual_mutex_wop)
{
	if (agv_id == allocate_agv_) {
		return true;
	}

	// 1.检查边是否被占用
	if (true == this->IsAllocated(agv_id)) {
		return false;
	}

	// 注释后，仅仅使用边互斥
	//// 2.检查终点是否被占用
	//if (true == end_node_->IsAllocated(agv_id)) {
	//	return false;
	//}

	//// 3.检查任何指向终点的其他边是否被占用
	//std::map<int, TrafficSegment*> enter_segment = end_node_->GetEnterSegments();
	//if (true == this->CheckSegmentsAllocated(enter_segment, agv_id)) {
	//	return false;
	//}

	// 4.检查自动互斥节点、边是否被占用
	if (true == this->CheckNodesAllocated(atuo_mutex_nodes_, agv_id)) {
		return false;
	}

	if (true == this->CheckSegmentsAllocated(auto_mutex_segments_, agv_id, allcoated_auto_mutex_segment, allocated_auto_mutex_wop)) {
		return false;
	}

	// 5.检查人工互斥节点、边是否被占用
	if (true == this->CheckNodesAllocated(manual_mutex_nodes_, agv_id)) {
		return false;
	}

	if (true == this->CheckSegmentsAllocated(manual_mutex_segments_, agv_id, allocated_manual_mutex_segment, allocated_manual_mutex_wop)) {
		return false;
	}

	return true;
}

bool TrafficSegment::CheckCanAllocated(int agv_id)
{
	int allcoated_auto_mutex_segment = -1;
	int allocated_auto_mutex_wop = -1;
	int allocated_manual_mutex_segment = -1;
	int allocated_manual_mutex_wop = -1;
	return this->CheckCanAllocated(agv_id, allcoated_auto_mutex_segment, allocated_auto_mutex_wop, allocated_manual_mutex_segment, allocated_manual_mutex_wop);
}

bool TrafficSegment::AllocateSegment( int agv_id, bool is_init )
{
	std::lock_guard<decltype(mutex_allocated_agv_)> guard(mutex_allocated_agv_);
	
	
	if (false == this->CheckCanAllocated(agv_id)) {
		return false;
	}

	// 6.占用起点、终点
	if (true == is_init) {
		start_node_->AllocateNode(agv_id);
	}

	end_node_->AllocateNode(agv_id);

	// 7.占用边
	allocate_agv_ = agv_id;
	loinfo("traffic") << "TrafficSegment agv " << agv_id << " allocated segment:" << segment_id_ << " wop:" << wop_id_;

	return true;
}

bool TrafficSegment::ReleaseSegment(int agv_id)
{
	std::lock_guard<decltype(mutex_allocated_agv_)> guard(mutex_allocated_agv_);
	if (allocate_agv_ == agv_id) {

		loinfo("traffic") << "TrafficSegment agv " << agv_id << " release segment:" << segment_id_ << " wop:" << wop_id_;
		allocate_agv_ = -1;
		return true;
	}
	else {
		//loinfo("traffic") << "TrafficSegment agv " << agv_id << " is not allocated segment:" << segment_id_ << " wop:" << wop_id_;
		return false;
	}
}

bool TrafficSegment::CheckNodesAllocated( std::map<int, TrafficNode *> nodes, int agv_id )
{
	for (std::map<int, TrafficNode*>::iterator iter = nodes.begin();
		iter != nodes.end(); ++iter) {
			if (true == iter->second->IsAllocated(agv_id)) {
				return true;
			}
	}
	return false;
}

bool TrafficSegment::CheckSegmentsAllocated(std::map<std::pair<int, int>, TrafficSegment *> segments, int agv_id, int &allocated_blocking_segment, int &allocated_blocking_wop)
{
	for (std::map<std::pair<int, int>, TrafficSegment*>::iterator iter = segments.begin();
		iter != segments.end(); ++iter) {
			if (true == iter->second->IsAllocated(agv_id)) {
				allocated_blocking_segment = iter->second->GetSegmentId();
				allocated_blocking_wop = iter->second->GetWopId();
				return true;
			}
	}
	return false;
}