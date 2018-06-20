#pragma once
#include "BasicDefine.h"
#include <QtCore/QMap>
#include <QtCore/QSharedPointer>

class MapScene;
class NodeItem;

class NodeItemMgr
{
public:
	NodeItemMgr(MapScene *mapScene);
	~NodeItemMgr();

	NodeItem *GetNodeItem(int id);
	void AddNodeItem(const NODEINFO &nodeInfo);
	void Clear();
private:
	MapScene *m_mapScene{nullptr};
	QMap<int, QSharedPointer<NodeItem>> m_nodeItemMap;
};

