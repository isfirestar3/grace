#include "AnchorItemManage.h"
#include "MapScene.h"
#include "DataManage.h"
#include "point_algorithm.hpp"

AnchorItemManage::AnchorItemManage(MapScene* pMapScene)
:m_mapScene(pMapScene)
{
}


AnchorItemManage::~AnchorItemManage()
{
}

void AnchorItemManage::AddAnchorItem(const QPointF& pos, const EDGEINFO& edgeInfo)
{
	ANCHORINFO anchorInfo;
	anchorInfo.id = GetMaxAnchorId() + 1;
	anchorInfo.edge_id = edgeInfo.id;
	anchorInfo.pos_x = pos.x();
	anchorInfo.pos_y = pos.y();
	position_t pos_t;
	pos_t.x_ = anchorInfo.pos_x;
	pos_t.y_ = anchorInfo.pos_y;
	pos_t.angle_ = 0;
	UPL upl;
	PT_ALG::point_mapping_upl(pos_t, upl);//¸ù¾ÝÎ»ÖÃËãUPL
	anchorInfo.percent = upl.percent;
	AnchorItem* pAnchorItem = AddAnchorItem(anchorInfo, edgeInfo);
	if (pAnchorItem)
	{
		pAnchorItem->PopupInfoWnd();
		AddAnchorItemToVector(pAnchorItem);
	}
}

int AnchorItemManage::GetMaxAnchorId()
{
	int iMaxId = 0;
	for (auto pAnchorItem : m_vecAnchorItem)
	{
		if (pAnchorItem && pAnchorItem->GetAnchorId() > iMaxId)
		{
			iMaxId = pAnchorItem->GetAnchorId();
		}
	}
	return iMaxId;
}

AnchorItem* AnchorItemManage::AddAnchorItem(const ANCHORINFO& anchorInfo, const EDGEINFO& edgeInfo)
{
	if (m_mapScene == NULL)
	{
		return NULL;
	}
	AnchorItem *anchorItem = new AnchorItem();
	if (anchorItem == NULL)
	{
		return NULL;
	}
	anchorItem->SetAnchorInfo(anchorInfo);
	anchorItem->SetEdgeInfo(edgeInfo);
	m_mapScene->addItem(anchorItem);
	anchorItem->setPos(QPointF(anchorInfo.pos_x, anchorInfo.pos_y));
	anchorItem->setRotation(anchorInfo.angle);
	DATAMNG->AddAnchorInfo(anchorInfo);
	return anchorItem;
}

void AnchorItemManage::AddAnchorItemToVector(AnchorItem* pAnchorItem)
{
	m_vecAnchorItem.push_back(pAnchorItem);
}

void AnchorItemManage::SetAnchorInfo(const std::vector<ANCHORINFO>& vecAnchor)
{
	for (auto anchorInfo : vecAnchor)
	{
		EDGEINFO edgeInfo;
		DATAMNG->GetEdgeById(anchorInfo.edge_id, edgeInfo);
		AnchorItem* pAnchorItem = AddAnchorItem(anchorInfo, edgeInfo);
		if (pAnchorItem)
		{
			AddAnchorItemToVector(pAnchorItem);
		}
	}
}

void AnchorItemManage::RemoveAllTargetItem()
{
	for (auto &anchorItem : m_vecAnchorItem)
	{
		if (anchorItem && anchorItem->IsTarget())
		{
			anchorItem->ChangeToTarget(false);
		}
	}
}

bool AnchorItemManage::RemoveAnchorItem(int anchor_id)
{
	auto itAnchor = m_vecAnchorItem.begin();
	for (; itAnchor != m_vecAnchorItem.end(); ++itAnchor)
	{
		AnchorItem* pAnchorItem = *itAnchor;
		if (pAnchorItem && pAnchorItem->GetAnchorId() == anchor_id)
		{
			DATAMNG->DeleteAnchorInfo(anchor_id);
			m_mapScene->removeItem(pAnchorItem);
			m_vecAnchorItem.erase(itAnchor);
			delete pAnchorItem;
			pAnchorItem = NULL;
			return true;
		}
	}
	return false;
}

void AnchorItemManage::UpdateAnchorInfo(const ANCHORINFO& anchorInfo)
{
	for (auto itItem = m_vecAnchorItem.begin(); itItem != m_vecAnchorItem.end(); ++itItem){
		AnchorItem* pAnchorItem = *itItem;
		if (pAnchorItem && pAnchorItem->GetAnchorId() == anchorInfo.id){
			pAnchorItem->SetAnchorInfo(anchorInfo);
			DATAMNG->AddAnchorInfo(anchorInfo);
		}
	}
}

void AnchorItemManage::GetAnchorInfoOnEdge(int edge_id, std::vector<ANCHORINFO>& vecAnchor)
{
	for (auto itItem = m_vecAnchorItem.begin(); itItem != m_vecAnchorItem.end(); ++itItem){
		AnchorItem* pAnchorItem = *itItem;
		if (pAnchorItem && pAnchorItem->GetEdgeId() == edge_id){
			ANCHORINFO anchorInfo;
			pAnchorItem->GetAnchorInfo(anchorInfo);
			vecAnchor.push_back(anchorInfo);
		}
	}
}
