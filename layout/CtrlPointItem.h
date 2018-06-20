#pragma once
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qstyleoption.h>
#include <QtGui/qpainter.h>
#include "BasicDefine.h"

class NodeItem;
class EdgeItem;

class CtrlPointItem : public QGraphicsObject
{
public:
	CtrlPointItem(NodeItem* pNodeItem,EdgeItem* pEdgeItem);
	~CtrlPointItem();
public:
	void SetFeature();
	void SetMoveable(bool bMoveable);
protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
	QVariant itemChange(GraphicsItemChange change, const QVariant& value);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
public:
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
	NodeItem* m_pNodeItem;
	EdgeItem* m_pEdgeItem;

	double m_shape_r;
	QString tip_;
	double m_adjust;
	bool m_mouse_hover;
};

