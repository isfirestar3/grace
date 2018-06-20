#include "TrafficNode.h"
#include "TrafficSegment.h"
#include "log.h"

TrafficNode::TrafficNode(int node_id)
: node_id_(node_id), allocate_agv_(-1)
{
	enter_segments_.clear();
	outer_segments_.clear();
}

TrafficNode::~TrafficNode()
{

}

void TrafficNode::AddEnterSegment(TrafficSegment *enter_segment)
{
	enter_segments_[enter_segment->GetSegmentId()] = enter_segment;
}

void TrafficNode::AddOuterSegment(TrafficSegment *outer_segment)
{
	outer_segments_[outer_segment->GetSegmentId()] = outer_segment;
}

std::map<int, TrafficSegment*> TrafficNode::GetEnterSegments()
{
	return enter_segments_;
}

std::map<int, TrafficSegment*> TrafficNode::GetOuterSegments()
{
	return outer_segments_;
}

int TrafficNode::GetNodeId()
{
	return node_id_;
}

int TrafficNode::GetAllocateAgv()
{
	return allocate_agv_;
}

bool TrafficNode::IsAllocated(int agv_id)
{
	if (-1 == allocate_agv_) {
		return false;
	}
	else {
		if (agv_id == allocate_agv_) {
			return false;
		}
		else {
			return true;
		}
	}
}

bool TrafficNode::AllocateNode(int agv_id)
{
	if (true == this->IsAllocated(agv_id)) {
		return false;
	}
	else {
		allocate_agv_ = agv_id;
		loinfo("traffic") << "TrafficNode node " << node_id_ << " is allocated to agv " << agv_id;
		return true;
	}
}

bool TrafficNode::ReleaseNode(int agv_id)
{
	if (allocate_agv_ == agv_id) {
		loinfo("traffic") << "TrafficNode agv " << agv_id << " release node " << node_id_;
		allocate_agv_ = -1;
		return true;
	}
	else {
		//loinfo("traffic") << "TrafficNode agv " << agv_id << " is not allocated node " << node_id_;
		return false;
	}
}
