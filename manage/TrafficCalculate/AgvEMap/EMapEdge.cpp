#include "EMapEdge.h"

EMapEdge::EMapEdge(int edge_id, QPointF source, QPointF first_control, QPointF second_control, QPointF dest, EdgeDirection direction, bool allow_back, QGraphicsItem*parent/* =0 */)
	: edge_id_(edge_id), source_(source), first_control_(first_control), second_control_(second_control), dest_(dest), 
	  direction_(direction), allow_back_(allow_back), QGraphicsItem(parent)
{
	this->setCacheMode(DeviceCoordinateCache);
	this->setZValue(-10);
	color_valid_ = false;
}

QRectF EMapEdge::boundingRect() const
{
	QPainterPath path;
	path.moveTo(mapFromScene(source_));
	path.cubicTo(mapFromScene(first_control_), mapFromScene(second_control_), mapFromScene(dest_));

	QPainterPathStroker stroker;
	stroker.setWidth(2.5 * kEdgeWidth_ * 4); 
	path = stroker.createStroke(path);
	return (path.controlPointRect() | childrenBoundingRect());
}

QPainterPath EMapEdge::shape() const
{
	QPainterPath path;
	path.moveTo(mapFromScene(source_));
	path.cubicTo(mapFromScene(first_control_), mapFromScene(second_control_), mapFromScene(dest_));
	QPainterPathStroker stroker;
	stroker.setWidth(kEdgeWidth_); 
	return stroker.createStroke(path);
}

void EMapEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPainterPath path;
	path.moveTo(mapFromScene(source_));
	path.cubicTo(mapFromScene(first_control_), mapFromScene(second_control_), mapFromScene(dest_));

	QPainterPathStroker stroker;
	stroker.setWidth(kEdgeWidth_); 
	stroker.setCapStyle(Qt::RoundCap);
	stroker.setJoinStyle(Qt::RoundJoin);
	QPainterPath m_painter_path = stroker.createStroke(path);

	//painter->setPen(QPen(Qt::black, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->setPen(Qt::NoPen);
	
	// 画线
	if (true == allow_back_) {
		painter->setBrush(QColor(Qt::green));
	}
	else {
		painter->setBrush(QColor(Qt::cyan));
	}
	if (color_valid_)
	{
		painter->setBrush(color_);
	}
	painter->drawPath(m_painter_path);


	QPointF textPoint = path.pointAtPercent(0.5);
	painter->setFont(QFont("Times", 40, QFont::Normal));
	QPen pen = QPen(Qt::black, 1);
	painter->setPen(pen);
	QString strText;
	strText = QString::number(edge_id_);
	painter->save();
	QTransform transform = painter->transform();
	transform.translate(textPoint.x(), textPoint.y());
	transform.scale(1, -1);
	painter->setTransform(transform);
	painter->drawText(pos(), strText);
	painter->restore();
}

void EMapEdge::DrawEdgeDirection(QPainter *painter, QPainterPath path, bool direction)
{
	// 线上画箭头
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

	painter->setPen(QPen(Qt::cyan));
	painter->setBrush(QColor(Qt::cyan));
	//painter->drawLine(mapFromScene(pos1), mapFromScene(pos2));
	painter->drawLine(ar3, pos1);
	painter->drawPolygon(QPolygonF()<<ar0<<ar1<<ar2);
}

void EMapEdge::SetColor( QColor & color, bool color_valid)
{
	if (color_valid_ != color_valid) {
		color_valid_ = color_valid;
		if (true == color_valid) {
			if (color_ != color) {
				color_ = color;
				update();
			}
		}
		else {
			color_ = color;
			update();
		}
	}
}





