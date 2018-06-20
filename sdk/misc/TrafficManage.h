#ifndef TRAFFIC_MANAGE_TRAFFIC_MANAGE_WEILEI_0529_H_
#define TRAFFIC_MANAGE_TRAFFIC_MANAGE_WEILEI_0529_H_

#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include "PathSearch.h"
#include "navigation.h"

#if !defined Pi
#define Pi  3.1415926
#endif

class TrafficNode;
class TrafficSegment;
class TrafficCluster;
class RDBManage;

struct SegmentNotAllocateAgv
{
	int segment_id_;
	int wop_id_;
	std::vector<trail_t> complete_path_;
	std::vector<int> direction_change_node_list_;
	bool is_init_;

	int agv_id_;
	int priority_;
	int allocate_time_;

	bool is_allocated_;
};

struct SegmentAllocatedFialedReason
{
	enum FailedReason {
		NoFailedReason,
		SegmentFailed,
		OcupiedClusteFailedr,
		AckClusterFailed
	};

	FailedReason failed_reason_;
	int segment_id_;
	int wop_id_;

	int ocupied_cluster_id_;
	int ack_cluster_id_;

	SegmentAllocatedFialedReason() {

		failed_reason_ = NoFailedReason;

		segment_id_ = -1;
		wop_id_ = -1;

		ocupied_cluster_id_ = -1;
		ack_cluster_id_ = -1;
	}
};

class TrafficManage
{
public:
	TrafficManage();
	~TrafficManage();

public:
	bool LoadMapData(PathSearch_LayoutData *layout);

	bool LoadAutoTrafficFromFile(std::string file_name);
	bool LoadManulTrafficFormFile(std::string file_name);

	bool LoadOccupiedCluster(std::string file_name);
	bool LoadAckCluster(std::string file_name);

	bool LoadRdbConfig(std::string file_name);

	void StartTrafficManage();

public:
	bool CheckSegmentCanAllocated(int agv_id, int segment_id, int wop_id);

	bool AllocateSegment(int agv_id, int segment_id, int wop_id, bool is_init = false);
	bool AllocateSegment(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init = false);
	bool ReleaseSegment(int agv_id, int segment_id, int wop_id);

	bool ReleaseNode(int agv_id, int node_id);

	bool ReleaseSegmentAndNode(int agv_id);

	bool ReleaseSegmentAndNodeButNotCurrentUpl(int agv_id, int segment_id, int wop_id, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init = false);

	void ClearNotAllocateSegment(int agv_id);

	bool CheckAgvBlockedReason(int agv_id, SegmentAllocatedFialedReason &failed_reason);
	bool CheckSegmentWopAllocatedAgv(int segment_id, int wop_id, int& agv_id);
	bool CheckOcupiedClusterAllocatedAgv(int cluster_id, int& agv_id);
	bool CheckAckClusterAllocatedAgv(int cluster_id, int& agv_id);

public:
	void FreezeAll(bool freeze_all);
	bool GetFreezeAll();

public:
	PathSearch_LayoutData* GetLayoutData();
	std::vector<std::pair<int, int> > GetNodeAllocateAgv();
	std::vector<std::pair<std::pair<int, int>, int> > GetSegmentAllocateAgv();
	std::map<int, std::vector<std::pair<int, int> > > GetAgvAllocateSegment();
	int GetAgvAllocateSegmentCount(int agv_id);

	TrafficSegment* FindTrafficSegment(int segment_id, int wop_id);
	TrafficCluster* FindTrafficOcupiedCluster(int cluster_id);
	TrafficCluster* FindTrafficAckCluster(int cluster_id);

private:
	bool LoadTrafficSegment(int segment_id, int wop_id, int start_id, int end_id, double length);

	bool LoadAutoTrafficMutex(int segment_id, int wop_id, int mutex_segment_id, int mutex_wop_id);
	bool LoadManualTrafficMutex(int segment_id, int wop_id, int mutex_segment_id, int mutex_wop_id);


	int SplitString(const std::string& input, const std::string& delimiter, std::vector<std::string>& results);

private:
	void TafficManageLoop();

private:

	uint64_t posix_clock_gettime();

	TrafficNode* FindTrafficNode(int node_id);

	bool FindSegmentNodeId(int segment_id, int &start_node_id, int &end_node_id);

private:
	void AddSegmentNotAllocate(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init);

	bool CheckSegmentInSegmentNotAllocate(int agv_id, int segment_id, int wop_id);

	bool CheckSegmentNotAllocateIsAllocated(int agv_id, int segment_id, int wop_id);

	void RemoveSegmentNotAllocate(int agv_id, int segment_id, int wop_id);

private:
	bool SimpleAllocatedSegment(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init = false, bool freeze_not_usage = false);

	bool CheckSimpleAllocatedSegmentWithoutRdb(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init = false, bool freeze_not_usage = false);
	bool TryCheckSimpleAllocatedSegmentWithoutRdb(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init = false, bool freeze_not_usage = false);
	bool SimpleAllocatedSegmentWithoutRdb(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init = false, bool freeze_not_usage = false);

	bool SimpleAllocatedAndNotAllocatedSegment(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init = false, bool freeze_not_usage = false);

private:
	void ReAllocateNotAllocatedSegments();

public:
	std::vector<int> CheckDirectionChangeNode(std::vector<trail_t> complete_path);

private:
	bool GetGlobalWopAngle(const PathSearch_Wop &wop, const PathSearch_EDGEINFO &edge, const bool start_node, double &global_wop_angle);
	
	bool GetPosByT(const double t, const PathSearch_EDGEINFO &edge, position_t &pos);
	bool GetNodePos(const int node_id, position_t &pos);

	double NormalAngle(double sita);

private:
	PathSearch_LayoutData *layout_data_;

private:
	std::map<int, TrafficNode*> traffic_nodes_;

	std::map<std::pair<int, int/*edge_id, wop_id*/>, TrafficSegment*> traffic_segments_;

	std::map<int, TrafficCluster*> traffic_occupied_cluster_;
	std::map<int, TrafficCluster*> traffic_ack_cluster_;
	int max_cluster_id_;

private:
	std::mutex mutex_traffic_;

	std::thread *th_;

private:
	std::vector<SegmentNotAllocateAgv> segment_not_allocate_agv_;

private:
	std::map<int, bool> agv_allocated_success_;	// record allocate result to log failed once

private:
	bool freeze_all_;

private:
	RDBManage* rdb_manage_;

private:
	std::map<int, SegmentAllocatedFialedReason > agv_allocated_failed_reason_;
};

#endif
