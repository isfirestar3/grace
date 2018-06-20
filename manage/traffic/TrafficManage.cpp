#include "TrafficManage.h"
#include "TrafficNode.h"
#include "TrafficSegment.h"
#include "TrafficCluster.h"
#include "RouteDependentBlocking.h"
#include "math.h"
#include <fstream>
#include "log.h"
#include "os_util.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <functional>


TrafficManage::TrafficManage()
	: max_cluster_id_(1)
{
	traffic_nodes_.clear();
	traffic_segments_.clear();
	traffic_occupied_cluster_.clear();
	traffic_ack_cluster_.clear();

	freeze_all_ = false;
	rdb_manage_ = new RDBManage;
	rdb_manage_->SetTrafficManage(this);
}

TrafficManage::~TrafficManage()
{
	delete rdb_manage_;
}

bool TrafficManage::LoadMapData(PathSearch_LayoutData *layout)
{
	layout_data_ = layout;
	
	for (unsigned int i = 0; i < layout_data_->nodeList.size(); ++i) {
		PathSearch_NodeInfo node_data = layout_data_->nodeList[i];

		TrafficNode *traffic_node = new TrafficNode(node_data.node_id);
		traffic_nodes_[node_data.node_id] = traffic_node;
	}

	std::map<unsigned int/*wop_id*/, PathSearch_Wop> wop_types;
	for (unsigned int i = 0; i < layout_data_->wopList.size(); ++i) {
		PathSearch_Wop m_wop = layout_data_->wopList[i];
		wop_types[m_wop.wop_id] = m_wop;
	}

	for (unsigned int i = 0; i < layout_data_->edgeList.size(); ++i) {
		PathSearch_EdgeInfo edge_data = layout_data_->edgeList[i];

		int m_segment_id = edge_data.id;
		int m_start_id = edge_data.start_node_id;
		int m_end_id = edge_data.end_node_id;
		double m_length = edge_data.length;
		
		for (unsigned int j = 0; j < edge_data.wop_list.size(); ++j) {
			int m_wop_id = edge_data.wop_list[j];
			PathSearch_Wop m_wop = wop_types[m_wop_id];
			if (PathSearch_FORWARD == m_wop.direction) {
				if (false == this->LoadTrafficSegment(m_segment_id, m_wop_id, m_start_id, m_end_id, m_length)) {
					loerror("traffic") << "TrafficManage load segment failed, segment " << m_start_id << "->" << m_end_id << " wop:" << m_wop_id;
					return false;
				}
			}
			else if (PathSearch_BACKWARD == m_wop.direction) {
				if (false == this->LoadTrafficSegment(m_segment_id, m_wop_id, m_end_id, m_start_id, m_length)) {
					loerror("traffic") << "TrafficManage load segment failed, segment " << m_start_id << "->" << m_end_id << " wop:" << m_wop_id;
					return false;
				}
			}
		}
	}

	return true;
}

bool TrafficManage::LoadAutoTrafficFromFile(std::string file_name)
{
	std::ifstream ifs;
	ifs.open(file_name.c_str());
	if (!ifs) {
		loerror("traffic") << "TrafficManage load auto traffic blocking file " << file_name.c_str() << " failed, not exsist...";
		return false;
	}

	int m_segment_id, m_segment_wop_id, m_other_segment_id, m_other_segment_wop_id;
	while (ifs >> m_segment_id >> m_segment_wop_id >> m_other_segment_id >> m_other_segment_wop_id) {
		
		if (false == this->LoadAutoTrafficMutex(m_segment_id, m_segment_wop_id, m_other_segment_id, m_other_segment_wop_id)) {
			loerror("traffic") << "TrafficManage load auto traffic blocking file failed, segment:" << m_segment_id << " wop:" << m_segment_wop_id << ", segment:" << m_other_segment_id << " wop:" << m_other_segment_wop_id;
			//return false;
		}
	}

	return true;
}

bool TrafficManage::LoadManulTrafficFormFile(std::string file_name)
{
	std::ifstream ifs;
	ifs.open(file_name.c_str());
	if (!ifs) {
		loerror("traffic") << "TrafficManage load manul traffic blocking file " << file_name.c_str() << " failed, not exsist...";
		return false;
	}

	int line_count = 0;
	while (!ifs.eof())
	{
		line_count++;
		std::string manual_id_list;
		getline(ifs, manual_id_list);
		loinfo("traffic") << "munual line" << line_count << ":" << manual_id_list;
		if (manual_id_list == "") {
			loinfo("traffic") << "munual line" << line_count << " is empty";
			continue;
		}
		else
		{
			const char* munual_chars = manual_id_list.c_str();
			if (munual_chars[0] == '#')
			{
				loinfo("traffic") << "munual line" << line_count << " " << munual_chars[0];
				continue;
			}
		}

		std::vector<std::string> str_node_list;
		this->SplitString(manual_id_list, " ", str_node_list);
		if (str_node_list.size() != 4)
		{
			loerror("traffic") << "Load munual Config File failed at line:" << line_count;
			return false;
		}

		int m_segment_id, m_segment_wop_id, m_other_segment_id, m_other_segment_wop_id;
		m_segment_id = atoi(str_node_list[0].c_str());
		m_segment_wop_id = atoi(str_node_list[1].c_str());
		m_other_segment_id = atoi(str_node_list[2].c_str());
		m_other_segment_wop_id = atoi(str_node_list[3].c_str());

		if (false == this->LoadManualTrafficMutex(m_segment_id, m_segment_wop_id, m_other_segment_id, m_other_segment_wop_id)) {
			loerror("traffic") << "TrafficManage load manul traffic blocking file failed, segment:" << m_segment_id << " wop:" << m_segment_wop_id << ", segment:" << m_other_segment_id << " wop:" << m_other_segment_wop_id;
			//return false;
		}

	}


	return true;
}

bool TrafficManage::LoadOccupiedCluster(std::string file_name)
{
	std::ifstream cluster_ifs;
	cluster_ifs.open(file_name.c_str());
	if (!cluster_ifs) {
		loerror("traffic") << "TrafficManage load manul OccupiedCluster cfg file " << file_name.c_str() << " failed, not exsist...";
		return false;
	}
	int line_count = 1;
	int m_node_id = max_cluster_id_;
	while (!cluster_ifs.eof()) {

		std::string cluster_id_list;
		getline(cluster_ifs, cluster_id_list);
		if (cluster_id_list == "") {
			continue;
		}
		else
		{
			const char* cc_cluster = cluster_id_list.c_str();
			//std::cout << "line" << line_count++ << " " << cc_cluster[0] << std::endl;
			if (cc_cluster[0] == '#')
				continue;
		}

		std::vector<std::string> str_node_list;
		this->SplitString(cluster_id_list, " ", str_node_list);

		std::vector<int> m_node_list;
		for (unsigned int k = 0; k < str_node_list.size(); ++k) {
			m_node_list.push_back(atoi(str_node_list[k].c_str()));
		}

		TrafficCluster *occupied_cluster = new TrafficCluster(m_node_id, m_node_list);
		traffic_occupied_cluster_[m_node_id] = occupied_cluster;

		m_node_id++;
	}

	max_cluster_id_ = m_node_id;
	return true;
}

