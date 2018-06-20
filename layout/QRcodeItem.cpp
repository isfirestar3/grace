#include "QRcodeItem.h"
#include "EdgeItem.h"
#include "DataManage.h"
#include "QRcodeInfoWnd.h"
#include "MapScene.h"

QRcodeItem::QRcodeItem(MapScene* mapScene)
:m_bMouseHover(false)
, m_bMousePress(false)
, m_bAlign(false)
, m_mapScene(mapScene)
{
	m_dScale = QR_SHAPE_R;
	SetFeature();
}

QRcodeItem::~QRcodeItem()
{

}

void QRcodeItem::SetFeature()
{
	//this->setFlag(ItemIsMovable, true);
	this->setFlag(ItemIsSelectable, true);
	this->setFlag(ItemSendsGeometryChanges);
	this->setCacheMode(DeviceCoordinateCache);
	this->setAcceptedMouseButtons(Qt::LeftButton);
	this->setAcceptHoverEvents(true);
	this->setZValue(-1);
}

void QRcodeItem::SetCodeInfo(const CODEPOSXYA& codeInfo)
{
	m_posInfo = codeInfo;
}

QRectF QRcodeItem::boundingRect() const
{
	double shaper = 0;
	shaper = 1.5 * QR_SHAPE_R;

	double adjust = 3;
	return QRectF(-2 * shaper - adjust, -2 * shaper - adjust, 4 * (shaper + adjust), 4 * (shaper + adjust));
}

QPainterPath QRcodeItem::shape() const
{
	double dScale = m_bMouseHover ? 1.5 * QR_SHAPE_R : QR_SHAPE_R;

	QPainterPath path;
	path.addRect(-dScale, -dScale, 2 * dScale, 2 * dScale);
	return path;
}

void QRcodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (painter == NULL)
	{
		return;
	}
	//此句是转换坐标系方向
	painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)), true);
	DrawCode(painter);
}

void QRcodeItem::DrawCode(QPainter* painter)
{
	QColor mainColor = QColor(Qt::red);
	m_dScale = m_bMouseHover ? 1.5 * QR_SHAPE_R : QR_SHAPE_R;

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
		pen = QPen(Qt::yellow, 2);
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
	font.setPixelSize(3);
	//painter->setFont(QFont("Times", m_dScale - 2, QFont::Bold));

	if (m_bMouseHover)
	{
		font.setPixelSize(6);
		font.setBold(true);
	}

	painter->setFont(font);

	QString strText;
	strText = QString::number(m_posInfo.pos_id);

	painter->drawText(textRect,Qt::AlignCenter,strText);
}

int QRcodeItem::GetCodeId()
{
	return m_posInfo.pos_id;
}

void QRcodeItem::SetMoveable(bool bMoveable)
{
	this->setFlag(ItemIsMovable, bMoveable);
}

void QRcodeItem::SetPosData(const QPointF& pos)
{

}

void QRcodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	m_bMouseHover = true;
	update();
}

void QRcodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	m_bMouseHover = false;
	update();
}

void QRcodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	m_bMousePress = true;
	QGraphicsItem::mousePressEvent(event);
}

void QRcodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	m_bMousePress = false;
	QGraphicsItem::mouseMoveEvent(event);
}

void QRcodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	m_bMousePress = false;
	m_bAlign = false;
	QGraphicsItem::mouseReleaseEvent(event);
}

QVariant QRcodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
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
		//SetPosData(newPos);
	}
	return QGraphicsItem::itemChange(change, value);
}


void QRcodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (static_cast<MapScene*>(scene())->GetActionType() == ActionType::ACTION_SELECT || 
		static_cast<MapScene*>(scene())->GetActionType() == ActionType::ACTION_HAND_DRAG)
	{
		QRcodeInfoWnd infoWnd;
		infoWnd.SetCodeInfo(m_posInfo);
		if (infoWnd.exec() == QDialog::Accepted)
		{
			infoWnd.GetCodeInfo(m_posInfo);
			//QPointF newPos;
			//newPos.setX(m_posInfo.pos_x);
			//newPos.setY(m_posInfo.pos_y);
			//setPos(newPos);
			////DATAMNG->UpdateNodeInfo(m_posInfo);
			//update();
		}
	}
	QGraphicsItem::mouseDoubleClickEvent(event);
}