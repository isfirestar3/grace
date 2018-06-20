#ifndef TRAFFIC_MANAGE_TRAFFIC_SEGMENT_WEILEI_20170529_H_
#define TRAFFIC_MANAGE_TRAFFIC_SEGMENT_WEILEI_20170529_H_

#include <iostream>
#include <map>
#include "mutex"

class TrafficNode;
class TrafficSegment;

class TrafficSegment
{
public:
	TrafficSegment(int segment_id, int wop_id, int start_id, int end_id, double length);
	~TrafficSegment();

public:
	void SetTrafficStartNode(TrafficNode *node);
	void SetTrafficEndNode(TrafficNode *node);

public:
	void AddAutoMutexNode(TrafficNode * node);
	void AddAutoMutexSegment(TrafficSegment* segment);

	void AddManualMutexNode(TrafficNode * node);
	void AddManualMutexSegment(TrafficSegment* segment);

public:
	int GetSegmentId();
	int GetWopId();
	int GetStartId();
	int GetEndId();
	double GetLength();

public:
	TrafficNode *GetTrafficStartNode();
	TrafficNode *GetTrafficEndNode();

public:
	int GetAllocateAgv();
	
public:
	bool IsAllocated(int agv_id);
	bool CheckCanAllocated(int agv_id, int &allcoated_auto_mutex_segment, int &allocated_auto_mutex_wop, int &allocated_manual_mutex_segment, int &allocated_manual_mutex_wop);
	bool CheckCanAllocated(int agv_id);
	bool AllocateSegment(int agv_id, bool is_init = false);
	bool ReleaseSegment(int agv_id);

private:
	bool CheckNodesAllocated(std::map<int, TrafficNode *> nodes, int agv_id);
	bool CheckSegmentsAllocated(std::map<std::pair<int, int>, TrafficSegment *> segments, int agv_id, int &allocated_blocking_segment, int &allocated_blocking_wop);

private:
	int segment_id_;
	int wop_id_;
	int start_id_;
	int end_id_;
	double length_;

	TrafficNode *start_node_;
	TrafficNode *end_node_;

	std::map<int, TrafficNode *> atuo_mutex_nodes_;
	std::map<std::pair<int, int>, TrafficSegment *> auto_mutex_segments_;

	std::map<int, TrafficNode *> manual_mutex_nodes_;
	std::map<std::pair<int, int>, TrafficSegment *> manual_mutex_segments_;


private:
	int allocate_agv_;

	std::mutex mutex_allocated_agv_;
};

#endif
