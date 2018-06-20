#include "VehicleItemManage.h"
#include "MapScene.h"
#include "Geometry.h"
#include "DataManage.h"


VehicleItemManage::VehicleItemManage(MapScene* pMapScene)
: m_pMapScene(pMapScene)
{
	//CFGFILE_INSTANCE->ReadFile("VehicleInit.ini");
}


VehicleItemManage::~VehicleItemManage()
{
}

VehicleItem* VehicleItemManage::AddVehicleItem(const QPointF& pos, const EDGEINFO& edgeInfo,bool manual)
{
	std::lock_guard<decltype(m_mutexVehicleItem)> guard(m_mutexVehicleItem);
	if (!m_vecVehicleItem.empty())
	{
		VehicleItem* pVehicleItem = m_vecVehicleItem.at(0);
		ResetVehicleItem(pVehicleItem, pos, edgeInfo);
		return pVehicleItem;
	}
	VEHICLEINFO vehicleInfo;
	vehicleInfo.vehicle_id = GetMaxVehicleId() + 1;
	vehicleInfo.pos_x = pos.x();
	vehicleInfo.pos_y = pos.y();
	vehicleInfo.angle = 90.0;
	vehicleInfo.edge_id = edgeInfo.id;
	return AddVehicleItem(vehicleInfo,manual);
}

VehicleItem* VehicleItemManage::AddVehicleItem(const VEHICLEINFO& vehicleInfo,bool manual)
{
	if (!m_pMapScene)
	{
		return NULL;
	}
	VehicleItem* pVehicleItem = new VehicleItem();
	if (pVehicleItem == NULL)
	{
		return NULL;
	}
	pVehicleItem->SetVehicleInfo(vehicleInfo);
	m_pMapScene->addItem(pVehicleItem);
	pVehicleItem->setPos(vehicleInfo.pos_x, vehicleInfo.pos_y);
	pVehicleItem->setRotation(vehicleInfo.angle - 90.0);
	{
		std::lock_guard<decltype(m_mutexVehicleItem)> guard(m_mutexVehicleItem);
		m_vecVehicleItem.push_back(pVehicleItem);
	}
	//UIDATA->AddVehicleInfo(vehicleInfo);
	if (manual)
	{
		pVehicleItem->PopupInfoWnd();
	}
	return pVehicleItem;
}

VehicleItem* VehicleItemManage::FindVehicle(int iVehicleId)
{
	std::lock_guard<decltype(m_mutexVehicleItem)> guard(m_mutexVehicleItem);
	std::vector<VehicleItem*>::const_iterator itVehicle = m_vecVehicleItem.begin();
	for (; itVehicle != m_vecVehicleItem.end(); ++itVehicle)
	{
		VehicleItem* pVehicleItem = *itVehicle;
		if (pVehicleItem == NULL)
		{
			continue;
		}
		if (pVehicleItem->GetVehicleId() == iVehicleId)
		{
			return pVehicleItem;
		}
	}
	return NULL;
}

void VehicleItemManage::UpdateVehiclePos(int iVehicleId,const POSXYA& posXya)
{
	std::lock_guard<decltype(m_mutexVehicleItem)> guard(m_mutexVehicleItem);

	for (auto &iter : m_vecVehicleItem) 
	{
		assert(iter);
		if (iter->GetVehicleId() == iVehicleId)
		{
			iter->SetVehiclePos(posXya);
			iter->setPos(posXya.pos_x, posXya.pos_y);//设置机器人位置
			iter->setRotation(posXya.angle + 270);//设置机器人角度

			m_pMapScene->update(posXya.pos_x - 5000, posXya.pos_y - 5000, 10000, 10000);
		}
	}
}

