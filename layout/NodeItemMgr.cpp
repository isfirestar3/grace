#include "NodeItemMgr.h"
#include "NodeItem.h"
#include "MapScene.h"
#include "common_impls.hpp"
#include <cassert>

NodeItemMgr::NodeItemMgr(MapScene *mapScene)
:m_mapScene{mapScene}
{
}


NodeItemMgr::~NodeItemMgr()
{
}

NodeItem *NodeItemMgr::GetNodeItem(int id)
{
	return m_nodeItemMap.contains(id) ? m_nodeItemMap[id].data() : nullptr;
}

void NodeItemMgr::AddNodeItem(const NODEINFO &nodeInfo)
{
	assert(nullptr != m_mapScene);

	if (nullptr == m_mapScene)
	{
		return;
	}

	QSharedPointer<NodeItem> nodeItem = QSharedPointer<NodeItem>::create(m_mapScene);

	if (nodeItem.isNull())
	{
		return;
	}

	nodeItem->SetNodeInfo(nodeInfo);
	nodeItem->setPos(QPointF(nodeInfo.pos_x, nodeInfo.pos_y));
	m_mapScene->addItem(nodeItem.data());
	m_nodeItemMap.insert(nodeInfo.node_id, nodeItem);
}

void NodeItemMgr::Clear()
{
	shr::gui::qMapFree(m_nodeItemMap);
}