bool TrafficManage::LoadAckCluster(std::string file_name)
{
	std::ifstream cluster_ifs;
	cluster_ifs.open(file_name.c_str());
	if (!cluster_ifs) {
		return false;
	}

	int line_count = 1;
	int m_node_id = max_cluster_id_;
	while (!cluster_ifs.eof()) {

		std::string cluster_id_list;
		getline(cluster_ifs, cluster_id_list);
		if (cluster_id_list == "") {
			continue;
		}
		else
		{
			const char* cc_cluster = cluster_id_list.c_str();
			std::cout << "line" << line_count++ << " " << cc_cluster[0] << std::endl;
			if (cc_cluster[0] == '#')
				continue;
		}

		std::vector<std::string> str_node_list;
		this->SplitString(cluster_id_list, " ", str_node_list);

		std::vector<int> m_node_list;
		for (unsigned int k = 0; k < str_node_list.size(); ++k) {
			m_node_list.push_back(atoi(str_node_list[k].c_str()));
		}

		TrafficCluster *ack_cluster = new TrafficCluster(m_node_id, m_node_list);
		traffic_ack_cluster_[m_node_id] = ack_cluster;

		m_node_id++;
	}

	max_cluster_id_ = m_node_id;
	return true;
}

bool TrafficManage::LoadRdbConfig(std::string file_name)
{
	return rdb_manage_->LoadRdbConfigFile(file_name);
}

void TrafficManage::StartTrafficManage()
{
	th_ = new std::thread(std::bind(&TrafficManage::TafficManageLoop, this));
}

bool TrafficManage::CheckSegmentCanAllocated(int agv_id, int segment_id, int wop_id)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	int priority = 1;
	std::vector<trail_t> complete_path;
	std::vector<int> direction_change_node_list;
	return this->TryCheckSimpleAllocatedSegmentWithoutRdb(agv_id, segment_id, wop_id, 1, complete_path, direction_change_node_list, false, false);
}

bool TrafficManage::AllocateSegment(int agv_id, int segment_id, int wop_id, bool is_init)
{
	int priority = 1;
	std::vector<trail_t> complete_path;
	std::vector<int> direction_change_node_list;
	return this->AllocateSegment(agv_id, segment_id, wop_id, priority, complete_path, direction_change_node_list, is_init);
}

bool TrafficManage::AllocateSegment(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);

	return this->SimpleAllocatedAndNotAllocatedSegment(agv_id, segment_id, wop_id, priority, complete_path, direction_change_node_list, is_init);
}

bool TrafficManage::ReleaseSegment(int agv_id, int segment_id, int wop_id)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	TrafficSegment *traffic_segment = this->FindTrafficSegment(segment_id, wop_id);
	if (traffic_segment == NULL) {
		loinfo("traffic") << "TrafficManage Agv " << agv_id << " release segment failed, segment:" << segment_id << " wop:" << wop_id << " not exsist...";
		return false;
	}

	traffic_segment->ReleaseSegment(agv_id);
	loinfo("traffic") << "TrafficManage Agv " << agv_id << " release segment success, segment:" << segment_id << " wop:" << wop_id;

	for (std::map<int, TrafficCluster*>::iterator iter = traffic_occupied_cluster_.begin(); iter != traffic_occupied_cluster_.end(); ++iter) {
		TrafficCluster *m_occupied_cluster = iter->second;
		if (true == m_occupied_cluster->CheckSegmentOccupiedInCluster(traffic_segment->GetStartId(), traffic_segment->GetEndId())) {

			m_occupied_cluster->RemoveSegment(traffic_segment->GetStartId(), traffic_segment->GetEndId());

			if (true == m_occupied_cluster->CheckSegmentEmpty()) {
				m_occupied_cluster->ReleaseCluster(agv_id);
				loinfo("traffic") << "TrafficManage Agv " << agv_id << " release occupied cluster success, segment:" << segment_id << " wop:" << wop_id;
			}
		}
	}


	for (std::map<int, TrafficCluster*>::iterator iter = traffic_ack_cluster_.begin(); iter != traffic_ack_cluster_.end(); ++iter) {
		TrafficCluster *m_ack_cluster = iter->second;
		if (true == m_ack_cluster->CheckSegmentInCluster(traffic_segment->GetStartId(), traffic_segment->GetEndId()) && agv_id == m_ack_cluster->GetAllocateAgv()) {

			m_ack_cluster->RemoveSegment(traffic_segment->GetStartId(), traffic_segment->GetEndId());

			if (true == m_ack_cluster->CheckSegmentEmpty()) {
				m_ack_cluster->ReleaseCluster(agv_id);
				loinfo("traffic") << "TrafficManage Agv " << agv_id << " release acknowledge cluster success, segment:" << segment_id << " wop:" << wop_id;
			}
		}
	}

	return true;
}

bool TrafficManage::ReleaseNode(int agv_id, int node_id)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	TrafficNode *traffic_node = this->FindTrafficNode(node_id);
	if (traffic_node == NULL) {
		loinfo("traffic") << "TrafficManage Agv " << agv_id << " release node success, node " << node_id << " is not exsist...";
		return false;
	}

	return traffic_node->ReleaseNode(agv_id);
}

bool TrafficManage::ReleaseSegmentAndNode(int agv_id)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	for (std::map<int, TrafficNode*>::iterator iter = traffic_nodes_.begin(); iter != traffic_nodes_.end(); ++iter) {
		iter->second->ReleaseNode(agv_id);
	}

	for (std::map<std::pair<int, int>, TrafficSegment*>::iterator iter = traffic_segments_.begin();
		iter != traffic_segments_.end(); ++iter) {
		iter->second->ReleaseSegment(agv_id);
	}

	for (std::map<int, TrafficCluster*>::iterator iter = traffic_occupied_cluster_.begin(); iter != traffic_occupied_cluster_.end(); ++iter) {
		iter->second->ReleaseCluster(agv_id);
	}

	for (std::map<int, TrafficCluster*>::iterator iter = traffic_ack_cluster_.begin(); iter != traffic_ack_cluster_.end(); ++iter) {
		iter->second->ReleaseCluster(agv_id);
	}

	return true;
}

bool TrafficManage::ReleaseSegmentAndNodeButNotCurrentUpl(int agv_id, int segment_id, int wop_id, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	for (std::map<int, TrafficNode*>::iterator iter = traffic_nodes_.begin(); iter != traffic_nodes_.end(); ++iter) {
		iter->second->ReleaseNode(agv_id);
	}

	for (std::map<std::pair<int, int>, TrafficSegment*>::iterator iter = traffic_segments_.begin();
		iter != traffic_segments_.end(); ++iter) {
		iter->second->ReleaseSegment(agv_id);
	}

	for (std::map<int, TrafficCluster*>::iterator iter = traffic_occupied_cluster_.begin(); iter != traffic_occupied_cluster_.end(); ++iter) {
		iter->second->ReleaseCluster(agv_id);
	}

	for (std::map<int, TrafficCluster*>::iterator iter = traffic_ack_cluster_.begin(); iter != traffic_ack_cluster_.end(); ++iter) {
		iter->second->ReleaseCluster(agv_id);
	}

	return this->SimpleAllocatedAndNotAllocatedSegment(agv_id, segment_id, wop_id, 1, complete_path, direction_change_node_list, is_init, true);
}

