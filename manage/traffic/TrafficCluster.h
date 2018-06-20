#ifndef TRAFFIC_MANAGE_TRAFFIC_CLUSTER_WEILEI_20170529_H_
#define TRAFFIC_MANAGE_TRAFFIC_CLUSTER_WEILEI_20170529_H_

#include <iostream>
#include <vector>
#include <mutex>

class TrafficCluster
{
public:
	TrafficCluster(int cluster_id, std::vector<int> nodes);
	~TrafficCluster();

public:
	bool CheckNodeInCluster(int node_id);
	bool CheckNodesInCluster(std::vector<int> node_list, int &node_id);
	bool CheckSegmentInCluster(int start_id, int end_id);

	void AddSegment(int start_id, int end_id);

	bool CheckSegmentOccupiedInCluster(int start_id, int end_id);
	void RemoveSegment(int start_id, int end_id);

	bool CheckSegmentEmpty();

public:
	int GetAllocateAgv();

public:
	bool IsAllocated(int agv_id);
	bool AllocateCluster(int agv_id);
	bool ReleaseCluster(int agv_id);

private:
	int cluster_id_;
	std::vector<int> nodes_;

private:
	int allocate_agv_;

	std::mutex mutex_allocated_agv_; 

private:
	std::vector<std::pair<int, int> > segments_;

};

#endif
