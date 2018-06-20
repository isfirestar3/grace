#pragma once
#include "BasicDefine.h"
#include <QtCore/QMap>
#include <QtCore/QSharedPointer>

class MapScene;
class NodeItem;
class EdgeItem;

class EdgeItemMgr
{
public:
	EdgeItemMgr(MapScene *mapScene);
	~EdgeItemMgr();

	void AddEdgeItem(NodeItem *pStartNode, NodeItem *pEndNode, const EDGEINFO &edgeInfo);
	void Clear();
private:
	MapScene *m_mapScene{ nullptr };
	QMap<int, QSharedPointer<EdgeItem>> m_edgeItemMap;
};

