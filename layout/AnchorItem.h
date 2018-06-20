#pragma once
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qstyleoption.h>
#include <QtGui/qpainter.h>
#include "BasicDefine.h"

class AnchorItem : public QGraphicsItem
{
public:
	AnchorItem();
	~AnchorItem();
public:
	int type() const { return ItemType::ITEM_ANCHOR; }
	void SetFeature();
	void SetAnchorInfo(const ANCHORINFO& anchorInfo){ m_anchorInfo = anchorInfo; }
	void GetAnchorInfo(ANCHORINFO& anchorInfo){ anchorInfo = m_anchorInfo; }
	void SetEdgeInfo(const EDGEINFO& edgeInfo){ m_edgeInfo = edgeInfo; }
	int	 GetAnchorId(){ return m_anchorInfo.id; }
	int  GetEdgeId(){ return m_anchorInfo.edge_id; }
	void PopupInfoWnd();
	void ChangeToTarget(bool bTarget);
	bool IsTarget(){ return m_is_target; }
private:
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void DrawAnchorItem(QPainter* painter);
	int CalAnchorAngle(double posAngle, const ANCHORINFO& anchorInfo);
protected:
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
private:
	double m_dScale = SHAPE_R;
	bool m_bMouseHover = false;//是否鼠标悬停
	bool m_is_target = false; //是否被设为了目标点

	ANCHORINFO m_anchorInfo;
	EDGEINFO m_edgeInfo;
};