void TrafficManage::ClearNotAllocateSegment(int agv_id)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	loinfo("traffic") << "TrafficManage agv " << agv_id << " clear not allocate segment";
	for (std::vector<SegmentNotAllocateAgv>::iterator iter = segment_not_allocate_agv_.begin();
		iter != segment_not_allocate_agv_.end(); ++iter) {
		if (agv_id == iter->agv_id_) {
			loinfo("traffic") << "TrafficManage agv " << agv_id << " clear not allocate segment:" << iter->segment_id_ << " wop:" << iter->wop_id_;
			segment_not_allocate_agv_.erase(iter);
			return;
		}
	}
}

bool TrafficManage::CheckAgvBlockedReason(int agv_id, SegmentAllocatedFialedReason &failed_reason)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	std::map<int, SegmentAllocatedFialedReason>::iterator iter_find = agv_allocated_failed_reason_.find(agv_id);
	if (iter_find == agv_allocated_failed_reason_.end()) {
		return false;
	}
	failed_reason = iter_find->second;
	return true;
}

bool TrafficManage::CheckSegmentWopAllocatedAgv(int segment_id, int wop_id, int& agv_id)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	TrafficSegment *traffic_segment = this->FindTrafficSegment(segment_id, wop_id);
	if (traffic_segment == NULL) {
		agv_id = -1;
		return false;
	}

	agv_id = traffic_segment->GetAllocateAgv();
	if (agv_id == -1) {
		agv_id = -1;
		return false;
	}
	
	return true;
}

bool TrafficManage::CheckOcupiedClusterAllocatedAgv(int cluster_id, int& agv_id)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	TrafficCluster *traffc_cluster = this->FindTrafficOcupiedCluster(cluster_id);
	if (traffc_cluster == NULL) {
		agv_id = -1;
		return false;
	}

	agv_id = traffc_cluster->GetAllocateAgv();
	if (agv_id == -1) {
		agv_id = -1;
		return false;
	}

	return true;
}

bool TrafficManage::CheckAckClusterAllocatedAgv(int cluster_id, int& agv_id)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	TrafficCluster *traffc_cluster = this->FindTrafficAckCluster(cluster_id);
	if (traffc_cluster == NULL) {
		agv_id = -1;
		return false;
	}

	agv_id = traffc_cluster->GetAllocateAgv();
	if (agv_id == -1) {
		agv_id = -1;
		return false;
	}

	return true;
}

void TrafficManage::FreezeAll(bool freeze_all)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	freeze_all_ = freeze_all;
}

bool TrafficManage::GetFreezeAll()
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	return freeze_all_;
}

PathSearch_LayoutData* TrafficManage::GetLayoutData()
{
	return layout_data_;
}

std::vector<std::pair<int, int> > TrafficManage::GetNodeAllocateAgv()
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	std::vector<std::pair<int, int> > node_allocate_agv;
	std::map<int, TrafficNode*>::iterator iter = traffic_nodes_.begin();
	for (; iter != traffic_nodes_.end(); ++iter) {
		node_allocate_agv.push_back(std::pair<int, int>(iter->first, iter->second->GetAllocateAgv()));
	}

	return node_allocate_agv;
}

std::vector<std::pair<std::pair<int, int>, int> > TrafficManage::GetSegmentAllocateAgv()
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	std::vector<std::pair<std::pair<int, int>, int> > segment_allocate_agv;
	std::map<std::pair<int, int>, TrafficSegment*>::iterator iter = traffic_segments_.begin();
	for (; iter != traffic_segments_.end(); ++iter) {
		int m_allocated_agv = iter->second->GetAllocateAgv();
		segment_allocate_agv.push_back(std::pair<std::pair<int, int>, int>(iter->first, m_allocated_agv));

	}

	return segment_allocate_agv;
}

std::map<int, std::vector<std::pair<int, int> > > TrafficManage::GetAgvAllocateSegment()
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	std::map<int, std::vector<std::pair<int, int> > > agv_allocate_segment;
	std::map<std::pair<int, int>, TrafficSegment*>::iterator iter = traffic_segments_.begin();
	for (; iter != traffic_segments_.end(); ++iter) {
		int m_allocated_agv = iter->second->GetAllocateAgv();
		if (-1 != m_allocated_agv) {
			agv_allocate_segment[m_allocated_agv].push_back(iter->first);
		}
	}

	return agv_allocate_segment;
}

int TrafficManage::GetAgvAllocateSegmentCount(int agv_id)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	int agv_allocate_segment_count = 0;
	std::map<std::pair<int, int>, TrafficSegment*>::iterator iter = traffic_segments_.begin();
	for (; iter != traffic_segments_.end(); ++iter) {
		int m_allocated_agv = iter->second->GetAllocateAgv();
		if (agv_id == m_allocated_agv) {
			++agv_allocate_segment_count;
		}
	}

	return agv_allocate_segment_count;
}

bool TrafficManage::LoadTrafficSegment(int segment_id, int wop_id, int start_id, int end_id, double length)
{
	TrafficSegment *traffic_segment = new TrafficSegment(segment_id, wop_id, start_id, end_id, length);
	traffic_segments_[std::pair<int, int>(segment_id, wop_id)] = traffic_segment;

	TrafficNode* start_traffic_node = this->FindTrafficNode(start_id);
	if (start_traffic_node == NULL) {
		loerror("traffic") << "TrafficManage node " << start_id << " is not exsist...";
		return false;
	}

	TrafficNode* end_traffic_node = this->FindTrafficNode(end_id);
	if (end_traffic_node == NULL) {
		loerror("traffic") << "TrafficManage node " << end_id << " is not exsist...";
		return false;
	}

	traffic_segment->SetTrafficStartNode(start_traffic_node);
	traffic_segment->SetTrafficEndNode(end_traffic_node);
	start_traffic_node->AddOuterSegment(traffic_segment);
	end_traffic_node->AddEnterSegment(traffic_segment);

	return true;
}

bool TrafficManage::LoadAutoTrafficMutex(int segment_id, int wop_id, int mutex_segment_id, int mutex_wop_id)
{
	TrafficSegment* m_segment = this->FindTrafficSegment(segment_id, wop_id);
	if (NULL == m_segment) {
		return false;
	}

	TrafficSegment* m_mutex_segment = this->FindTrafficSegment(mutex_segment_id, mutex_wop_id);
	if (NULL == m_mutex_segment) {
		return false;
	}

	m_segment->AddAutoMutexSegment(m_mutex_segment);

	return true;
}

