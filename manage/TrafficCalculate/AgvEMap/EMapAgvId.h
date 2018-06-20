#pragma once
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qstyleoption.h>
#include <QtGui/qpainter.h>

class EMapAgvId : public QGraphicsItem
{
public:
	EMapAgvId(int agv_id, QGraphicsItem* parent = 0);

public:
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
	static const int kNormalShapeSize_ = 200;

private:
	int agv_id_;
};


