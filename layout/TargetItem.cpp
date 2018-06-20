#include "TargetItem.h"
#include "TargetInfoWnd.h"
#include "point_algorithm.hpp"

TargetItem::TargetItem()
{
	SetFeature();
}
TargetItem::~TargetItem()
{

}

void TargetItem::SetFeature()
{
	//this->setFlag(ItemIsMovable, true);
	this->setFlag(ItemIsSelectable, true);
	this->setFlag(ItemSendsGeometryChanges);
	this->setCacheMode(DeviceCoordinateCache);
	this->setAcceptedMouseButtons(Qt::LeftButton);
	this->setAcceptHoverEvents(true);
	this->setZValue(1);
}

QPainterPath TargetItem::shape() const
{
	double dScale = m_bMouseHover ? 1.5 * SHAPE_R : SHAPE_R;

	QPainterPath path;
	path.addEllipse(-dScale, -dScale, 2 * dScale, 2 * dScale);
	return path;
}

QRectF TargetItem::boundingRect() const
{
	double shaper = 0;
	shaper = 1.5 * SHAPE_R;

	double adjust = 3;
	return QRectF(-2 * shaper - adjust, -2 * shaper - adjust, 4 * (shaper + adjust), 4 * (shaper + adjust));
}

void TargetItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (painter == NULL)
	{
		return;
	}
	//此句是转换坐标系方向
	painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)), true);
	DrawTargetItem(painter);
}

void TargetItem::DrawTargetItem(QPainter* painter)
{
	if (!painter)
	{
		return;
	}
	QPen pen(QColor(255, 0, 0));
	pen.setWidth(2);
	QBrush brush(QColor(255, 255, 128));
	painter->setPen(pen);
	painter->setBrush(brush);
	painter->drawEllipse(-m_dScale, -m_dScale, 2 * m_dScale, 2 * m_dScale);

	pen.setColor(QColor(0, 0, 255));
	brush.setColor(QColor(0, 0, 255));
	painter->setPen(pen);
	painter->setBrush(brush);
	painter->drawPolygon(QPolygonF() 
		<< QPointF(m_dScale, 0)
		<< QPointF(0 - 0.7 * m_dScale, 0 + 0.7 * m_dScale)
		<< QPointF(0, 0)
		<< QPointF(0 - 0.7 * m_dScale, 0 - 0.7 * m_dScale));

	//绘制文字
	pen.setColor(QColor(255, 255, 255));
	painter->setPen(pen);
	QRectF textRect = QRectF(QPointF(-m_dScale, -m_dScale), QSizeF(2 * m_dScale, 2 * m_dScale));
	painter->setFont(QFont("Times", m_dScale - 5, QFont::Normal));
	QString strText;
	strText = QString::number(m_targetInfo.id);
	painter->drawText(textRect, Qt::AlignCenter, strText);
}

void TargetItem::SetTargetInfo(const TARGETINFO& targetInfo)
{
	m_targetInfo = targetInfo;
}

void TargetItem::PopupInfoWnd()
{
	TargetInfoWnd targetInfoWnd;
	targetInfoWnd.UpdateWopTable(m_edgeInfo);
	if (targetInfoWnd.exec() == QDialog::Accepted)
	{
		m_targetInfo.wop_id = targetInfoWnd.GetSelectWopId();
		if (targetInfoWnd.GetRelocFlag())
		{
			position_t point_t;
			point_t.x_ = m_targetInfo.pos_x;
			point_t.y_ = m_targetInfo.pos_y;
			point_t.angle_ = m_targetInfo.angle;
			position_t point_on;
			PT_ALG::point_on_edge(m_edgeInfo.id, point_t, point_on);

			m_targetInfo.pos_x = point_on.x_;
			m_targetInfo.pos_y = point_on.y_;

			setPos(point_on.x_, point_on.y_);
			m_targetInfo.angle = CalTargetAngle(point_on.angle_ * 180.0 / PI, m_targetInfo);
			setRotation(m_targetInfo.angle);
		}
		DATAMNG->AddTargetInfo(m_targetInfo);
		update();
	}
}

int TargetItem::CalTargetAngle(double posAngle, const TARGETINFO& targetInfo)
{
	double angle = 0.0;
	WOP wop;
	DATAMNG->GetWopById(targetInfo.wop_id, wop);
	if (wop.angle_type == CONST_ANGLE)
	{
		angle = posAngle + wop.angle;
	}
	else if (wop.angle_type == GLOBAL_CONST_ANGLE)
	{
		angle = wop.angle;
	}
	angle = (angle >= 0.0) ? ((angle >= 360.0) ? (angle - 360) : angle) : (angle + 360.0);//将角度转化成0-360范围内
	return angle;
}