#include "EdgeItemMgr.h"
#include "MapScene.h"
#include "DataManage.h"
#include "common_impls.hpp"

EdgeItemMgr::EdgeItemMgr(MapScene *mapScene)
:m_mapScene{mapScene}
{
}

EdgeItemMgr::~EdgeItemMgr()
{
}

void EdgeItemMgr::AddEdgeItem(NodeItem *pStartNode, NodeItem *pEndNode, const EDGEINFO &edgeInfo)
{
	if (nullptr == m_mapScene)
	{
		return;
	}

	QSharedPointer<EdgeItem> edgeItem = QSharedPointer<EdgeItem>::create(pStartNode, pEndNode);

	if (edgeItem.isNull())
	{
		return;
	}

	edgeItem->SetEdgeInfo(edgeInfo);
	edgeItem->UpdateCtrlPoint(QPointF(edgeInfo.ctrl_point1.x, edgeInfo.ctrl_point1.y), QPointF(edgeInfo.ctrl_point2.x, edgeInfo.ctrl_point2.y));
	edgeItem->Adjust();
	std::vector<WOP> vecWopInfo;
	DATAMNG->GetAllWopInfo(vecWopInfo);
	std::vector<int> vecWopId;

	for (auto wopInfo : vecWopInfo)
	{
		vecWopId.push_back(wopInfo.id);
	}

	edgeItem->SetEdgeWopList(vecWopId);
	m_mapScene->addItem(edgeItem.data());
	m_edgeItemMap.insert(edgeInfo.id, edgeItem);
}

void EdgeItemMgr::Clear()
{
	shr::gui::qMapFree(m_edgeItemMap);
}
