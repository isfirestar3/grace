#include "QRcodeItemManage.h"
#include "MapScene.h"
#include "DataManage.h"


QRcodeItemManage::QRcodeItemManage(MapScene* mapScene)
:m_mapScene(mapScene)
{
	
}


QRcodeItemManage::~QRcodeItemManage()
{
}

void QRcodeItemManage::SetCodeInfo(const std::vector<CODEPOSXYA>& vecNodeInfo)
{
	std::vector<CODEPOSXYA>::const_iterator itNode = vecNodeInfo.begin();
	for (; itNode != vecNodeInfo.end(); ++itNode)
	{
		AddCodeItem(*itNode);
		//CODEPOSXYA posInfo;		
		//posInfo.pos_x = itNode->pos_x;
		//posInfo.pos_y = itNode->pos_y;
		//posInfo.angle = itNode->angle;
		//posInfo.pos_id = itNode->pos_id;
		//DATAMNG->AddQRcodeInfo(posInfo);
	}
}

int QRcodeItemManage::GetMaxCodeId()
{
	int iMaxId = 0;
	for (auto pNodeItem : m_vecCodeItem)
	{
		if (pNodeItem && pNodeItem->GetCodeId() > iMaxId)
		{
			iMaxId = pNodeItem->GetCodeId();
		}
	}
	return iMaxId;
}

QRcodeItem* QRcodeItemManage::AddCodeItem(const QPointF& pos)
{
	CODEPOSXYA posInfo;
	posInfo.pos_x = pos.x();
	posInfo.pos_y= pos.y();
	posInfo.pos_id = GetMaxCodeId() + 1;
	QRcodeItem* pNewNode = AddCodeItem(posInfo);
	DATAMNG->AddQRcodeInfo(posInfo);//UI上画完Node以后也在DATAMNG中保存一份数据

	return pNewNode;
}

QRcodeItem* QRcodeItemManage::AddCodeItem(const CODEPOSXYA& nodeInfo)
{
	if (m_mapScene == NULL)
	{
		return NULL;
	}
	QRcodeItem *nodeItem = new QRcodeItem(m_mapScene);
	if (nodeItem == NULL)
	{
		return NULL;
	}
	nodeItem->SetCodeInfo(nodeInfo);
	m_mapScene->addItem(nodeItem);
	nodeItem->setPos(QPointF(nodeInfo.pos_x, nodeInfo.pos_y));

	AddCodeToVector(nodeItem);

	return nodeItem;
}

void QRcodeItemManage::AddCodeToVector(QRcodeItem* nodeItem)
{
	m_vecCodeItem.push_back(nodeItem);
}

void QRcodeItemManage::GetAllCodeItem(std::vector<QRcodeItem*>& vecNodeItem)
{
	vecNodeItem = m_vecCodeItem;
}

QRcodeItem* QRcodeItemManage::GetCodeItem(int iNodeId)
{
	for (auto pNodeItem : m_vecCodeItem)
	{
		if (pNodeItem)
		{
			if (pNodeItem->GetCodeId() == iNodeId)
			{
				return pNodeItem;
			}
		}
	}
	return nullptr;
}

void QRcodeItemManage::SetItemMoveable(bool bMoveable)
{
	for (auto pNodeItem : m_vecCodeItem)
	{
		if (pNodeItem)
		{
			pNodeItem->SetMoveable(bMoveable);
		}
	}
}

bool QRcodeItemManage::RemoveCodeItem(int node_id)
{
	for (auto itNode = m_vecCodeItem.begin(); itNode != m_vecCodeItem.end(); ++itNode)
	{
		QRcodeItem* pNodeItem = *itNode;
		if (pNodeItem && pNodeItem->GetCodeId() == node_id)
		{
			//pNodeItem->RemoveAllEdge();
			DATAMNG->DeletePosInfo(node_id);
			m_mapScene->removeItem(pNodeItem);
			m_vecCodeItem.erase(itNode);

			delete pNodeItem;
			pNodeItem = NULL;
			return true;
		}
	}
	return false;
}