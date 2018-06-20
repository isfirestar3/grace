#include "NodeItemManage.h"
#include "MapScene.h"
#include "DataManage.h"


NodeItemManage::NodeItemManage(MapScene* mapScene)
:m_mapScene(mapScene)
{
	
}


NodeItemManage::~NodeItemManage()
{
}

void NodeItemManage::SetNodeInfo(const std::vector<NODEINFO>& vecNodeInfo)
{
	std::vector<NODEINFO>::const_iterator itNode = vecNodeInfo.begin();
	for (; itNode != vecNodeInfo.end(); ++itNode)
	{
		AddNodeItem(*itNode);
	}
}

int NodeItemManage::GetMaxNodeId()
{
	int iMaxId = 0;
	for (auto pNodeItem : m_vecNodeItem)
	{
		if (pNodeItem && pNodeItem->GetNodeId() > iMaxId)
		{
			iMaxId = pNodeItem->GetNodeId();
		}
	}
	return iMaxId;
}

NodeItem* NodeItemManage::AddNodeItem(const QPointF& pos)
{
	NodeInfo nodeInfo;
	nodeInfo.pos_x = pos.x();
	nodeInfo.pos_y = pos.y();
	nodeInfo.node_id = GetMaxNodeId() + 1;
	NodeItem* pNewNode = AddNodeItem(nodeInfo);
	DATAMNG->AddNodeInfo(nodeInfo);//UI上画完Node以后也在DATAMNG中保存一份数据

	return pNewNode;
}

NodeItem* NodeItemManage::AddNodeItem(const NodeInfo& nodeInfo)
{
	if (m_mapScene == NULL)
	{
		return NULL;
	}
	NodeItem *nodeItem = new NodeItem(m_mapScene);
	if (nodeItem == NULL)
	{
		return NULL;
	}
	nodeItem->SetNodeInfo(nodeInfo);
	m_mapScene->addItem(nodeItem);
	nodeItem->setPos(QPointF(nodeInfo.pos_x, nodeInfo.pos_y));

	AddNodeToVector(nodeItem);
	
	return nodeItem;
}

void NodeItemManage::AddNodeToVector(NodeItem* nodeItem)
{
	m_vecNodeItem.push_back(nodeItem);
}

void NodeItemManage::GetAllNodeItem(std::vector<NodeItem*>& vecNodeItem)
{
	vecNodeItem = m_vecNodeItem;
}

NodeItem* NodeItemManage::GetNodeItem(int iNodeId)
{
	for (auto pNodeItem : m_vecNodeItem)
	{
		if (pNodeItem)
		{
			if (pNodeItem->GetNodeId() == iNodeId)
			{
				return pNodeItem;
			}
		}
	}
	return nullptr;
}

void NodeItemManage::SetItemMoveable(bool bMoveable)
{
	for (auto pNodeItem : m_vecNodeItem)
	{
		if (pNodeItem)
		{
			pNodeItem->SetMoveable(bMoveable);
		}
	}
}

bool NodeItemManage::RemoveNodeItem(int node_id)
{
	for (auto itNode = m_vecNodeItem.begin(); itNode != m_vecNodeItem.end(); ++itNode)
	{
		NodeItem* pNodeItem = *itNode;
		if (pNodeItem && pNodeItem->GetNodeId() == node_id)
		{
			pNodeItem->RemoveAllEdge();
			DATAMNG->DeleteNodeInfo(node_id);
			m_mapScene->removeItem(pNodeItem);
			m_vecNodeItem.erase(itNode);

			delete pNodeItem;
			pNodeItem = NULL;
			return true;
		}
	}
	return false;
}

bool NodeItemManage::UpdateNodeItem(int node_id,bool spin)
{
	for (auto itNode = m_vecNodeItem.begin(); itNode != m_vecNodeItem.end(); ++itNode)
	{
		NodeItem* pNodeItem = *itNode;
		if (pNodeItem && pNodeItem->GetNodeId() == node_id)
		{
			pNodeItem->UpdateNodeInfo(spin);
		}
	}
	return false;
}