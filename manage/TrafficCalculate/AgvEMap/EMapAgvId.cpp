#include "EMapAgvId.h"

EMapAgvId::EMapAgvId(int agv_id, QGraphicsItem* parent /* = 0 */)
	: agv_id_(agv_id), QGraphicsItem(parent)
{
	this->setCacheMode(DeviceCoordinateCache);
	this->setZValue(0);
}

QRectF EMapAgvId::boundingRect() const
{
	double m_shape_r = 1.5 * kNormalShapeSize_ + 3;
	return QRectF( -m_shape_r, -m_shape_r, 2 * m_shape_r, 2 * m_shape_r);
}

QPainterPath EMapAgvId::shape() const
{
	double m_shape_r = kNormalShapeSize_;
	QPainterPath path;
	path.addRect(-m_shape_r, -m_shape_r, 2 * m_shape_r, 2 * m_shape_r);
	return path;
}

void EMapAgvId::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)),true);

	QPen pen(Qt::red, 2);
	painter->setPen(pen);

	double m_shape_r = kNormalShapeSize_;
	QRectF rect=QRectF(QPointF(-m_shape_r,-m_shape_r), QSizeF(2 * m_shape_r,2 * m_shape_r));
	painter->setFont( QFont("Times",m_shape_r - 10));
	QString draw_text;
	draw_text.setNum(agv_id_);
	painter->drawText(rect, Qt::AlignCenter, draw_text);
}