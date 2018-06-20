#include "TerminalPointItem.h"
#include "NodeItem.h"
#include "EdgeItem.h"


TerminalPointItem::TerminalPointItem(NodeItem* pNodeItem, EdgeItem* pEdgeItem)
: m_pNodeItem(pNodeItem)
, m_pEdgeItem(pEdgeItem)
, m_shape_r(25)
{
	SetFeature();
	setParentItem(pEdgeItem);
}


TerminalPointItem::~TerminalPointItem()
{
}

void TerminalPointItem::SetFeature()
{
	setFlag(ItemIsSelectable, true);
	setFlag(ItemSendsGeometryChanges);
	setCacheMode(DeviceCoordinateCache);
	setAcceptedMouseButtons(Qt::LeftButton);
	setZValue(11);
}

void TerminalPointItem::SetMoveable(bool bMoveable)
{
	setFlag(ItemIsMovable, bMoveable);
}

QRectF TerminalPointItem::boundingRect() const
{
	int adjust = 3;
	return QRectF(-m_shape_r - adjust, -m_shape_r - adjust, 2 * (m_shape_r + adjust), 2 * (m_shape_r + adjust));
}

QPainterPath TerminalPointItem::shape() const
{
	QPainterPath path;
	path.addEllipse(-m_shape_r, -m_shape_r, 2 * m_shape_r, 2 * m_shape_r);
	return path;
}

void TerminalPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (painter == NULL || m_pEdgeItem == NULL)
	{
		return;
	}
	painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)), true);
	if (isSelected())
	{
		painter->setPen(QPen(Qt::darkGreen, 6, Qt::DashLine));
	}
	else
	{
		painter->setPen(QPen(Qt::green, 3, Qt::DashLine));
	}

	painter->drawEllipse(-m_shape_r, -m_shape_r, 2 * m_shape_r, 2 * m_shape_r);

	//»æÖÆÎÄ×Ö
	QRectF rect = QRectF(QPointF(-m_shape_r, -m_shape_r), QSizeF(2 * m_shape_r, 2 * m_shape_r));
	painter->setFont(QFont("Times", m_shape_r - 15));

	NodeItem *another_node;
	if (m_pNodeItem == m_pEdgeItem->StartNode())
	{
		another_node = m_pEdgeItem->EndNode();
	}
	else
	{
		another_node = m_pEdgeItem->StartNode();
	}
	QString strText = QString().setNum(m_pNodeItem->GetNodeId()) + "-" + QString().setNum(another_node->GetNodeId());
	painter->drawText(rect, Qt::AlignCenter, strText);
}

void TerminalPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	GraphicsItemFlags itemflags = flags();
	if (((itemflags & QGraphicsItem::ItemIsMovable) >> itemflag(QGraphicsItem::ItemIsMovable)) == 1)
	{
		QPointF newPos = event->scenePos();
		if (m_pEdgeItem)
		{
			m_pEdgeItem->AdjustMoveTermPt(m_pNodeItem, newPos);
		}
	}
	QGraphicsItem::mouseMoveEvent(event);
}

int TerminalPointItem::itemflag(GraphicsItemFlag flag)
{
	if (flag == 0)
		return 0;
	int i = 0;
	int intflag = (int)flag;
	while (true) 
	{
		intflag = intflag / 2;
		if (intflag == 0) 
		{
			break;
		}
		i++;
	}
	return i;
}

QVariant TerminalPointItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if ((change == QGraphicsItem::ItemPositionChange) || (change == QGraphicsItem::ItemPositionHasChanged))
	{
		if (m_pEdgeItem)
		{
			m_pEdgeItem->UpdateWhenTermPtChanged();
		}
	}

	return QGraphicsItem::itemChange(change, value);
}