#include "VehicleItem.h"
#include <mutex>
#include "VehicleInfoWnd.h"

#define ITEM_RADIUS (10 * BIG_SIZE)

VehicleItem::VehicleItem()
: m_shape_r(ITEM_RADIUS)
, m_mouse_hover(false)
, m_bCollideState(false)
{
	SetFeature();
}

VehicleItem::~VehicleItem()
{
	
}

void VehicleItem::SetFeature()
{
	setZValue(2);
	setAcceptDrops(true);
	setFlags(ItemIsSelectable);
}

QRectF VehicleItem::boundingRect() const
{
	double shape_r = 1.5 * m_shape_r;
	int adjust = 2;
	return QRectF(-shape_r - adjust, -shape_r - adjust, (shape_r + adjust) * 2, (shape_r + adjust) * 2);
}

void VehicleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (painter == NULL)
	{
		return;
	}
	painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)), true);
	QPixmap pixmap(QString::fromUtf8(":/newPrefix/images/vehicle.png"));
	QPixmap pixmapGray(QString::fromUtf8(":/newPrefix/images/vehicle_gray.png"));
	QRectF sourceRect(pixmap.rect());

	double shape_r = m_mouse_hover ? 1.5 * m_shape_r : m_shape_r;
	QRectF destRect(-shape_r, -shape_r, shape_r * 2, shape_r * 2);
		
	if (!m_bCollideState)
	{
		painter->drawPixmap(destRect, pixmap, sourceRect);
		//painter->drawRect(destRect);
	}
	else
	{
		painter->drawPixmap(destRect, pixmapGray, sourceRect);
		//painter->drawRect(destRect);
	}
	painter->setFont(QFont("Times", m_shape_r - 2));
	painter->setPen(QColor(255, 0, 0, 255));
	QString qsId = QString::number(m_vehicleInfo.vehicle_id);
	painter->drawText(destRect, Qt::AlignHCenter | Qt::AlignBottom, qsId);
}

QPainterPath VehicleItem::shape() const
{
	double shape_r = m_mouse_hover ? 1.5 * m_shape_r : m_shape_r;
    QPainterPath paintPath;
	paintPath.addRect(-shape_r, -shape_r, shape_r * 2, shape_r * 2);
    return paintPath;
}

void VehicleItem::SetVehicleInfo(const VEHICLEINFO& vehicleInfo)
{
	std::lock_guard<decltype(m_mutexVehicleInfo)> guard(m_mutexVehicleInfo);
	m_vehicleInfo = vehicleInfo;
}

int VehicleItem::GetVehicleId()
{
	std::lock_guard<decltype(m_mutexVehicleInfo)> guard(m_mutexVehicleInfo);
	return m_vehicleInfo.vehicle_id;
}

void VehicleItem::SetVehicleId(int vehicle_id)
{
	std::lock_guard<decltype(m_mutexVehicleInfo)> guard(m_mutexVehicleInfo);
	m_vehicleInfo.vehicle_id = vehicle_id;
}

void VehicleItem::GetVehicleInfo(VEHICLEINFO& vehicleInfo)
{
	std::lock_guard<decltype(m_mutexVehicleInfo)> guard(m_mutexVehicleInfo);
	m_vehicleInfo.pos_x = x();
	m_vehicleInfo.pos_y = y();
	m_vehicleInfo.angle = (rotation() + 90.0 >= 360.0) ? (rotation() + 90.0 - 360.0) : (rotation() + 90.0);
	vehicleInfo = m_vehicleInfo;
}

void VehicleItem::SetVehiclePos(const POSXYA& posXya)
{
	std::lock_guard<decltype(m_mutexVehicleInfo)> guard(m_mutexVehicleInfo);
	m_vehicleInfo.pos_x = posXya.pos_x;
	m_vehicleInfo.pos_y = posXya.pos_y;
	m_vehicleInfo.angle = posXya.angle;
}

void VehicleItem::GetVehiclePos(POSXYA& posXya)
{
	std::lock_guard<decltype(m_mutexVehicleInfo)> guard(m_mutexVehicleInfo);
	posXya.pos_x = x();
	posXya.pos_y = y();
	posXya.angle = rotation();
}

void VehicleItem::SetCollideState(bool bCollision)
{
	std::lock_guard<decltype(m_mutexCollideState)> guard(m_mutexCollideState);
	m_bCollideState = bCollision;
}

void VehicleItem::PopupInfoWnd()
{
	VehicleInfoWnd vehicleWnd;
	vehicleWnd.SetVehicleInfo(m_vehicleInfo);
	if (vehicleWnd.exec() == QDialog::Accepted)
	{
		vehicleWnd.GetVehicleInfo(m_vehicleInfo);
		setRotation(m_vehicleInfo.angle - 90.0);
	}
}