bool VehicleItemManage::CollisionDetect(int iVehicleId)
{
	std::lock_guard<decltype(m_mutexVehicleItem)> guard(m_mutexVehicleItem);
	for (auto &iter : m_vecVehicleItem)
	{
		assert(iter);
		if (iter->GetVehicleId() == iVehicleId)
		{
			for (auto &itOther : m_vecVehicleItem)
			{
				if (itOther->GetVehicleId() != iVehicleId && iter->collidesWithItem(itOther))
				{
					iter->SetCollideState(true);
					itOther->SetCollideState(true);
					std::cout << "vehicle: " << iVehicleId << "collide with vehicle:" << itOther->GetVehicleId() << std::endl;
					return true;
				}
			}
			break;
		}
	}
	return false;
}

int VehicleItemManage::GetMaxVehicleId()
{
	int iMaxId = 0;
	std::lock_guard<decltype(m_mutexVehicleItem)> guard(m_mutexVehicleItem);
	for (auto &pVehicle : m_vecVehicleItem)
	{
		assert(pVehicle);
		if (pVehicle->GetVehicleId() > iMaxId)
		{
			iMaxId = pVehicle->GetVehicleId();
		}
	}
	return iMaxId;
}

bool VehicleItemManage::GetVehicleInfo(int iVehicleId, VEHICLEINFO& vehicleInfo)
{
	std::lock_guard<decltype(m_mutexVehicleItem)> guard(m_mutexVehicleItem);
	for (auto &pVehicle : m_vecVehicleItem)
	{
		assert(pVehicle);
		if (pVehicle->GetVehicleId() == iVehicleId)
		{
			pVehicle->GetVehicleInfo(vehicleInfo);
			return true;
		}
	}
	return false;
}

void VehicleItemManage::ResetVehicleItem(VehicleItem* pVehicleItem, const QPointF& pos, const EDGEINFO& edgeInfo)
{
	if (!pVehicleItem)
	{
		return;
	}
	VEHICLEINFO vehicleInfo;
	pVehicleItem->GetVehicleInfo(vehicleInfo);
	vehicleInfo.pos_x = pos.x();
	vehicleInfo.pos_y = pos.y();
	vehicleInfo.edge_id = edgeInfo.id;

	pVehicleItem->SetVehicleInfo(vehicleInfo);
	pVehicleItem->SetEdgeInfo(edgeInfo);
	pVehicleItem->setPos(QPointF(vehicleInfo.pos_x, vehicleInfo.pos_y));
	pVehicleItem->PopupInfoWnd();
}

void VehicleItemManage::SetVehiclePos(int iVehicleId, const POSXYA& pos)
{
	VehicleItem* pVehicleItem = FindVehicleItem(iVehicleId);
	if (!pVehicleItem)
	{
		EDGEINFO edgeInfo;
		QPointF vehiclePos(pos.pos_x, pos.pos_y);
		pVehicleItem = AddVehicleItem(vehiclePos, edgeInfo,false);
	}
	if (pVehicleItem)
	{
		VEHICLEINFO vehicleInfo;
		DATAMNG->GetVehicleInfo(iVehicleId, vehicleInfo);
		pVehicleItem->SetVehicleInfo(vehicleInfo);
		pVehicleItem->SetVehiclePos(pos);
		QPointF point(pos.pos_x, pos.pos_y);
		pVehicleItem->setPos(point);
		pVehicleItem->setRotation(pos.angle + 270);//设置机器人角度
	}
}

VehicleItem* VehicleItemManage::FindVehicleItem(int iVehicleId)
{
	std::lock_guard<decltype(m_mutexVehicleItem)> guard(m_mutexVehicleItem);
	std::vector<VehicleItem*>::iterator itVehicle = m_vecVehicleItem.begin();
	for (; itVehicle != m_vecVehicleItem.end(); ++itVehicle)
	{
		VehicleItem* pVehicleItem = *itVehicle;
		if (pVehicleItem && pVehicleItem->GetVehicleId() == iVehicleId)
		{
			return pVehicleItem;
		}
	}
	return NULL;
}

void VehicleItemManage::SetVehicleInfo(const VEHICLEINFO& vehicleInfo)
{
	VehicleItem* pVehicleItem = FindVehicleItem(vehicleInfo.vehicle_id);
	if (!pVehicleItem)
	{
		pVehicleItem->SetVehicleInfo(vehicleInfo);
	}
}

