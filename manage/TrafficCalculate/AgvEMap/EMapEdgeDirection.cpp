#include "EMapEdgeDirection.h"

EMapEdgeDirection::EMapEdgeDirection(QPointF source, QPointF first_control, QPointF second_control, QPointF dest, EdgeDirection direction, bool allow_back, QGraphicsItem*parent/* =0 */)
	: source_(source), first_control_(first_control), second_control_(second_control), dest_(dest), 
	  direction_(direction), allow_back_(allow_back), QGraphicsItem(parent)
{
	this->setCacheMode(DeviceCoordinateCache);
	this->setZValue(-8);
}

QRectF EMapEdgeDirection::boundingRect() const
{
	QPainterPath path;
	path.moveTo(mapFromScene(source_));
	path.cubicTo(mapFromScene(first_control_), mapFromScene(second_control_), mapFromScene(dest_));

	QPainterPathStroker stroker;
	stroker.setWidth(2.5 * kEdgeWidth_ * 2); 
	path = stroker.createStroke(path);
	return (path.controlPointRect() | childrenBoundingRect());
}

QPainterPath EMapEdgeDirection::shape() const
{
	QPainterPath path;
	path.moveTo(mapFromScene(source_));
	path.cubicTo(mapFromScene(first_control_), mapFromScene(second_control_), mapFromScene(dest_));
	QPainterPathStroker stroker;
	stroker.setWidth(kEdgeWidth_); 
	return stroker.createStroke(path);
}

void EMapEdgeDirection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPainterPath path;
	path.moveTo(mapFromScene(source_));
	path.cubicTo(mapFromScene(first_control_), mapFromScene(second_control_), mapFromScene(dest_));

	if (Forward == direction_) {
		this->DrawEdgeDirection(painter, path, true);
	}
	else if (Backward == direction_) {
		this->DrawEdgeDirection(painter, path, false);
	}
	else if (BothDirection == direction_) {
		this->DrawEdgeDirection(painter, path, true);
		this->DrawEdgeDirection(painter, path, false);
	}
}

void EMapEdgeDirection::DrawEdgeDirection(QPainter *painter, QPainterPath path, bool direction)
{
	// ÏßÉÏ»­¼ýÍ·
	QPointF pos0, pos1, pos2;

	if (true == direction) {
		pos0 = path.pointAtPercent(0.65);
		pos1 = path.pointAtPercent(0.7);
		pos2 = path.pointAtPercent(0.75);
	}
	else {
		pos0 = path.pointAtPercent(0.35);
		pos1 = path.pointAtPercent(0.3);
		pos2 = path.pointAtPercent(0.25);
	}
	
	double pathlength = kEdgeWidth_ / 2;

	QLineF line(pos1, pos2);

	double langle = (360 - line.angle())* 3.1415926 /180;
	double k = tan(langle);

	double k1=-1/k;
	double agl0=atan(k);
	double agl1=atan(k1);
	QPointF ar1 = pos1 + QPointF(pathlength * cos(agl1), pathlength * sin(agl1));
	QPointF ar2 = pos1 - QPointF(pathlength * cos(agl1), pathlength * sin(agl1));
	QPointF ar0 = pos1 + QPointF(pathlength * cos(langle), pathlength * sin(langle));
	QPointF ar3 = pos1 - QPointF(2 * pathlength * cos(langle), 2 * pathlength * sin(langle));

	painter->setPen(QPen(Qt::red));
	painter->setBrush(QColor(Qt::red));
	//painter->drawLine(mapFromScene(pos1), mapFromScene(pos2));
	painter->drawLine(ar3, pos1);
	painter->drawPolygon(QPolygonF()<<ar0<<ar1<<ar2);
}