bool TrafficManage::LoadManualTrafficMutex(int segment_id, int wop_id, int mutex_segment_id, int mutex_wop_id)
{
	TrafficSegment* m_segment = this->FindTrafficSegment(segment_id, wop_id);
	if (NULL == m_segment) {
		return false;
	}

	TrafficSegment* m_mutex_segment = this->FindTrafficSegment(mutex_segment_id, mutex_wop_id);
	if (NULL == m_mutex_segment) {
		return false;
	}

	m_segment->AddManualMutexSegment(m_mutex_segment);

	return true;
}

TrafficSegment* TrafficManage::FindTrafficSegment(int segment_id, int wop_id)
{
	TrafficSegment* traffic_segment = NULL;
	std::map<std::pair<int, int>, TrafficSegment*>::iterator iter = traffic_segments_.find(std::pair<int, int>(segment_id, wop_id));
	if (iter != traffic_segments_.end()) {
		traffic_segment = iter->second;
	}

	return traffic_segment;
}

TrafficCluster* TrafficManage::FindTrafficOcupiedCluster(int cluster_id)
{
	TrafficCluster* traffic_cluster = NULL;
	std::map<int, TrafficCluster*>::iterator iter = traffic_occupied_cluster_.find(cluster_id);
	if (iter != traffic_occupied_cluster_.end()) {
		traffic_cluster = iter->second;
	}
	return traffic_cluster;
}

TrafficCluster* TrafficManage::FindTrafficAckCluster(int cluster_id)
{
	TrafficCluster* traffic_cluster = NULL;
	std::map<int, TrafficCluster*>::iterator iter = traffic_ack_cluster_.find(cluster_id);
	if (iter != traffic_ack_cluster_.end()) {
		traffic_cluster = iter->second;
	}
	return traffic_cluster;
}

uint64_t TrafficManage::posix_clock_gettime()
{
#if _WIN32
	LARGE_INTEGER counter;
	static LARGE_INTEGER frequency = { 0 };
	if (0 == frequency.QuadPart) {
		if (!QueryPerformanceFrequency(&frequency)) {
			return 0;
		}
	}

	if (QueryPerformanceCounter(&counter)) {
		return (uint64_t)((uint64_t)((uint64_t)1000 * 1000 * 10) * ((double)counter.QuadPart / frequency.QuadPart));
	}
	return 0;
#else
	/* gcc -lrt */
	struct timespec tsc;
	if (clock_gettime(CLOCK_MONOTONIC, &tsc) >= 0) { /* CLOCK_REALTIME */
		return (uint64_t)tsc.tv_sec * 10000000 + tsc.tv_nsec / 100; /* 返回 100ns, 兼容windows的KRNL计时 */
	}
	return 0;
#endif
}

int TrafficManage::SplitString(const std::string& input, const std::string& delimiter, std::vector<std::string>& results)
{
	int iPos = 0;
	int newPos = -1;
	int sizeS2 = (int)delimiter.size();
	int isize = (int)input.size();
	results.clear();

	if ((isize == 0) || (sizeS2 == 0)) {
		return 0;
	}

	std::vector<int> positions;

	int numFound = 0;

	while ((newPos = input.find(delimiter, iPos)) > 0) {
		positions.push_back(newPos);
		iPos = newPos + sizeS2;
		numFound++;
	}

	if (numFound == 0) {
		if (input.size() > 0) {
			results.push_back(input);
		}
		return 1;
	}

	if (positions.back() != isize) {
		positions.push_back(isize);
	}

	int offset = 0;
	std::string s("");

	for (int i = 0; i < (int)positions.size(); ++i) {

		s = input.substr(offset, positions[i] - offset);

		offset = positions[i] + sizeS2;

		if (s.length() > 0 ) {
			results.push_back(s);
		}

	}
	return numFound;
}

void TrafficManage::TafficManageLoop()
{
	nsp::os::waitable_handle time_wait;
	while (true) {
		time_wait.wait(500);

		this->ReAllocateNotAllocatedSegments();
	}
}

TrafficNode* TrafficManage::FindTrafficNode(int node_id)
{
	TrafficNode* traffic_node = NULL;
	std::map<int, TrafficNode*>::iterator iter = traffic_nodes_.find(node_id);
	if (iter != traffic_nodes_.end()) {
		traffic_node = iter->second;
	}

	return traffic_node;
}

bool TrafficManage::FindSegmentNodeId(int segment_id, int &start_node_id, int &end_node_id)
{
	for (unsigned int i = 0; i < layout_data_->edgeList.size(); ++i) {
		PathSearch_EDGEINFO &m_segment = layout_data_->edgeList[i];
		if (m_segment.id == segment_id) {

			start_node_id = m_segment.start_node_id;
			end_node_id = m_segment.end_node_id;

			return true;
		}
	}

	return false;
}

void TrafficManage::AddSegmentNotAllocate(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init)
{
	if (false == this->CheckSegmentInSegmentNotAllocate(agv_id, segment_id, wop_id)) {
		SegmentNotAllocateAgv m_segment_not_allocate_agv;
		m_segment_not_allocate_agv.segment_id_ = segment_id;
		m_segment_not_allocate_agv.wop_id_ = wop_id;
		m_segment_not_allocate_agv.complete_path_ = complete_path;
		m_segment_not_allocate_agv.direction_change_node_list_ = direction_change_node_list;
		m_segment_not_allocate_agv.is_init_ = is_init;
		m_segment_not_allocate_agv.agv_id_ = agv_id;
		m_segment_not_allocate_agv.priority_ = priority;
		m_segment_not_allocate_agv.allocate_time_ = (int)this->posix_clock_gettime();
		m_segment_not_allocate_agv.is_allocated_ = false;
		segment_not_allocate_agv_.push_back(m_segment_not_allocate_agv);
	}
}

bool TrafficManage::CheckSegmentInSegmentNotAllocate(int agv_id, int segment_id, int wop_id)
{
	for (std::vector<SegmentNotAllocateAgv>::iterator iter = segment_not_allocate_agv_.begin();
		iter != segment_not_allocate_agv_.end(); ++iter) {
		SegmentNotAllocateAgv m_segment_not_allocate_agv = *iter;
		if (m_segment_not_allocate_agv.agv_id_ == agv_id
			&& m_segment_not_allocate_agv.segment_id_ == segment_id
			&& m_segment_not_allocate_agv.wop_id_ == wop_id) {

			return true;
		}
	}
	return false;
}

