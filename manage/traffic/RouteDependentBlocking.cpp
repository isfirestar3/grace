#include "RouteDependentBlocking.h"
#include <fstream>
#include "log.h"
#include "TrafficManage.h"


RDBManage::RDBManage()
{

}

RDBManage::~RDBManage()
{

}
void RDBManage::SetTrafficManage(TrafficManage* traffic_manage)
{
	traffic_manage_ = traffic_manage;
}
bool RDBManage::LoadRdbConfigFile(std::string file_name)
{
	std::ifstream ifs;
	ifs.open(file_name.c_str());
	if (!ifs) {
		loerror("traffic") << "RDBManage load route dependent blocking file " << file_name.c_str() << " failed, not exsist...";
		return false;
	}

	int line_count = 0;
	while (!ifs.eof()) {
		line_count++;
		std::string cluster_id_list;
		getline(ifs, cluster_id_list);
		loinfo("traffic") << "rdb line" << line_count << ":" << cluster_id_list;
		if (cluster_id_list == "") {
			lotrace("traffic") << "line" << line_count << " is empty";
			continue;
		}
		else
		{
			const char* cc_cluster = cluster_id_list.c_str();
			if (cc_cluster[0] == '#')
			{
				lotrace("traffic") << "line" << line_count << " " << cc_cluster[0];
				continue;
			}
		}

		std::vector<std::string> str_node_list;
		this->SplitString(cluster_id_list, " ", str_node_list);
		if (str_node_list.size() != 6)
		{
			loerror("traffic") << "Load RDB Config File failed at line:" << line_count;
			return false;
		}

		int m_segment_id, m_segment_wop_id, m_futrue_segment_id, m_futrue_segment_wop_id, m_offset, m_allocate_number;
		m_segment_id = atoi(str_node_list[0].c_str());
		m_segment_wop_id = atoi(str_node_list[1].c_str());
		m_futrue_segment_id = atoi(str_node_list[2].c_str());
		m_futrue_segment_wop_id = atoi(str_node_list[3].c_str());
		m_offset = atoi(str_node_list[4].c_str());
		m_allocate_number = atoi(str_node_list[5].c_str());

		TrafficSegment* m_segment = traffic_manage_->FindTrafficSegment(m_segment_id, m_segment_wop_id);
		if (NULL == m_segment) {
			loerror("traffic") << "Load RDB Config File failed at line:" << line_count << " m_segment_id:" << m_segment_id << " m_segment_wop_id:" << m_segment_wop_id;
			return false;
		}	
		TrafficSegment* m_segment2 = traffic_manage_->FindTrafficSegment(m_futrue_segment_id, m_futrue_segment_wop_id);
		if (NULL == m_segment2) {
			loerror("traffic") << "Load RDB Config File failed at line:" << line_count << " m_futrue_segment_id:" << m_futrue_segment_id << " m_futrue_segment_wop_id:" << m_futrue_segment_wop_id;
			return false;
		}

		std::map<std::pair<int, int>, std::vector<RouteDependentBlocking> >::iterator iter_find = rdb_config_.find(std::pair<int, int>(m_segment_id, m_segment_wop_id));
		if (iter_find == rdb_config_.end()) {

			std::vector<RouteDependentBlocking> m_rdb_list;

			RouteDependentBlocking m_rdb;
			m_rdb.segment_id_ = m_segment_id;
			m_rdb.wop_id_ = m_segment_wop_id;

			m_rdb.futrue_segment_id_ = m_futrue_segment_id;
			m_rdb.futrue_wop_id_ = m_futrue_segment_wop_id;

			m_rdb.offset_ = m_offset;

			m_rdb.allocate_number_ = m_allocate_number;

			m_rdb_list.push_back(m_rdb);
			rdb_config_[std::pair<int, int>(m_segment_id, m_segment_wop_id)] = m_rdb_list;
		}
		else {

			std::vector<RouteDependentBlocking> &m_rdb_list = iter_find->second;

			RouteDependentBlocking m_rdb;
			m_rdb.segment_id_ = m_segment_id;
			m_rdb.wop_id_ = m_segment_wop_id;

			m_rdb.futrue_segment_id_ = m_futrue_segment_id;
			m_rdb.futrue_wop_id_ = m_futrue_segment_wop_id;

			m_rdb.offset_ = m_offset;

			m_rdb.allocate_number_ = m_allocate_number;
			m_rdb_list.push_back(m_rdb);
		}
	}

	/*int m_segment_id, m_segment_wop_id, m_futrue_segment_id, m_futrue_segment_wop_id, m_offset, m_allocate_number;
	while (ifs >> m_segment_id >> m_segment_wop_id >> m_futrue_segment_id >> m_futrue_segment_wop_id >> m_offset >> m_allocate_number) {

		std::map<std::pair<int, int>, std::vector<RouteDependentBlocking> >::iterator iter_find = rdb_config_.find(std::pair<int, int>(m_segment_id, m_segment_wop_id));
		if (iter_find == rdb_config_.end()) {
			
			std::vector<RouteDependentBlocking> m_rdb_list;

			RouteDependentBlocking m_rdb;
			m_rdb.segment_id_ = m_segment_id;
			m_rdb.wop_id_ = m_segment_wop_id;

			m_rdb.futrue_segment_id_ = m_futrue_segment_id;
			m_rdb.futrue_wop_id_ = m_futrue_segment_wop_id;

			m_rdb.offset_ = m_offset;

			m_rdb.allocate_number_ = m_allocate_number;

			m_rdb_list.push_back(m_rdb);
			rdb_config_[std::pair<int, int>(m_segment_id, m_segment_wop_id)] = m_rdb_list;
		}
		else {

			std::vector<RouteDependentBlocking> &m_rdb_list = iter_find->second;

			RouteDependentBlocking m_rdb;
			m_rdb.segment_id_ = m_segment_id;
			m_rdb.wop_id_ = m_segment_wop_id;

			m_rdb.futrue_segment_id_ = m_futrue_segment_id;
			m_rdb.futrue_wop_id_ = m_futrue_segment_wop_id;

			m_rdb.offset_ = m_offset;

			m_rdb.allocate_number_ = m_allocate_number;
			m_rdb_list.push_back(m_rdb);
		}
	}*/

	return true;
}

