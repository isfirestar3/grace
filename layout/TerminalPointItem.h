#pragma once
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qstyleoption.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtGui/qpainter.h>
#include "BasicDefine.h"

class NodeItem;
class EdgeItem;

class TerminalPointItem : public QGraphicsItem
{
public:
	TerminalPointItem(NodeItem* pNodeItem,EdgeItem* pEdgeItem);
	~TerminalPointItem();
public:
	void SetFeature();
	void SetMoveable(bool bMoveable);
public:
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	QVariant itemChange(GraphicsItemChange change, const QVariant& value);
private:
	int itemflag(GraphicsItemFlag flag);
private:
	NodeItem* m_pNodeItem;
	EdgeItem* m_pEdgeItem;

	double m_shape_r;
};

