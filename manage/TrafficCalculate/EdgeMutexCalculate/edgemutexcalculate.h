#pragma once

#include <QtWidgets/QMainWindow>
#include "PathSearch.h"
#include "thread"

class EMapScene;
class EMapView;
class EMapAgv;
class EMapAgvId;

struct EdgeWopMutex
{
	std::pair<int/*edge id*/, int/*wop_id*/> edge_wop_id_;
	std::vector<std::pair<int/*edge id*/, int/*wop_id*/> > mutex_edge_wops_;
	
};

class EdgeMutexCalculate : public QMainWindow
{
	Q_OBJECT

public:
	EdgeMutexCalculate(double agv_width, double agv_head, double agv_tail, QWidget *parent = 0);
	~EdgeMutexCalculate();

public:
	void LoadMapData(const std::string & file_name);

public:
	void CalculateEdgeMutex();
	
private:
	void InitForm();

private:
	void LoadWopData();

private:
	bool CheckEdgeMutex(PathSearch_Wop m_edge, PathSearch_Wop m_other_edge);

	// µ¥Î»m£¬¶È
	bool CheckAgvCollocation(PathSearch_Position agv_pos_1, PathSearch_Position agv_pos_2);

	bool FindNodeInfo(int node_id, PathSearch_NodeInfo &node_info);
	bool FindEdgeInfo(int edge_id, PathSearch_EdgeInfo &edge_info);

	double CalculateDist(PathSearch_NodeInfo agv_pos_1, PathSearch_NodeInfo agv_pos_2);
	double CalculateDirection(PathSearch_POSPOINT agv_pos_1, PathSearch_POSPOINT agv_pos_2);
	


private:
	EMapScene *emap_scene_;
	EMapView *emap_view_;

	EMapAgv *agv_item_;
	EMapAgvId *agv_id_item_;

	EMapAgv *other_agv_item_;
	EMapAgvId *other_agv_id_item_;

private:
	PathSearch_LayoutData layout_data_;
	std::map<int, PathSearch_NodeInfo> layout_nodes_;
	std::map<int, PathSearch_EdgeInfo> layout_edges_;
	std::vector<PathSearch_Wop> layout_wops_;
	
private:
	std::vector<EdgeWopMutex> base_mutex_;

private:
	std::thread *th_;
};
