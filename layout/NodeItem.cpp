#include "NodeItem.h"
#include "EdgeItem.h"
#include "DataManage.h"
#include "NodeInfoWnd.h"
#include "MapScene.h"

NodeItem::NodeItem(MapScene* mapScene)
:m_bMouseHover(false)
, m_bMousePress(false)
, m_bAlign(false)
, m_mapScene(mapScene)
{
	m_dScale = SHAPE_R;
	SetFeature();
}

NodeItem::~NodeItem()
{

}

void NodeItem::SetFeature()
{
	//this->setFlag(ItemIsMovable, true);
	this->setFlag(ItemIsSelectable, true);
	this->setFlag(ItemSendsGeometryChanges);
	this->setCacheMode(DeviceCoordinateCache);
	this->setAcceptedMouseButtons(Qt::LeftButton);
	this->setAcceptHoverEvents(true);
	this->setZValue(-1);
}

void NodeItem::SetNodeInfo(const NODEINFO& nodeInfo)
{
	m_nodeInfo = nodeInfo;
}

QRectF NodeItem::boundingRect() const
{
	double shaper = 0;
	shaper = 1.5 * SHAPE_R;

	double adjust = 3;
	return QRectF(-2 * shaper - adjust, -2 * shaper - adjust, 4 * (shaper + adjust), 4 * (shaper + adjust));
}

QPainterPath NodeItem::shape() const
{
	double dScale = m_bMouseHover ? 1.5 * SHAPE_R : SHAPE_R;

	QPainterPath path;
	path.addRect(-dScale, -dScale, 2 * dScale, 2 * dScale);
	return path;
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (painter == NULL)
	{
		return;
	}
	//此句是转换坐标系方向
	painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)), true);
	DrawNode(painter);
}

void NodeItem::DrawNode(QPainter* painter)
{
	QColor mainColor = (m_nodeInfo.spin == TRUE) ? QColor(Qt::magenta) : QColor(Qt::yellow);
	m_dScale = m_bMouseHover ? 1.5 * SHAPE_R : SHAPE_R;

	QPen pen;
	QRadialGradient gradient(-3, -3, 10);
	if (!isSelected())
	{
		gradient.setColorAt(0, mainColor);
		gradient.setColorAt(1, mainColor);
		pen = QPen(Qt::black, 0);
	}
	else
	{
		gradient.setColorAt(1, QColor(mainColor).light(120));
		gradient.setColorAt(0, QColor(mainColor).light(120));
		pen = QPen(Qt::red, 2);
	}
	painter->setBrush(gradient);
	painter->setPen(pen);
	painter->drawRect(-m_dScale, -m_dScale, 2 * m_dScale, 2 * m_dScale);
	if (m_bMousePress && m_bAlign)
	{
		painter->drawLine(QLineF(QPointF(-2 * m_dScale, 0), QPointF(-0.5* m_dScale, 0)));
		painter->drawLine(QLineF(QPointF(0.5* m_dScale, 0), QPointF(2 * m_dScale, 0)));
		painter->drawLine(QLineF(QPointF(0, -2 * m_dScale), QPointF(0, -0.5* m_dScale)));
		painter->drawLine(QLineF(QPointF(0, 0.5* m_dScale), QPointF(0, 2 * m_dScale)));
	}
	//绘制文字
	QRectF textRect = QRectF(QPointF(-m_dScale, -m_dScale), QSizeF(2 * m_dScale, 2 * m_dScale));
	QFont font;
	font.setPixelSize(5);
	//painter->setFont(QFont("Times", m_dScale - 2, QFont::Bold));

	if (m_bMouseHover)
	{
		font.setPixelSize(8);
		font.setBold(true);
	}

	painter->setFont(font);

	QString strText;
	strText = QString::number(m_nodeInfo.node_id);
	if (m_nodeInfo.tshaped == TRUE)
	{
		strText += "T";
	}
	painter->drawText(textRect,Qt::AlignCenter,strText);
}

int NodeItem::GetNodeId()
{
	return m_nodeInfo.node_id;
}

int NodeItem::GetTshaped()
{
	return m_nodeInfo.tshaped;
}

void NodeItem::SetMoveable(bool bMoveable)
{
	this->setFlag(ItemIsMovable, bMoveable);
}

void NodeItem::SetPosData(const QPointF& pos)
{
	m_nodeInfo.pos_x = pos.x();
	m_nodeInfo.pos_y = pos.y();
	DATAMNG->UpdateNodeInfo(m_nodeInfo);
}

void NodeItem::SetEdgeItem(EdgeItem* pEdgeItem)
{
	m_listEdgeItem << pEdgeItem;
}

void NodeItem::RemoveEdge(EdgeItem* pEdgeItem)
{
	int index = m_listEdgeItem.indexOf(pEdgeItem);
	if (index != -1)
	{
		m_listEdgeItem.removeAt(index);
	}
}

void NodeItem::RemoveAllEdge()
{
	if (!m_mapScene)
	{
		return;
	}
	foreach(EdgeItem *edge, m_listEdgeItem)
	{
		m_mapScene->DeleteEdgeItem(edge);
	}
}

void NodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	m_bMouseHover = true;
	update();
}

void NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	m_bMouseHover = false;
	update();
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	m_bMousePress = true;
	QGraphicsItem::mousePressEvent(event);
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	m_bMousePress = false;
	QGraphicsItem::mouseMoveEvent(event);
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	m_bMousePress = false;
	m_bAlign = false;
	QGraphicsItem::mouseReleaseEvent(event);
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (change == ItemPositionChange || change == ItemPositionHasChanged)
	{
		//m_bAlign = true;
		foreach(EdgeItem* pEdgeItem, m_listEdgeItem)
		{
			if (pEdgeItem)
			{
				pEdgeItem->Adjust();
			}
		}
		QPointF newPos = value.toPointF();
		SetPosData(newPos);
	}
	return QGraphicsItem::itemChange(change, value);
}


void NodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (static_cast<MapScene*>(scene())->GetActionType() == ActionType::ACTION_SELECT || 
		static_cast<MapScene*>(scene())->GetActionType() == ActionType::ACTION_HAND_DRAG)
	{
		NodeInfoWnd infoWnd;
		infoWnd.SetNodeInfo(m_nodeInfo);
		if (infoWnd.exec() == QDialog::Accepted)
		{
			infoWnd.GetNodeInfo(m_nodeInfo);
			QPointF newPos;
			newPos.setX(m_nodeInfo.pos_x);
			newPos.setY(m_nodeInfo.pos_y);
			setPos(newPos);
			DATAMNG->UpdateNodeInfo(m_nodeInfo);
			update();
		}
	}
	QGraphicsItem::mouseDoubleClickEvent(event);
}


void  NodeItem::UpdateNodeInfo(bool spin)
{
	spin ? m_nodeInfo.spin = 1 : m_nodeInfo.spin = 0;
	DATAMNG->UpdateNodeInfo(m_nodeInfo);
	update();
}