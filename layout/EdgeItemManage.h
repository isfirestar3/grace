#pragma once
#include "EdgeItem.h"
#include "AlgorithmDefine.hpp"
#include "GlobalVariables.h"

class MapScene;

class EdgeItemManage
{
public:
	EdgeItemManage(MapScene* mapScene);
	~EdgeItemManage();
public:
	void SetEdgeInfo(const std::vector<EDGEINFO>& vecEdgeInfo);
	EdgeItem* AddEdgeItem(int startNodeId, int endNodeId,int edgeId = -1);
	EdgeItem* AddEdgeItem(const EDGEINFO& edgeInfo);
	EdgeItem* CreateEdgeItem(int startNodeId, int endNodeId);
	void AddEdgeToVector(EdgeItem* pEdgeItem);
	void SetItemMoveable(bool bMoveable);
	EdgeItem* GetEdgeItem(int EdgeId);
	EdgeItem* FindEdge(int startNodeId, int endNodeId);//不定方向的边
	EdgeItem* FindEdgeDirect(int startNodeId, int endNodeId);//确定方向的边
	void GetAllEdgeItem(std::vector<EdgeItem*>& vecEdgeItem);
	void SetEdgeSrcNode(NodeItem* pNodeItem);
	void AddLineItem(const QPointF& startPos, const QPointF& endPos);
	EdgeItem* DrawMoveLine(NodeItem* pDestNode, const QPointF& mousePos);
	void DeleteLineItem();
	void HideAllEdgeCtrlPoint();
	void ShowNavPath(const std::vector<UPL>& vecUpl);
	void ResetPathColor();

	bool RemoveEdgeItem(int edge_id);
	void AddSelectEdgeItem(EdgeItem* edge_item);
	void RemoveAllSelectEdgeItem();
	int GetSelectEdgeNum();
	void DrawMoveLine(NodeItem* srcNode, NodeItem* pDestNode, const QPointF& mousePos);
	EdgeItem* NodeItemOnEdge(NodeItem* nodeItem);
	bool CreateLinkCurve(NodeItem* srcNode, NodeItem* dstNode,double angle);
	void NodePointWithAngle(NodeItem* nodeItem, EdgeItem* edgeItem, PointParamete& point);
	bool AddLinkCurve(NodeItem* srcNode, NodeItem* dstNode, const std::vector<ControlPoints_>& ctrl_points);
	void UpdateEdgeItem(EdgeItem* edge_item,const ControlPoints_& ctrl_point1, const ControlPoints_& ctrl_point2);
	void ConvertNodeAngle90(PointParamete& point_start, PointParamete& point_end);
	void ConvertNodeAngle180(PointParamete& point_start, PointParamete& point_end, double angle);
	void ConvertPointOrder(NodeItem* srcNode, NodeItem* dstNode, std::vector<ControlPoints_>& ctrl_points);
private:
	void UpdateLineItem(const QPointF& startPos, const QPointF& endPos);
	int GetMaxEdgeId();
	bool less_than(double val, double param);
	bool greater_than(double val, double param);
private:
	MapScene* m_pMapScene;
	std::recursive_mutex m_mutexEdgeItem;
	std::vector<EdgeItem*> m_vecEdgeItem;
	NodeItem* m_pSrcNode;
	NodeItem* m_pDestNode;
	QGraphicsLineItem* m_pDrawLineItem;
	std::vector<EdgeItem*> select_edge_items;
};

