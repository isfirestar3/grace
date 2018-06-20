#pragma once
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qstyleoption.h>
#include <QtGui/qpainter.h>
#include "BasicDefine.h"
#include <mutex>

class VehicleItem : public QGraphicsItem
{
public:
	VehicleItem();
	~VehicleItem();
public:
	void SetFeature();
	void SetVehicleInfo(const VEHICLEINFO& vehicleInfo);
	void GetVehicleInfo(VEHICLEINFO& vehicleInfo);
	int GetVehicleId();
	void SetVehicleId(int vehicle_id);
	void SetVehiclePos(const POSXYA& posXya);
	void GetVehiclePos(POSXYA& posXya);
	void SetCollideState(bool bCollision);
	void SetEdgeInfo(const EDGEINFO& edgeInfo){ m_edgeInfo = edgeInfo; }//车所在的边信息
	void PopupInfoWnd();
public:
	int type() const { return ItemType::ITEM_VEHICLE; }
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
private:
	std::recursive_mutex m_mutexVehicleInfo;
	VEHICLEINFO m_vehicleInfo;
	EDGEINFO m_edgeInfo;
	std::recursive_mutex m_mutexCollideState;
	bool m_bCollideState;
    qreal m_shape_r;
	bool m_mouse_hover;
};
