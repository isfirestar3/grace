#include "CtrlPointItem.h"
#include "NodeItem.h"
#include "EdgeItem.h"
#include "Geometry.h"
#include "CtrlPointInfoWnd.h"
#include <QtWidgets/qapplication.h>

CtrlPointItem::CtrlPointItem(NodeItem* pNodeItem, EdgeItem* pEdgeItem)
: m_pNodeItem(pNodeItem)
, m_pEdgeItem(pEdgeItem)
, m_shape_r(4 * BIG_SIZE)
, m_adjust(2)
, m_mouse_hover(false)
{
	SetFeature();
	//setParentItem(pEdgeItem);
}


CtrlPointItem::~CtrlPointItem()
{
}

void CtrlPointItem::SetFeature()
{
	setFlag(ItemIsSelectable, true);
	setFlag(ItemSendsGeometryChanges);
	setCacheMode(DeviceCoordinateCache);
	this->setAcceptedMouseButtons(Qt::LeftButton);
	this->setAcceptHoverEvents(true);
	setZValue(10);
}

void CtrlPointItem::SetMoveable(bool bMoveable)
{
	setFlag(ItemIsMovable, bMoveable);
}

QRectF CtrlPointItem::boundingRect() const
{
	double shape_r = 1.5 * m_shape_r;
	return QRectF(-shape_r - m_adjust, -shape_r - m_adjust, 2 * (shape_r + m_adjust), 2 * (shape_r + m_adjust));
}

QPainterPath CtrlPointItem::shape() const
{
	double shape_r = m_mouse_hover ? 1.5 * m_shape_r : m_shape_r;

	QPainterPath path;
	path.addRect(-shape_r, -shape_r, 2 * shape_r, 2 * shape_r);
	return path;
}

void CtrlPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	double shape_r = m_mouse_hover ? 1.5 * m_shape_r : m_shape_r;

	painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)), true);
	QRadialGradient gradient(-3, -3, 2 * shape_r);

	gradient.setColorAt(0, Qt::cyan);
	gradient.setColorAt(1, Qt::blue);
	painter->setBrush(gradient);
	if (isSelected())
	{
		painter->setPen(QPen(Qt::red, 2));
		gradient.setColorAt(1, Qt::cyan);
		gradient.setColorAt(0, Qt::blue);
		painter->setBrush(gradient);
	}
	else
	{
		painter->setPen(QPen(Qt::black, 0));
	}
	painter->drawRect(-shape_r, -shape_r, 2 * shape_r, 2 * shape_r);
	painter->drawText(-shape_r, -shape_r, tip_);
}

void CtrlPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if (!m_pNodeItem || !m_pEdgeItem)
	{
		return;
	}
	NodeItem *other_node = NULL;
	CtrlPointItem *other_ctrl = NULL;

	if (m_pNodeItem == m_pEdgeItem->StartNode())
	{
		other_node = m_pEdgeItem->EndNode();
	}
	else if (m_pNodeItem == m_pEdgeItem->EndNode())
	{
		other_node = m_pEdgeItem->StartNode();
	}

	if (this == m_pEdgeItem->CtrlPointItem1())
	{
		other_ctrl = m_pEdgeItem->CtrlPointItem2();
	}
	else
	{
		other_ctrl = m_pEdgeItem->CtrlPointItem1();
	}

	VecPosition pos(m_pNodeItem->scenePos().x(), m_pNodeItem->scenePos().y());
	VecPosition otherpos(other_node->scenePos().x(), other_node->scenePos().y());
	VecPosition movpos(event->scenePos().x(), event->scenePos().y());
	VecPosition vec1 = movpos - pos;
	VecPosition vec2 = otherpos - pos;
	double angle = vec1.normalizeAngle(vec1.getDirection() - vec2.getDirection());

	VecPosition otherctrlpos(other_ctrl->scenePos().x(), other_ctrl->scenePos().y());
	VecPosition vec3 = otherctrlpos - otherpos;

	double a = vec3.getY() * (pos.getX() - otherpos.getX()) - vec3.getX() * (pos.getY() - otherpos.getY());
	double b = vec3.getY() * (movpos.getX() - otherpos.getX()) - vec3.getX() * (movpos.getY() - otherpos.getY());

	bool en = qApp->property("language").toInt() == 1;
	if (a * b < 0)
	{
		m_adjust = 20;
		tip_ = en ? "Warning" : QStringLiteral("少御");
	}
	else if (angle > 90 || angle < -90)
	{
		m_adjust = 20;
		tip_ = en ? "Warning" : QStringLiteral("少御");
	}
	else{
		m_adjust = 2;
		tip_ = "";
	}
	QGraphicsItem::mouseMoveEvent(event);
}

QVariant CtrlPointItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if ((change == QGraphicsItem::ItemPositionChange) || (change == QGraphicsItem::ItemPositionHasChanged))
	{
		if (m_pEdgeItem)
		{
			m_pEdgeItem->Adjust();
		}
	}
	return QGraphicsItem::itemChange(change, value);
}

void CtrlPointItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
	m_mouse_hover = true;
	update();
}

void CtrlPointItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
	m_mouse_hover = false;
	update();
}

void CtrlPointItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	CtrlPointInfoWnd ctrlInfoWnd;
	ctrlInfoWnd.SetPos(pos());
	if (ctrlInfoWnd.exec() == QDialog::Accepted)
	{
		QPointF newPos;
		ctrlInfoWnd.GetPos(newPos);
		setPos(newPos);
	}
}