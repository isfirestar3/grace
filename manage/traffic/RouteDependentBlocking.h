#ifndef TRAFFIC_MANAGE_ROUTE_DEPENDENT_BLOCKING_WEILEI_20170726_H_
#define TRAFFIC_MANAGE_ROUTE_DEPENDENT_BLOCKING_WEILEI_20170726_H_

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <navigation.h>
#include <mutex>

class TrafficManage;

struct RouteDependentBlocking
{
	int segment_id_;
	int wop_id_;

	int futrue_segment_id_;
	int futrue_wop_id_;

	int offset_;

	int allocate_number_;
};

class RDBManage
{
public:
	RDBManage();
	~RDBManage();

public:
	bool LoadRdbConfigFile(std::string file_name);

public:
	bool CheckRdbConditionActive(int segment_id, int wop_id, std::vector<trail_t> complete_path, std::vector<trail_t> &allocate_path);

	void SetTrafficManage(TrafficManage* traffic_manage);

private:
	int SplitString(const std::string& input, const std::string& delimiter, std::vector<std::string>& results);

private:
	std::mutex mutex_rdb_manage_;
	std::map<std::pair<int, int/*segment_id, wop_id*/>, std::vector<RouteDependentBlocking> > rdb_config_;

	TrafficManage* traffic_manage_;
};

#endif
