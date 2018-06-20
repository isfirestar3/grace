#include "EMapNode.h"

EMapNode::EMapNode(int node_id, QGraphicsItem* parent /* = 0 */)
	: node_id_(node_id), QGraphicsItem(parent)
{
	this->setCacheMode(DeviceCoordinateCache);
	this->setZValue(-2);
	color_valid_ = false;
}

QRectF EMapNode::boundingRect() const
{
	double m_shape_r = 1.5 * kNormalShapeSize_ + 3;
	return QRectF( -m_shape_r, -m_shape_r, 2 * m_shape_r, 2 * m_shape_r);
}

QPainterPath EMapNode::shape() const
{
	double m_shape_r = kNormalShapeSize_;
	QPainterPath path;
	path.addRect(-m_shape_r, -m_shape_r, 2 * m_shape_r, 2 * m_shape_r);
	return path;
}

void EMapNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)),true);

	QPen pen(Qt::red, 2);
	painter->setPen(pen);

	QColor main_color(Qt::yellow);
	QRadialGradient gradient(-3, -3, 10);	
	gradient.setColorAt(0, main_color);
	gradient.setColorAt(1, main_color);
	painter->setBrush(gradient);
	if (color_valid_)
	{
		painter->setBrush(color_);
	}

	double m_shape_r = kNormalShapeSize_;
	painter->drawRect(-m_shape_r, -m_shape_r, 2 * m_shape_r, 2 * m_shape_r);

	//»æÖÆÎÄ×Ö
	QRectF rect=QRectF(QPointF(-m_shape_r,-m_shape_r), QSizeF(2 * m_shape_r,2 * m_shape_r));
	painter->setFont( QFont("Times",m_shape_r - 10));
	QString draw_text;
	draw_text.setNum(node_id_);
	painter->drawText(rect, Qt::AlignCenter, draw_text);
}

void EMapNode::SetColor(QColor & color, bool color_valid)
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

void EMapNode::SetShow(bool bShow)
{
	this->setVisible(bShow);
}