#pragma once
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qstyleoption.h>
#include <QtGui/qpainter.h>
#include "BasicDefine.h"

class TargetItem : public QGraphicsItem
{
public:
	TargetItem();
	~TargetItem();
public:
	int type() const { return ItemType::ITEM_TARGET; }
	void SetFeature();
	void SetTargetInfo(const TARGETINFO& targetInfo);
	void SetEdgeInfo(const EDGEINFO& edgeInfo){ m_edgeInfo = edgeInfo; };
	int GetTargetId() const { return m_targetInfo.id; }
	void PopupInfoWnd();
	void GetTargetInfo(TARGETINFO& targetInfo){ targetInfo = m_targetInfo; };
	int CalTargetAngle(double posAngle,const TARGETINFO& targetInfo);
private:
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void DrawTargetItem(QPainter* painter);
	
protected:
private:
	double m_dScale = SHAPE_R;
	bool m_bMouseHover = false;// «∑Ò Û±Í–¸Õ£

	TARGETINFO m_targetInfo;
	EDGEINFO m_edgeInfo;
};