bool TrafficManage::CheckSegmentNotAllocateIsAllocated(int agv_id, int segment_id, int wop_id)
{
	for (std::vector<SegmentNotAllocateAgv>::iterator iter = segment_not_allocate_agv_.begin();
		iter != segment_not_allocate_agv_.end(); ++iter) {
		SegmentNotAllocateAgv m_segment_not_allocate_agv = *iter;
		if (m_segment_not_allocate_agv.agv_id_ == agv_id
			&& m_segment_not_allocate_agv.segment_id_ == segment_id
			&& m_segment_not_allocate_agv.wop_id_ == wop_id) {

			if (true == m_segment_not_allocate_agv.is_allocated_) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	return false;
}

void TrafficManage::RemoveSegmentNotAllocate(int agv_id, int segment_id, int wop_id)
{
	for (std::vector<SegmentNotAllocateAgv>::iterator iter = segment_not_allocate_agv_.begin();
		iter != segment_not_allocate_agv_.end(); ++iter) {
		SegmentNotAllocateAgv m_segment_not_allocate_agv = *iter;
		if (m_segment_not_allocate_agv.agv_id_ == agv_id
			&& m_segment_not_allocate_agv.segment_id_ == segment_id
			&& m_segment_not_allocate_agv.wop_id_ == wop_id) {

			segment_not_allocate_agv_.erase(iter);
			break;
		}
	}
}
bool TrafficManage::SimpleAllocatedSegment(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init, bool freeze_not_usage)
{
	// check segment fit rdb
	std::vector<trail_t> segment_group;
	bool segment_fit_rdb = false;

	if (true == rdb_manage_->CheckRdbConditionActive(segment_id, wop_id, complete_path, segment_group)) {

		segment_fit_rdb = true;
	}

	// if first segment don't use rdb
	if (complete_path.size() > 0) {
		if (segment_id == complete_path[0].edge_id_ && wop_id == complete_path[0].wop_id_) {
			segment_fit_rdb = false;
		}
	}

	if (true == segment_fit_rdb) {

		bool segment_group_can_allocated = true;

		int allocated_failed_segment_id = -1;
		int allocated_failed_wop_id = -1;

		for (unsigned int i = 0; i < segment_group.size(); ++i) {

			int m_segment_id = segment_group[i].edge_id_;
			int m_wop_id = segment_group[i].wop_id_;

			if (false == this->CheckSimpleAllocatedSegmentWithoutRdb(agv_id, m_segment_id, m_wop_id, priority, complete_path, direction_change_node_list, is_init, freeze_not_usage)) {

				segment_group_can_allocated = false;

				allocated_failed_segment_id = m_segment_id;
				allocated_failed_wop_id = m_wop_id;
				break;
			}

		}

		if (true == segment_group_can_allocated) {

			for (unsigned int i = 0; i < segment_group.size(); ++i) {

				int m_segment_id = segment_group[i].edge_id_;
				int m_wop_id = segment_group[i].wop_id_;
				
				this->SimpleAllocatedSegmentWithoutRdb(agv_id, m_segment_id, m_wop_id, priority, complete_path, direction_change_node_list, is_init);
				lotrace("traffic") << "TrafficManage agv " << agv_id << " allocated segment(group) success, segment:" << segment_id << " wop:" << wop_id;
			}

			return true;
		}
		else {

			this->AddSegmentNotAllocate(agv_id, segment_id, wop_id, priority, complete_path, direction_change_node_list, is_init);

			std::string rdb_group = "";
			for (unsigned int i = 0; i < segment_group.size(); ++i) {
				rdb_group += "segment:";
				rdb_group += std::to_string(segment_group[i].edge_id_);
				rdb_group += ", wop:";
				rdb_group += std::to_string(segment_group[i].wop_id_);
				rdb_group += " ";
			}

			lotrace("traffic") << "TrafficManage agv " << agv_id << " allocated acknowledge segment:" << segment_id << " wop:" << wop_id
				<< " failed, " << ", RDB check:" << segment_group_can_allocated << " Group:" << rdb_group
				<< " allocated_failed_segment_id: " << allocated_failed_segment_id
				<< " allocated_failed_wop_id: " << allocated_failed_wop_id;

			return false;
		}
	}
	else {

		if (true == this->CheckSimpleAllocatedSegmentWithoutRdb(agv_id, segment_id, wop_id, priority, complete_path, direction_change_node_list, is_init, freeze_not_usage)) {

			this->SimpleAllocatedSegmentWithoutRdb(agv_id, segment_id, wop_id, priority, complete_path, direction_change_node_list, is_init, freeze_not_usage);
			return true;
		}
		else {
			this->AddSegmentNotAllocate(agv_id, segment_id, wop_id, priority, complete_path, direction_change_node_list, is_init);
			return false;
		}
	}

	return false;
}

bool TrafficManage::CheckSimpleAllocatedSegmentWithoutRdb(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init, bool freeze_not_usage)
{
	TrafficSegment *traffic_segment = this->FindTrafficSegment(segment_id, wop_id);
	if (traffic_segment == NULL) {
		loinfo("traffic") << "TrafficManage agv " << agv_id << " allocate segment failed, segment:" << segment_id << " " << wop_id << " is not exsist...";
		return false;
	}

	// check if freeze
	if (true == freeze_all_ && false == freeze_not_usage) {
		loinfo("traffic") << "TrafficManage agv " << agv_id << " allocate segment failed, segment:" << segment_id << " " << wop_id << " is freeze all...";
		return false;
	}

	// check segment can allcoated
	bool segment_can_allocated = false;
	int allocated_auto_mutex_segment = -1;
	int allocated_auto_mutex_wop = -1;
	int allocated_manual_mutex_segment = -1;
	int allocated_manual_mutex_wop = -1;
	if (true == traffic_segment->CheckCanAllocated(agv_id, allocated_auto_mutex_segment, allocated_auto_mutex_wop, allocated_manual_mutex_segment, allocated_manual_mutex_wop)) {
		segment_can_allocated = true;
	}
	else {
		segment_can_allocated = false;
	}

	// check occupied cluster can allocated
	bool occupied_cluster_can_allocated = true;
	int occupied_cluster_id = -1;
	for (std::map<int, TrafficCluster*>::iterator iter = traffic_occupied_cluster_.begin(); iter != traffic_occupied_cluster_.end(); ++iter) {
		TrafficCluster *m_occupied_cluster = iter->second;
		if (true == m_occupied_cluster->CheckSegmentInCluster(traffic_segment->GetStartId(), traffic_segment->GetEndId())) {
			if (true == m_occupied_cluster->IsAllocated(agv_id)) {
				occupied_cluster_can_allocated = false;
				occupied_cluster_id = iter->first;
				break;
			}
		}
	}

	// check acknowledge cluster can allocated
	bool ack_cluster_can_allocated = true;
	int current_start_id = -1;
	int current_end_id = -1;
	int target_start_id = -1;
	int target_end_id = -1;

	int ack_cluster_id = -1;
	int direction_change_node_id = -1;
	if (complete_path.size() > 0) {
		if (false == this->FindSegmentNodeId(complete_path[0].edge_id_, current_start_id, current_end_id)) {
			return false;
		}

		if (false == this->FindSegmentNodeId(complete_path[complete_path.size() - 1].edge_id_, target_start_id, target_end_id)) {
			return false;
		}

		for (std::map<int, TrafficCluster*>::iterator iter = traffic_ack_cluster_.begin(); iter != traffic_ack_cluster_.end(); ++iter) {
			TrafficCluster *m_ack_cluster = iter->second;
			if (true == m_ack_cluster->CheckSegmentInCluster(traffic_segment->GetStartId(), traffic_segment->GetEndId())) {

				if (true == m_ack_cluster->CheckSegmentInCluster(current_start_id, current_end_id)
					|| true == m_ack_cluster->CheckSegmentInCluster(target_start_id, target_end_id)
					|| true == m_ack_cluster->CheckNodesInCluster(direction_change_node_list, direction_change_node_id)) {

					if (true == m_ack_cluster->IsAllocated(agv_id)) {
						ack_cluster_can_allocated = false;
						ack_cluster_id = iter->first;
						break;
					}
				}

			}
		}
	}

	if (true == segment_can_allocated && true == occupied_cluster_can_allocated && true == ack_cluster_can_allocated) {

		agv_allocated_success_[agv_id] = true;

		SegmentAllocatedFialedReason m_failed_reason;
		agv_allocated_failed_reason_[agv_id] = m_failed_reason;

		return true;
	}
	else {

		{
			SegmentAllocatedFialedReason m_failed_reason;
			if (false == segment_can_allocated) {

				m_failed_reason.failed_reason_ = SegmentAllocatedFialedReason::SegmentFailed;
				if (allocated_auto_mutex_segment != -1 && allocated_auto_mutex_wop != -1) {

					m_failed_reason.segment_id_ = allocated_auto_mutex_segment;
					m_failed_reason.wop_id_ = allocated_auto_mutex_wop;
				}

				if (allocated_manual_mutex_segment != -1 && allocated_manual_mutex_wop != -1) {

					m_failed_reason.segment_id_ = allocated_manual_mutex_segment;
					m_failed_reason.wop_id_ = allocated_manual_mutex_wop;
				}
			}
			else if (false == occupied_cluster_can_allocated) {

				m_failed_reason.failed_reason_ = SegmentAllocatedFialedReason::OcupiedClusteFailedr;
				m_failed_reason.ocupied_cluster_id_ = occupied_cluster_id;
			}
			else if (false == ack_cluster_can_allocated) {

				m_failed_reason.failed_reason_ = SegmentAllocatedFialedReason::AckClusterFailed;
				m_failed_reason.ack_cluster_id_ = ack_cluster_id;
			}
			else {

				m_failed_reason.failed_reason_ = SegmentAllocatedFialedReason::NoFailedReason;
				m_failed_reason.segment_id_ = -1;
				m_failed_reason.wop_id_ = -1;
				m_failed_reason.ocupied_cluster_id_ = -1;
				m_failed_reason.ack_cluster_id_ = -1;
			}

			agv_allocated_failed_reason_[agv_id] = m_failed_reason;
		}

		bool first_allocated_failed = false;
		std::map<int, bool>::iterator iter_find = agv_allocated_success_.find(agv_id);
		if (iter_find != agv_allocated_success_.end()) {
			if (iter_find->second == true) {
				first_allocated_failed = true;
			}
		}
		else {
			first_allocated_failed = true;
		}

		if (true == first_allocated_failed) {

			agv_allocated_success_[agv_id] = false;

			lotrace("traffic") << "TrafficManage agv " << agv_id << " check allocated acknowledge segment:" << segment_id << " wop:" << wop_id
				<< " failed, Segment check:" << segment_can_allocated
				<< " auto_mutex:" << allocated_auto_mutex_segment << " wop:" << allocated_auto_mutex_wop
				<< " manual_mutex:" << allocated_manual_mutex_segment << " wop:" << allocated_manual_mutex_wop

				<< ", Occupied cluster:" << occupied_cluster_id << " check:" << occupied_cluster_can_allocated

				<< ", Acknowledge cluster:" << ack_cluster_id << " check:" << ack_cluster_can_allocated << " direction_change_node_id:" << direction_change_node_id;
		}
		
		return false;
	}
}

bool TrafficManage::TryCheckSimpleAllocatedSegmentWithoutRdb(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init, bool freeze_not_usage)
{
	TrafficSegment *traffic_segment = this->FindTrafficSegment(segment_id, wop_id);
	if (traffic_segment == NULL) {
		loinfo("traffic") << "TrafficManage agv " << agv_id << " allocate segment failed, segment:" << segment_id << " " << wop_id << " is not exsist...";
		return false;
	}

	// check if freeze
	if (true == freeze_all_ && false == freeze_not_usage) {
		loinfo("traffic") << "TrafficManage agv " << agv_id << " allocate segment failed, segment:" << segment_id << " " << wop_id << " is freeze all...";
		return false;
	}

	// check segment can allcoated
	bool segment_can_allocated = false;
	int allocated_auto_mutex_segment = -1;
	int allocated_auto_mutex_wop = -1;
	int allocated_manual_mutex_segment = -1;
	int allocated_manual_mutex_wop = -1;
	if (true == traffic_segment->CheckCanAllocated(agv_id, allocated_auto_mutex_segment, allocated_auto_mutex_wop, allocated_manual_mutex_segment, allocated_manual_mutex_wop)) {
		segment_can_allocated = true;
	}
	else {
		segment_can_allocated = false;
	}

	// check occupied cluster can allocated
	bool occupied_cluster_can_allocated = true;
	int occupied_cluster_id = -1;
	for (std::map<int, TrafficCluster*>::iterator iter = traffic_occupied_cluster_.begin(); iter != traffic_occupied_cluster_.end(); ++iter) {
		TrafficCluster *m_occupied_cluster = iter->second;
		if (true == m_occupied_cluster->CheckSegmentInCluster(traffic_segment->GetStartId(), traffic_segment->GetEndId())) {
			if (true == m_occupied_cluster->IsAllocated(agv_id)) {
				occupied_cluster_can_allocated = false;
				occupied_cluster_id = iter->first;
				break;
			}
		}
	}

	// check acknowledge cluster can allocated
	bool ack_cluster_can_allocated = true;
	int current_start_id = -1;
	int current_end_id = -1;
	int target_start_id = -1;
	int target_end_id = -1;

	int ack_cluster_id = -1;
	int direction_change_node_id = -1;
	if (complete_path.size() > 0) {
		if (false == this->FindSegmentNodeId(complete_path[0].edge_id_, current_start_id, current_end_id)) {
			return false;
		}

		if (false == this->FindSegmentNodeId(complete_path[complete_path.size() - 1].edge_id_, target_start_id, target_end_id)) {
			return false;
		}

		for (std::map<int, TrafficCluster*>::iterator iter = traffic_ack_cluster_.begin(); iter != traffic_ack_cluster_.end(); ++iter) {
			TrafficCluster *m_ack_cluster = iter->second;
			if (true == m_ack_cluster->CheckSegmentInCluster(traffic_segment->GetStartId(), traffic_segment->GetEndId())) {

				if (true == m_ack_cluster->CheckSegmentInCluster(current_start_id, current_end_id)
					|| true == m_ack_cluster->CheckSegmentInCluster(target_start_id, target_end_id)
					|| true == m_ack_cluster->CheckNodesInCluster(direction_change_node_list, direction_change_node_id)) {

					if (true == m_ack_cluster->IsAllocated(agv_id)) {
						ack_cluster_can_allocated = false;
						ack_cluster_id = iter->first;
						break;
					}
				}

			}
		}
	}

	if (true == segment_can_allocated && true == occupied_cluster_can_allocated && true == ack_cluster_can_allocated) {

		return true;
	}
	else {

		return false;
	}
}

bool TrafficManage::SimpleAllocatedSegmentWithoutRdb(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init, bool freeze_not_usage)
{
	if (true == this->CheckSimpleAllocatedSegmentWithoutRdb(agv_id, segment_id, wop_id, priority, complete_path, direction_change_node_list, is_init, freeze_not_usage)) {

		TrafficSegment *traffic_segment = this->FindTrafficSegment(segment_id, wop_id);
		if (traffic_segment == NULL) {
			lotrace("traffic") << "TrafficManage agv " << agv_id << " check allocate segment failed, segment:" << segment_id << " " << wop_id << " is not exsist...";
			return false;
		}

		int current_start_id = -1;
		int current_end_id = -1;
		int target_start_id = -1;
		int target_end_id = -1;

		int direction_change_node_id = -1;

		if (complete_path.size() > 0) {
			if (false == this->FindSegmentNodeId(complete_path[0].edge_id_, current_start_id, current_end_id)) {
				return false;
			}

			if (false == this->FindSegmentNodeId(complete_path[complete_path.size() - 1].edge_id_, target_start_id, target_end_id)) {
				return false;
			}
		}

		traffic_segment->AllocateSegment(agv_id, is_init);
		lotrace("traffic") << "TrafficManage agv " << agv_id << " allocated segment success, segment:" << segment_id << " wop:" << wop_id << " target segment:" << target_start_id << "->" << target_end_id;

		for (std::map<int, TrafficCluster*>::iterator iter = traffic_occupied_cluster_.begin(); iter != traffic_occupied_cluster_.end(); ++iter) {
			TrafficCluster *m_occupied_cluster = iter->second;
			if (true == m_occupied_cluster->CheckSegmentInCluster(traffic_segment->GetStartId(), traffic_segment->GetEndId())) {

				m_occupied_cluster->AllocateCluster(agv_id);
				m_occupied_cluster->AddSegment(traffic_segment->GetStartId(), traffic_segment->GetEndId());

				lotrace("traffic") << "TrafficManage agv " << agv_id << " allocated occupied cluster:" << iter->first << " success, segment:" << segment_id << " wop:" << wop_id << " target segment:" << target_start_id << "->" << target_end_id;
			}
		}

		for (std::map<int, TrafficCluster*>::iterator iter = traffic_ack_cluster_.begin(); iter != traffic_ack_cluster_.end(); ++iter) {
			TrafficCluster *m_ack_cluster = iter->second;
			if (true == m_ack_cluster->CheckSegmentInCluster(traffic_segment->GetStartId(), traffic_segment->GetEndId())) {

				if (true == m_ack_cluster->CheckSegmentInCluster(current_start_id, current_end_id)
					|| true == m_ack_cluster->CheckSegmentInCluster(target_start_id, target_end_id)
					|| true == m_ack_cluster->CheckNodesInCluster(direction_change_node_list, direction_change_node_id)) {

					m_ack_cluster->AllocateCluster(agv_id);
					m_ack_cluster->AddSegment(traffic_segment->GetStartId(), traffic_segment->GetEndId());

					lotrace("traffic") << "TrafficManage agv " << agv_id << " allocated acknowledge cluster:" << iter->first << " success, segment:" << segment_id << " wop:" << wop_id << " target segment:" << target_start_id << "->" << target_end_id;
				}
			}
		}

		return true;
	}
	else {
		
		return false;
	}

	return false;
}

bool TrafficManage::SimpleAllocatedAndNotAllocatedSegment(int agv_id, int segment_id, int wop_id, int priority, std::vector<trail_t> complete_path, std::vector<int> direction_change_node_list, bool is_init, bool freeze_not_usage)
{
	if (true == this->CheckSegmentInSegmentNotAllocate(agv_id, segment_id, wop_id)) {
		if (true == this->CheckSegmentNotAllocateIsAllocated(agv_id, segment_id, wop_id)) {

			this->RemoveSegmentNotAllocate(agv_id, segment_id, wop_id);

			lotrace("traffic") << "TrafficManage agv " << agv_id << " reallocated segment success, segment:" << segment_id << " wop:" << wop_id;
			return true;
		}
		else {
			return false;
		}
	}
	return this->SimpleAllocatedSegment(agv_id, segment_id, wop_id, priority, complete_path, direction_change_node_list, is_init, freeze_not_usage);
}

bool SegmentNotAllocateSortByPriorityAndAllcoateTime(const SegmentNotAllocateAgv& object, const SegmentNotAllocateAgv& object2)
{
	if (object.priority_ > object2.priority_) {
		return true;
	}
	else if (object.priority_ == object2.priority_) {
		if (object.allocate_time_ < object2.allocate_time_) {
			return true;
		}
	}
	return false;
}

void TrafficManage::ReAllocateNotAllocatedSegments()
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);
	std::vector<SegmentNotAllocateAgv> m_segment_not_allocate_agv_list;
	m_segment_not_allocate_agv_list = segment_not_allocate_agv_;

	if (m_segment_not_allocate_agv_list.size() == 0) {
		return;
	}

	std::sort(m_segment_not_allocate_agv_list.begin(), m_segment_not_allocate_agv_list.end(), SegmentNotAllocateSortByPriorityAndAllcoateTime);

	std::vector<SegmentNotAllocateAgv> m_segment_reallocated_list;

	for (unsigned int i = 0; i < m_segment_not_allocate_agv_list.size(); ++i) {
		SegmentNotAllocateAgv m_segment_not_allocate_agv = m_segment_not_allocate_agv_list[i];

		if (m_segment_not_allocate_agv.is_allocated_ == true) {
			continue;
		}

		if (true == this->SimpleAllocatedSegment(m_segment_not_allocate_agv.agv_id_, m_segment_not_allocate_agv.segment_id_,
			m_segment_not_allocate_agv.wop_id_, m_segment_not_allocate_agv.priority_, m_segment_not_allocate_agv.complete_path_, m_segment_not_allocate_agv.direction_change_node_list_, m_segment_not_allocate_agv.is_init_)) {

			m_segment_reallocated_list.push_back(m_segment_not_allocate_agv);
		}
	}

	if (m_segment_not_allocate_agv_list.size() == 0) {
		return;
	}

	for (unsigned int i = 0; i < m_segment_reallocated_list.size(); ++i) {
		SegmentNotAllocateAgv m_reallocated = m_segment_reallocated_list[i];
		for (unsigned int j = 0; j < segment_not_allocate_agv_.size(); ++j) {
			SegmentNotAllocateAgv &m_not_allocated = segment_not_allocate_agv_[j];
			if (m_reallocated.segment_id_ == m_not_allocated.segment_id_
				&& m_reallocated.wop_id_ == m_not_allocated.wop_id_
				&& m_reallocated.agv_id_ == m_not_allocated.agv_id_) {
				m_not_allocated.is_allocated_ = true;
				break;
			}
		}
	}
}

