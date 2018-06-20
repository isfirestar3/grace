#pragma once

#include <vector>
#include <QtWidgets/qgraphicsscene.h>
#include "PathSearch.h"

class EMapNode;
class EMapEdge;
class EMapAgv;
class EMapAgvId;

class EMapScene : public QGraphicsScene
{
	Q_OBJECT
public:
	EMapScene(QObject *parent = 0);
	~EMapScene();

public:
	void SetAgvSize(double agv_width, double agv_head, double agv_tail); // m
	void LoadMap(std::string file_name, double pos_x, double pos_y);
	void LoadNodeEdgeData(const std::vector<PathSearch_NodeInfo>& node_datas, const std::vector<PathSearch_EdgeInfo>& edge_datas, bool bShow = true);
	void SetRobotPos(const std::map<int, PathSearch_Position>& robot_pos);
	void SetAllocateAgv(const std::vector<std::pair<int, int> > &node_allocate_agv, const std::vector<std::pair<std::pair<int, int>, int> > &segment_allocate_agv);
	void SetEdgeColor(int edge_id, QColor & color, bool color_valid = false);
	void SetNodeColor(int node_id, QColor & color, bool color_valid = false);
protected:
	void timerEvent(QTimerEvent *event);

private:
	void UpdateRobotPos();
	void UpdateAllovateAgv();

private:
	bool FindNodeData(const std::vector<PathSearch_NodeInfo>& node_datas, int node_id, PathSearch_NodeInfo &node_data);
	EMapAgv* FindAgvItem(int agv_id);
	EMapAgvId* FindAgvIdItem(int agv_id);

	std::map<int, PathSearch_Position> GetRobotPos();

	std::vector<std::pair<int, int> > GetNodeAllocateAgv();
	std::vector<std::pair<std::pair<int, int>, int> > GetSegmentAllocateAgv();

	QColor GetAgvColor(int agv_id);

private:
	double agv_width_;
	double agv_head_;
	double agv_tail_;

	bool robot_pos_update_;
	std::map<int, PathSearch_Position> robot_pos_;
	QMutex robot_pos_mux_;

	bool allocate_agv_update_;
	std::vector<std::pair<int, int> > node_allocate_agv_;
	std::vector<std::pair<std::pair<int, int>, int> > segment_allocate_agv_;
	QMutex allocate_agv_mux_;

	std::map<int, EMapAgv*> agv_items_;
	std::map<int, EMapAgvId*> agv_id_items_;
	std::map<int, EMapEdge *> edge_items_;
	std::map<int, EMapNode *> node_items_;
};

