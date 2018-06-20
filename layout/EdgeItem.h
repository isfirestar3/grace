#pragma once
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qstyleoption.h>
#include <QtGui/qpainter.h>
#include <mutex>
//#include "MapScene.h"
#include "BasicDefine.h"
#include "NodeItem.h"
#include "CtrlPointItem.h"
#include "TerminalPointItem.h"
#include "AlgorithmDefine.hpp"

#define NORMAL_TYPE 0
#define NAV_PATH_TYPE 1

class EdgeItem : public QGraphicsItem
{
public:
	EdgeItem(NodeItem* pStartNode,NodeItem* pEndNode);
	~EdgeItem();
public:
	int type() const { return ItemType::ITEM_EDGE; }
	int GetId() { return m_edgeInfo.id; }
	void SetId(int iEdgeId){ m_edgeInfo.id = iEdgeId; }
	void SetFeature();
	void SetEdgeInfo(const EDGEINFO& edgeInfo);
	void GetEdgeInfo(EDGEINFO& edgeInfo);
	void InitMembers();
	void Adjust();
	void Adjust_CtrlPos_From_NodeChange();
	void UpdateCtrlPoint(QPointF ctrlPos1, QPointF ctrlPos2);
	QPointF InitCtrlPoint1Pos(NodeItem* pStartNode, NodeItem* pEndNode);
	QPointF InitCtrlPoint2Pos(NodeItem* pStartNode, NodeItem* pEndNode);
	void InitCtrlPoint(QPointF ctrlPos1, QPointF ctrlPos2);
	void InitTerminalPoint(QPointF startPos, QPointF endPos);
	NodeItem* StartNode() const;
	NodeItem* EndNode() const;
	void SetTangentLine();
	QPainterPath GetPath();
	void AdjustTerminalPoints();
	QPointF GetStartNodePos(){ return m_startNodePos; }
	QPointF GetEndNodePos(){ return m_endNodePos; }
	QPointF GetCtrlPoint1Pos(){ return m_ctrlPt1Pos; }
	QPointF GetCtrlPoint2Pos(){ return m_ctrlPt2Pos; }
	CtrlPointItem* CtrlPointItem1(){ return m_pCtrlPoint1; }
	CtrlPointItem* CtrlPointItem2(){ return m_pCtrlPoint2; }

	void SetItemMoveable(bool bMoveable);
	void AdjustMoveTermPt(NodeItem* node, const QPointF& newpos);
	NearPoint FindNearPos(QPainterPath path, QPointF newpos, double stper, double endper);
	void UpdateWhenTermPtChanged();
	void ShowControlPoint(bool bShow);
	void ChangeEdgeColor(double percent, Direction direction, int type);
	void GetEdgeWopList(std::vector<int>& vecWopId);
	void SetEdgeWopList(const std::vector<int>& vecWopId);
	void SetEdgeSpeed(const std::vector<EDGESPEED>& speed);
	void SetEdgeRealDistance(const double length);
	void SetEdgeWopAttr(const std::vector<WOPATTR>& wopAttrList);
	void UpdateEdgeAttr(const EDGEINFO& edgeInfo);
	void RemoveFromNode();
	void UpdateEdgeInfo(EDGEINFO& edgeInfo);
	EDGEINFO m_edgeInfo;
private:
	QPainterPath shape() const;
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void DrawEdge(QPainter* painter);
	void DrawEdgeArrow(QPainter *painter);
	void CalculateArrow(QPolygonF &polygon, QPointF &arrowendnear, QPointF &arrowend);
	void DrawTangentLine(QPainter *painter);
	void DrawEdgeText(QPainter* painter);
	void UpdateEdgeDirection(EDGEINFO& edgeInfo);
	bool judge_line_ctrl_point();
protected:
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void keyPressEvent(QKeyEvent *event); // add by fengqiang 2017 06 16
private:
	std::recursive_mutex m_mutexEdgeInfo;

	NodeItem* m_pStartNode;
	NodeItem* m_pEndNode;

	CtrlPointItem* m_pCtrlPoint1;
	CtrlPointItem* m_pCtrlPoint2;
	TerminalPointItem* m_pStartTermPt;
	TerminalPointItem* m_pEndTermPt;

	QPointF m_startNodePos;
	QPointF m_endNodePos;
	QPointF m_ctrlPt1Pos;
	QPointF m_ctrlPt2Pos;
	QPointF m_termPt1Pos;
	QPointF m_termPt2Pos;

	QLineF m_lineTangent1;
	QLineF m_lineTangent2;
	QPainterPath m_pathTangent1;
	QPainterPath m_pathTangent2;

	bool m_mouse_hover;
	int m_color_type;
	double m_start_percent;
	Direction m_direction;
};