std::vector<int> TrafficManage::CheckDirectionChangeNode(std::vector<trail_t> complete_path)
{
	std::lock_guard<decltype(mutex_traffic_)> guard(mutex_traffic_);

	std::vector<int> node_list;
	if (complete_path.size() <= 1) {

		return node_list;
	}

	for (unsigned int i = 0; i < complete_path.size() - 1; ++i) {

		trail_t first_path = complete_path[i];
		trail_t second_path = complete_path[i + 1];

		PathSearch_EDGEINFO first_edge;
		PathSearch_EDGEINFO second_edge;

		bool have_first_edge = false;
		bool have_second_edge = false;

		for (unsigned int i = 0; i < layout_data_->edgeList.size(); ++i) {

			PathSearch_EDGEINFO &m_segment = layout_data_->edgeList[i];
			if (m_segment.id == first_path.edge_id_) {

				first_edge = m_segment;
				have_first_edge = true;
			}

			if (m_segment.id == second_path.edge_id_) {

				second_edge = m_segment;
				have_second_edge = true;
			}

			if (true == have_first_edge && true == have_second_edge) {
				
				break;
			}
		}

		PathSearch_Wop first_wop;
		PathSearch_Wop second_wop;

		bool have_first_wop = false;
		bool have_second_wop = false;

		for (unsigned int i = 0; i < layout_data_->wopList.size(); ++i) {

			PathSearch_Wop &m_wop = layout_data_->wopList[i];
			if (m_wop.wop_id == first_path.wop_id_) {

				first_wop = m_wop;
				have_first_wop = true;
			}

			if (m_wop.wop_id == second_path.wop_id_) {

				second_wop = m_wop;
				have_second_wop = true;
			}

			if (true == have_first_wop && true == have_second_wop) {

				break;
			}
		}
	
		if (true == have_first_edge && true == have_second_edge && true == have_first_wop && true == have_second_wop) {

			double cur_g_wop_angle = 0;
			double nxt_g_wop_angle = 0;

			if (true == this->GetGlobalWopAngle(first_wop, first_edge, false, cur_g_wop_angle)
				&& true == this->GetGlobalWopAngle(second_wop, second_edge, true, nxt_g_wop_angle)) {

				loinfo("traffic") << "first_wop:" << first_wop.angle << " first_edge:" << first_edge.id << " cur_g_wop_angle:" << cur_g_wop_angle
					<< "second_wop:" << second_wop.angle << " second_edge:" << second_edge.id << " nxt_g_wop_angle:" << nxt_g_wop_angle;
				if (fabs(this->NormalAngle(cur_g_wop_angle - nxt_g_wop_angle)) > 90.0 / 180 * Pi) {

					int target_start_id = second_edge.start_node_id;
					if (second_wop.direction == 1) {
						target_start_id = second_edge.end_node_id;
					}

					node_list.push_back(target_start_id);
				}
			}
		}
	}

	return node_list;
}

