#ifndef AGV_INTERFACE_AGV_DRIVER_H_
#define AGV_INTERFACE_AGV_DRIVER_H_

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <memory>
#include "navigation.h"
#include <functional>

class TrafficManage;

class AgvDriver
{
public:
	AgvDriver(int agv_id);
	~AgvDriver();

	// 1.初始化时使用
public:
	void SetMaxSegmentDistance(double max_segment_distance);
	void SetReleaseDistance(double release_distance);

public:
	void SetTrafficManage(TrafficManage *traffic_manage);

public:
	void DoAllocatedAnddRelease();

	// 2.过程中调用
public:
	bool RegisterUPL(upl_t current_upl);						// 同步调用
	void UnRegister();											// 同步调用

	void UpdateUPL(upl_t current_upl, unsigned int wop_index);			// 更新当前UPL和当前WOP Index

public:
    void SetAllocatedAvailabeCallback(const std::function<int (int has_path)> &fn);
	bool StartTaskPath(std::vector<trail_t> complete_task_path);	// 任务开始开始路径分配
	bool UpdateTaskPath(std::vector<trail_t> complete_task_path);	// ÈÎÎñÖÐ¸üÐÂÂ·¾¶·ÖÅä
	void FinishTaskPath();											// 任务介绍结束路径分配
	bool CancelTaskPath();											// 任务取消，取消路径分配:确保车已经停下来后调用，注意取消后UPL跳

	std::vector<trail_t> GetAllocatedTaskPath(bool &is_finish,int& idx);				 // 从allocated_but_not_download_task_path_取出，放入download_and_allocated_task_path_
	bool GetBolocked(int &blocked_agv_id);
private:

	void AllocateDealCycle();
	void ReleaseDealCycle();

	double CalculateAllocatedSegmentDistance();

	bool SearchSegment(trail_t task_path, int &start_node_id, int &end_node_id, double &segment_length, bool &wop_forward, double &wop_angle);

	void ReleaseNodeDealCycle(upl_t m_current_upl);
	void ReleaseSegmentDealCycle(upl_t m_current_upl, int wop_index);
	int CheckCurrentUplIndexInNotReleaseSegment(upl_t m_current_upl, int wop_index);
	bool CheckAgvBlocked(upl_t m_current_upl, int wop_index, bool &blocked);
	bool CheckAgvBlockedByWhichAgv(upl_t m_current_upl, int wop_index, int &blocked_agv);
private:
	int agv_id_;

private:
	double agv_max_segment_distance_;
	double agv_release_distance_;

private:
	TrafficManage *traffic_manage_;

private:
	std::mutex agv_driver_mutex_;

private:
	std::shared_ptr<std::thread> thread_;

private:
	bool is_register_;
	std::vector<trail_t> register_allocated_task_path_;				// 注册时默认申请UPL的所有WOP，执行路径时，释放除第一条WOP的其他所有WOP

private:
	std::mutex agv_upl_mutex_;
	bool upl_valid_;
	upl_t current_upl_;
	unsigned int wop_index_;
	
private:
	// 路线
	bool first_task_;
	bool have_task_;
	std::vector<trail_t> complete_task_path_;						// 本次任务完整的路径
	std::vector<trail_t> need_allocate_task_path_;					// 需要申请交通的路径，从完整路径来，申请到后从该序列删除，放入已经申请到路径序列
	std::vector<trail_t> allocated_but_not_download_task_path_;		// 已经申请到，但是未被下发的路径序列，从需要申请路径序列中来，放入已经下发的路径序列
	std::vector<trail_t> download_and_allocated_task_path_;			// 已经下发的路径序列(不包括释放的)，从已申请未下发的路径序列来，放入已经释放的路径序列
	std::vector<trail_t> released_task_path_;						// 已经释放的路径序列

	unsigned int need_to_released_segment_count_;

	int not_release_segment_error_count_;
	int last_current_upl_index_in_not_relaase_segment;

private:
	bool blocked_;
	int blocked_agv_;
	bool blocked_first_print_;

private:
    std::function<int(int has_path)> __path_available_fn = nullptr;
	std::vector<int> direction_chanage_node_list_;
};

#endif