bool RDBManage::CheckRdbConditionActive(int segment_id, int wop_id, std::vector<trail_t> complete_path, std::vector<trail_t> &allocate_path)
{
	std::lock_guard<decltype(mutex_rdb_manage_)> guard(mutex_rdb_manage_);
	std::map<std::pair<int, int>, std::vector<RouteDependentBlocking> >::iterator iter_find = rdb_config_.find(std::pair<int, int>(segment_id, wop_id));
	if (iter_find == rdb_config_.end()) {

		return false;
	}

	std::vector<RouteDependentBlocking> m_rdb_list = iter_find->second;

	unsigned int index = 0;
	bool have_segment = false;
	std::vector<trail_t> m_remain_path;
	for (; index < complete_path.size(); ++index) {

		trail_t m_path = complete_path[index];
		if (m_path.edge_id_ == segment_id && m_path.wop_id_ == wop_id) {
			
			have_segment = true;
		}

		if (true == have_segment) {

			m_remain_path.push_back(m_path);
		}
	}

	if (false == have_segment) {

		loerror("traffic") << "RDBManage complete_path donot have segment_id:" << segment_id << " wop_id:" << wop_id << "...";
		return false;
	}

	for (unsigned int i = 0; i < m_rdb_list.size(); ++i) {
		RouteDependentBlocking m_rdb = m_rdb_list[i];

		int m_futrue_segment;
		int m_futrue_wop;

		if (m_remain_path.size() > (unsigned int)m_rdb.offset_) {

			m_futrue_segment = m_remain_path[m_rdb.offset_].edge_id_;
			m_futrue_wop = m_remain_path[m_rdb.offset_].wop_id_;

			if (m_futrue_segment == m_rdb.futrue_segment_id_ && m_futrue_wop == m_rdb.futrue_wop_id_ && m_remain_path.size() >= (unsigned int)m_rdb.allocate_number_) {

				allocate_path.clear();
				for (unsigned int j = 0; j < (unsigned int)m_rdb.allocate_number_; ++j) {
					allocate_path.push_back(m_remain_path[j]);
				}
				return true;
			}
		}
	}
	return false;
}


int RDBManage::SplitString(const std::string& input, const std::string& delimiter, std::vector<std::string>& results)
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

		if (s.length() > 0) {
			results.push_back(s);
		}

	}
	return numFound;
}