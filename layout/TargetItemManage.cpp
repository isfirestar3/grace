#include "TargetItemManage.h"
#include "MapScene.h"
#include "DataManage.h"

TargetItemManage::TargetItemManage(MapScene* pMapScene)
:m_mapScene(pMapScene)
{

}

TargetItemManage::~TargetItemManage()
{

}

void TargetItemManage::AddTargetItem(const QPointF& pos,const EDGEINFO& edgeInfo)
{
	TARGETINFO targetInfo;
	targetInfo.id = DATAMNG->GetTargetSize() + 1;
	targetInfo.edge_id = edgeInfo.id;
	targetInfo.pos_x = pos.x();
	targetInfo.pos_y = pos.y();
	TargetItem* pTargetItem = AddTargetItem(targetInfo,edgeInfo);
	if (pTargetItem)
	{
		pTargetItem->PopupInfoWnd();
		AddTargetToVector(pTargetItem);
	}
}

TargetItem* TargetItemManage::AddTargetItem(const TARGETINFO& targetInfo, const EDGEINFO& edgeInfo)
{
	if (m_mapScene == NULL)
	{
		return NULL;
	}
	TargetItem *targetItem = new TargetItem();
	if (targetItem == NULL)
	{
		return NULL;
	}
	targetItem->SetTargetInfo(targetInfo);
	targetItem->SetEdgeInfo(edgeInfo);
	m_mapScene->addItem(targetItem);
	targetItem->setPos(QPointF(targetInfo.pos_x, targetInfo.pos_y));
	DATAMNG->AddTargetInfo(targetInfo);
	return targetItem;
}

int TargetItemManage::GetMaxTargetId()
{
	int iMaxId = 0;
	for (auto pTargetItem : m_vecTargetItem)
	{
		if (pTargetItem && pTargetItem->GetTargetId() > iMaxId)
		{
			iMaxId = pTargetItem->GetTargetId();
		}
	}
	return iMaxId;
}

void TargetItemManage::AddTargetToVector(TargetItem* pTargetItem)
{
	m_vecTargetItem.push_back(pTargetItem);
}

bool TargetItemManage::GetTargetInfo(int iTargetId, TARGETINFO& targetInfo)
{
	for (auto pTargetItem : m_vecTargetItem)
	{
		if (pTargetItem && pTargetItem->GetTargetId() == iTargetId)
		{
			pTargetItem->GetTargetInfo(targetInfo);
			return true;
		}
	}
	return false;
}

void TargetItemManage::ResetTargetItem(TargetItem* pTargetItem, const QPointF& pos, const EDGEINFO& edgeInfo)
{
	if (!pTargetItem)
	{
		return;
	}
	TARGETINFO targetInfo;
	pTargetItem->GetTargetInfo(targetInfo);
	targetInfo.pos_x = pos.x();
	targetInfo.pos_y = pos.y();
	targetInfo.edge_id = edgeInfo.id;

	pTargetItem->SetTargetInfo(targetInfo);
	pTargetItem->SetEdgeInfo(edgeInfo);
	pTargetItem->setPos(QPointF(targetInfo.pos_x, targetInfo.pos_y));
	pTargetItem->PopupInfoWnd();
}

void TargetItemManage::DeleteAllTarget()
{
	std::vector<TargetItem*>::const_iterator itTarget = m_vecTargetItem.begin();
	for (; itTarget != m_vecTargetItem.end(); ++itTarget)
	{
		TargetItem* pTargetItem = *itTarget;
		if (pTargetItem)
		{
			m_mapScene->removeItem(pTargetItem);
			delete pTargetItem;
			pTargetItem = NULL;
		}
	}
	m_vecTargetItem.clear();
	DATAMNG->DeleteAllTargetInfo();//同时将DATAMNG中的所有目标点数据都删除
}

void TargetItemManage::SetTargetInfo(const std::vector<TARGETINFO>& vecTarget)
{
	for (auto targetInfo : vecTarget)
	{
		EDGEINFO edgeInfo;
		DATAMNG->GetEdgeById(targetInfo.edge_id, edgeInfo);
		TargetItem* pTargetItem = AddTargetItem(targetInfo, edgeInfo);
		if (pTargetItem)
		{
			AddTargetToVector(pTargetItem);
		}
	}
}