bool TrafficManage::GetGlobalWopAngle(const PathSearch_Wop &wop, const PathSearch_EDGEINFO &edge, const bool start_node, double &global_wop_angle)
{
	position_t start_pos;
	if (false == this->GetPosByT(0, edge, start_pos)) {
		return false;
	}

	position_t end_pos;
	if (false == this->GetPosByT(1, edge, end_pos)) {
		return false;
	}

	if (true == start_node) {

		if (wop.direction == 0) {
			// forward
			global_wop_angle = start_pos.angle_;
		}
		else {
			// backward
			global_wop_angle = this->NormalAngle(end_pos.angle_ + Pi);
		}
	}
	else {

		if (wop.direction == 0) {
			// forward
			global_wop_angle = end_pos.angle_;
		}
		else {
			// backward
			global_wop_angle = this->NormalAngle(start_pos.angle_ + Pi);
		}

	}

	return true;
}

bool TrafficManage::GetPosByT(const double t, const PathSearch_EDGEINFO &edge, position_t &pos)
{
	position_t start_pos, end_pos;

	if (false == this->GetNodePos(edge.start_node_id, start_pos) || false == this->GetNodePos(edge.end_node_id, end_pos)) {

		return false;
	}

	double x[9], dif_x, dif_y;

	x[0] = start_pos.x_;
	x[1] = start_pos.y_;
	x[2] = edge.ctrl_point1.x;
	x[3] = edge.ctrl_point1.y;
	x[4] = edge.ctrl_point2.x;
	x[5] = edge.ctrl_point2.y;
	x[6] = end_pos.x_;
	x[7] = end_pos.y_;
	x[8] = t;

	pos.x_ = x[0] * pow((1 - x[8]), 3) + x[2] * 3 * pow((1 - x[8]), 2) * x[8] + x[4] * 3 * (1 - x[8]) * pow(x[8], 2) + x[6] * pow(x[8], 3); // x
	pos.y_ = x[1] * pow((1 - x[8]), 3) + x[3] * 3 * pow((1 - x[8]), 2) * x[8] + x[5] * 3 * (1 - x[8]) * pow(x[8], 2) + x[7] * pow(x[8], 3); // y
	dif_x = 3 * x[6] * pow(x[8], 2) - 3 * x[4] * pow(x[8], 2) - 3 * x[0] * pow((x[8] - 1), 2) + 3 * x[2] * pow((x[8] - 1), 2) - 6 * x[4] * x[8] * (x[8] - 1) + 3 * x[2] * x[8] * (2 * x[8] - 2);
	dif_y = 3 * x[7] * pow(x[8], 2) - 3 * x[5] * pow(x[8], 2) - 3 * x[1] * pow((x[8] - 1), 2) + 3 * x[3] * pow((x[8] - 1), 2) - 6 * x[5] * x[8] * (x[8] - 1) + 3 * x[3] * x[8] * (2 * x[8] - 2);
	pos.angle_ = atan2(dif_y, dif_x);

	return true;
}

bool TrafficManage::GetNodePos(const int node_id, position_t &pos)
{
	for (unsigned int i = 0; i < layout_data_->nodeList.size(); ++i) {

		PathSearch_NodeInfo &m_node_info = layout_data_->nodeList[i];
		if (m_node_info.node_id == node_id) {

			pos.x_ = m_node_info.pos_x;
			pos.y_ = m_node_info.pos_y;
			pos.angle_ = 0;

			return true;
		}
	}

	return false;
}

double TrafficManage::NormalAngle(double sita)
{
	double result = sita;
	while (result > Pi) {
		result = result - 2 * Pi;
	}
	while (result < -Pi) {
		result = result + 2 * Pi;
	}
	return result;
}
