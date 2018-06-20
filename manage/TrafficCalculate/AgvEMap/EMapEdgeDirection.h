#pragma once
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qstyleoption.h>
#include <QtGui/qpainter.h>

class QPainterPath;
class QPainter;
class QPolygonF;

class EMapEdgeDirection : public QGraphicsItem
{
public:
	enum EdgeDirection {
		Forward,
		Backward,
		BothDirection
	};

public:
	EMapEdgeDirection(QPointF source, QPointF first_control, QPointF second_control, QPointF dest, EdgeDirection direction, bool allow_back, QGraphicsItem*parent=0);

public:
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
	void DrawEdgeDirection(QPainter *painter, QPainterPath path, bool direction);

private:
	static const int kEdgeWidth_ = 20;

private:
	QPointF source_;
	QPointF first_control_;
	QPointF second_control_;
	QPointF dest_;

	EdgeDirection direction_;
	bool allow_back_;
};

