#include "EMapAgv.h"
#include "CoordinateSystem.hpp"

EMapAgv::EMapAgv(int car_id, QGraphicsItem *parent)
: car_id_(car_id), agv_image_(new QImage(":/images/agv.png")), QGraphicsItem(parent)
{
	this->setCacheMode(DeviceCoordinateCache);
	this->setZValue(-1);

	agv_width_ = kAgvWidth;
	agv_head_ = kAgvHead;
	agv_tail_ = kAgvTail;
}

void EMapAgv::SetAgvSize( double agv_width, double agv_head, double agv_tail )
{
	agv_width_ = CoordinateSystem::Map2Scene(agv_width);
	agv_head_ = CoordinateSystem::Map2Scene(agv_head);
	agv_tail_ = CoordinateSystem::Map2Scene(agv_tail);
}

QRectF EMapAgv::boundingRect() const
{
	//QRectF rect(-kAgvTail, -kAgvWidth / 2, kAgvHead + kAgvTail, kAgvWidth);
	QRectF rect(-agv_tail_, -agv_width_ / 2, agv_head_ + agv_tail_, agv_width_);
	return rect;
}

QPainterPath EMapAgv::shape() const
{
	QPainterPath path;
	path.addRect(-agv_tail_, -agv_width_ / 2, agv_head_ + agv_tail_, agv_width_);
	return path;
}

void EMapAgv::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)),true);
	
	QPen pen(Qt::red, 20);
	painter->setPen(pen);

	//painter->drawImage(QRectF(-kAgvHead, -kAgvWidth / 2, kAgvHead + kAgvTail, kAgvWidth),*agv_image_);
	painter->drawRect(QRectF(-agv_tail_, -agv_width_ / 2, agv_head_ + agv_tail_, agv_width_));
	painter->drawRect(QRectF(-100, -100, 200, 200));
}
