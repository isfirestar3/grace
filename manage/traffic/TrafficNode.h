#ifndef TRAFFIC_MANAGE_TRAFFIC_NODE_WEILEI_20170529_H_
#define TRAFFIC_MANAGE_TRAFFIC_NODE_WEILEI_20170529_H_

#include <iostream>
#include <map>

class TrafficSegment;

class TrafficNode
{
public:
	TrafficNode(int node_id);
	~TrafficNode();

public:
	void AddEnterSegment(TrafficSegment *enter_segment);
	void AddOuterSegment(TrafficSegment *outer_segment);

	std::map<int, TrafficSegment*> GetEnterSegments();
	std::map<int, TrafficSegment*> GetOuterSegments();

public:
	int GetNodeId();

public:
	int GetAllocateAgv();

public:
	bool IsAllocated(int agv_id);
	bool AllocateNode(int agv_id);
	bool ReleaseNode(int agv_id);

private:
	int node_id_;

	std::map<int, TrafficSegment*> enter_segments_;
	std::map<int, TrafficSegment*> outer_segments_;

	int allocate_agv_;
};

#endif
